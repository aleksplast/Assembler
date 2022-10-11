#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "assembler.h"

int Assembler(struct asemblr* asemblr)
{
    int counter = 0;

    asemblr->ip = 3 * sizeof(int);

    while (counter < asemblr->text.nlines)
    {
        char cmd[10];
        int len = 0;

        char* line = asemblr->text.Strings[counter].ptr;

        sscanf(line, "%s%n", cmd, &len);

#define DEF_CMD(name, num, arg, cod)                                        \
if (stricmp(cmd, #name) == 0)                                               \
{                                                                           \
    asemblr->code[asemblr->ip] = CMD_##name;                                \
    if  (arg == 1)                                                          \
    {                                                                       \
        GetArg(line + len + 1, asemblr);                                    \
    }                                                                       \
    if (arg == 2)                                                           \
    {                                                                       \
        GetLabel(line + len + 1, asemblr);                                  \
    }                                                                       \
    asemblr->ip++;                                                          \
}                                                                           \
else
#include "C:\Users\USER\Documents\GitHub\Assembler\cmd.h"
#undef DEF_CMD

        if (strchr(cmd, ':'))
        {
            int label = 0;
            sscanf(cmd, "%d", &label);
            asemblr->labels[label] = asemblr->ip - 3 * sizeof(int);
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

    for (int i = 0; i < 5; i++)
        asemblr->labels[i] = 0;

    TextReader(input, &asemblr->text, mode);

    LinesSeparator(&asemblr->text, '\n');

    asemblr->code = (char*) calloc(asemblr->text.nlines * (sizeof(char) * 2 + sizeof(int)) + 3 * sizeof(int), sizeof(char));
}

int AsmDetor(struct asemblr* asemblr)
{
    free(asemblr->text.Strings);
    free(asemblr->code);
    asemblr->code = NULL;
    asemblr->ip = -1;

    return NOERR;
}

int GetArg(char* arg, struct asemblr* asemblr)
{
    char reg[30];
    int val = 0;
    int len = 0;

    if (*arg == '[')
    {
        sscanf(arg, "%s%n", reg, &len);
        if (*(arg + len - 1) == ']')
        {
            asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_MEM;
            *(arg + len - 1) = ' ';
        }
        GetArg(arg + 1, asemblr);
    }
    else if (sscanf(arg, "%d+%s", &val, reg) == 2)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;
        *(int*)(asemblr->code + asemblr->ip + 1) = val;

        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_REG;
        asemblr->code[asemblr->ip + sizeof(int) + 1] = CheckReg(reg);

        asemblr->ip += sizeof(int) + sizeof(char);
    }
    else if (sscanf(arg, "%d", &val) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;
        *(int*)(asemblr->code + asemblr->ip + 1) = val;

        asemblr->ip += sizeof(int);
    }
    else if (sscanf(arg, "%s", reg) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_REG;
        asemblr->code[asemblr->ip + 1] = CheckReg(reg);

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

int GetLabel(char* arg, struct asemblr* asemblr)
{
    int label = 0;
    char* labelptr = NULL;
    if ((labelptr = strchr(arg, ':')) != NULL)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;

        if (sscanf(arg + 1, "%d", &label) != 1)
            return LABELERR;

        *(int*)(asemblr->code + asemblr->ip + 1) = asemblr->labels[label];
        asemblr->ip += sizeof(int);
    }
    else
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;

        if (sscanf(arg, "%d", &label) != 1)
            return LABELERR;

        *(int*)(asemblr->code + asemblr->ip + 1) = label;
        asemblr->ip += sizeof(int);
    }

    return NOERR;
}


