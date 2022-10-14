#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#include "assembler.h"

int AssemblerMain(struct asemblr* asemblr)
{
    int counter = 0;

    asemblr->ip = 3 * sizeof(int);

    while (counter < asemblr->text.nlines)
    {
        char cmd[10];
        int len = 0;

        char* line = asemblr->text.Strings[counter].ptr;

        fprintf(asemblr->listing, "%-4d | %-8s |", asemblr->ip, line);

        sscanf(line, "%s%n", cmd, &len);

#define DEF_CMD(name, num, arg, cod)                                        \
if (stricmp(cmd, #name) == 0)                                               \
{                                                                           \
    asemblr->code[asemblr->ip] = CMD_##name;                                \
    if  (arg == 1)                                                          \
    {                                                                       \
        GetArg(line + len + 1, asemblr);                                    \
    }                                                                       \
    else                                                                    \
        fprintf(asemblr->listing, " %d", num);                              \
    fprintf(asemblr->listing, "\n");                                        \
    asemblr->ip++;                                                          \
}                                                                           \
else
#include "..\Assembler\cmd.h"
#undef DEF_CMD

        if (strchr(cmd, ':'))
        {
            int label = 0;
            sscanf(cmd, "%d", &label);
            CheckLabel(asemblr, label);
            asemblr->labels.labelsarray[label] = asemblr->ip - 3 * sizeof(int);
            fprintf(asemblr->listing, "\n");
        }
        counter++;
    }

    *(int*) asemblr->code = 'C' + 256 * 'P';
    *(int*) (asemblr->code + sizeof(int)) = version;
    *(int*) (asemblr->code + 2 * sizeof(int)) = asemblr->ip - 3 * sizeof(int);

    FILE* out = fopen("out.txt", "w");

    fwrite(asemblr->code, sizeof(char), asemblr->ip, out);

    fclose(out);

    return NOERR;
}

int AsmCtor(struct asemblr* asemblr, const char* input)
{
    const char* mode = "r";
    const char end = '\n';

    asemblr->text = {NULL, 0, 0, NULL};

    if ((asemblr->labels.labelsarray = (int*) calloc(10, sizeof(int))) == NULL)
        return MEMERR;

    asemblr->labels.labelsnum = 10;

    for (int i = 0; i < 10; i++)
        asemblr->labels.labelsarray[i] = 0;

    TextReader(input, &asemblr->text, mode);

    LinesSeparator(&asemblr->text, '\n');

    asemblr->listing = fopen("listing.txt", "w");

    if ((asemblr->code = (char*) calloc(asemblr->text.nlines * (sizeof(char) * 2 + sizeof(int)) + 3 * sizeof(int), sizeof(char))) == NULL)
        return MEMERR;
}

int AsmDetor(struct asemblr* asemblr)
{
    free(asemblr->text.Strings);
    free(asemblr->code);
    fclose(asemblr->listing);
    asemblr->code = NULL;
    asemblr->ip = -1;

    return NOERR;
}

int GetArg(char* arg, struct asemblr* asemblr)
{
    char reg[30];
    elem_t val = 0;
    int len = 0, label = 0;
    char* labelptr = NULL;

    if ((labelptr = strchr(arg, ':')) != NULL)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;

        if (sscanf(arg + 1, "%d", &label) != 1)
            return LABELERR;

        CheckLabel(asemblr, label);

        *(int*)(asemblr->code + asemblr->ip + 1) = asemblr->labels.labelsarray[label];

        fprintf(asemblr->listing, " %d %d", asemblr->code[asemblr->ip], label);

        asemblr->ip += sizeof(int);
    }
    else if (*arg == '[')
    {
        sscanf(arg, "%s%n", reg, &len);
        if (*(arg + len - 1) == ']')
        {
            asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_MEM;
            *(arg + len - 1) = ' ';
        }
        GetArg(arg + 1, asemblr);
        *(arg + len - 1) = ']';
    }
    else if (sscanf(arg, "%lg+%s", &val, reg) == 2)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;
        *(elem_t*)(asemblr->code + asemblr->ip + 1) = val;

        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_REG;
        asemblr->code[asemblr->ip + sizeof(elem_t) + 1] = CheckReg(reg);

        fprintf(asemblr->listing, " %d %lg %d", asemblr->code[asemblr->ip], val, asemblr->code[asemblr->ip + sizeof(elem_t) + 1]);

        asemblr->ip += sizeof(elem_t) + sizeof(char);
    }
    else if (sscanf(arg, "%lg", &val) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;
        *(elem_t*)(asemblr->code + asemblr->ip + 1) = val;

        fprintf(asemblr->listing, " %d %lg", asemblr->code[asemblr->ip], val);

        asemblr->ip += sizeof(elem_t);
    }
    else if (sscanf(arg, "%s", reg) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_REG;
        asemblr->code[asemblr->ip + 1] = CheckReg(reg);

        fprintf(asemblr->listing, " %d", asemblr->code[asemblr->ip]);

        asemblr->ip += 1;
    }
    else
        return ARGERR;

    return NOERR;
}

int CheckReg(const char* reg)
{
    int lenght = strlen(reg);

    if (*reg == 'r' && *(reg + 2) == 'x' && lenght == 3)
        if ((int)*(reg + 1) <= 101 && (int) *(reg + 1) >= 97)
            return *(reg + 1) - 'a' + 1;
        else
        {
            printf("Compilation error: unknown register\n");
            return REGERR;
        }
    else
    {
        printf("Compilation error: unknown register\n");
        return REGERR;
    }

    return NOERR;
}

int CheckLabel(struct asemblr* asemblr, int label)
{
    if (asemblr->labels.labelsnum <= label)
    {
        int* prev = asemblr->labels.labelsarray;

        int* buffer = (int*) realloc(asemblr->labels.labelsarray, sizeof(int) * (label * 2));

        if (buffer == NULL)
            return MEMERR;

        asemblr->labels.labelsarray = buffer;

        for (int i = asemblr->labels.labelsnum; i < label + 2; i++)
            asemblr->labels.labelsarray[i] = 0;

        if (prev != asemblr->labels.labelsarray)
            free(prev);

        asemblr->labels.labelsnum = label * 2;

        return NOERR;
    }
    else
        return NOERR;
}
