#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "assembler.h"

int AssemblerMain(struct asemblr* asemblr)
{
    int counter = 0;

    asemblr->ip = sizeof(asemblr->info);

    while (counter < asemblr->text.nlines)
    {
        char cmd[10] = "";
        int len = 0;

        if (CheckForComment(asemblr->text.Strings[counter].ptr))
            counter += 1;

        char* line = asemblr->text.Strings[counter].ptr;

        ListingPrint(asemblr->listing, &asemblr->ip, sizeof(int));
        fprintf(asemblr->listing, " | %-10s |", line);

        sscanf(line, "%s%n", cmd, &len);

#define DEF_CMD(name, num, arg, cod)                                                \
        if (stricmp(cmd, #name) == 0)                                               \
        {                                                                           \
            asemblr->code[asemblr->ip] = CMD_##name;                                \
            if  (arg == 1)                                                          \
                GetArg(line + len + 1, asemblr);                                    \
            else                                                                    \
                fprintf(asemblr->listing, " %02X", num);                            \
            fprintf(asemblr->listing, "\n");                                        \
            asemblr->ip++;                                                          \
        }                                                                           \
        else
#include "..\Assembler\cmd.h"
#undef DEF_CMD

        if (strchr(cmd, ':'))
        {
            PushLabel(asemblr, cmd);
        }
        counter++;
    }

    *(int*) asemblr->code = asemblr->info.sign;
    *(int*) (asemblr->code + sizeof(int)) = asemblr->info.version;
    *(int*) (asemblr->code + 2 * sizeof(int)) = asemblr->ip - 3 * sizeof(int);

    FILE* out = fopen("out.txt", "wb");

    if (out == NULL)
        return FILERR;

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

    asemblr->info.sign = 'C' + 256 * 'P';
    asemblr->info.version = version;

    TextReader(input, &asemblr->text, mode);

    LinesSeparator(&asemblr->text, '\n');

    asemblr->listing = fopen("listing.txt", "w");

    if ((asemblr->code = (char*) calloc(asemblr->text.nlines * (sizeof(char) * 2 + sizeof(elem_t)) + 3 * sizeof(int), sizeof(char))) == NULL)
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
    char reg[30] = {};
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

        fprintf(asemblr->listing, " %02X ", asemblr->code[asemblr->ip]);
        ListingPrint(asemblr->listing, (int*)(asemblr->code + asemblr->ip + 1), sizeof(int));

        asemblr->ip += sizeof(int);
    }
    else if (*arg == '[')
    {
        sscanf(arg, "%s%n", reg, &len);

        if (*(arg + len - 1) == ']')
        {
            asemblr->code[asemblr->ip] |= ARG_MEM;
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

        fprintf(asemblr->listing, " %02X ", asemblr->code[asemblr->ip]);
        ListingPrint(asemblr->listing, &val, sizeof(elem_t));
        ListingPrint(asemblr->listing, &(asemblr->code[asemblr->ip + sizeof(elem_t)]), sizeof(char));

        asemblr->ip += sizeof(elem_t) + sizeof(char);
    }
    else if (sscanf(arg, "%lg", &val) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_IMMED;
        *(elem_t*)(asemblr->code + asemblr->ip + 1) = val;

        fprintf(asemblr->listing, " %02X ", asemblr->code[asemblr->ip]);
        ListingPrint(asemblr->listing, &val, sizeof(elem_t));

        asemblr->ip += sizeof(elem_t);
    }
    else if (sscanf(arg, "%s", reg) == 1)
    {
        asemblr->code[asemblr->ip] = asemblr->code[asemblr->ip] | ARG_REG;
        asemblr->code[asemblr->ip + 1] = CheckReg(reg);

        fprintf(asemblr->listing, " %02X %02X", asemblr->code[asemblr->ip], asemblr->code[asemblr->ip + 1]);

        asemblr->ip += 1;
    }
    else
        return ARGERR;

    return NOERR;
}

const char* GetComArg(int argc, char* argv[])
{
    const char* input = "input.txt";

    if (argc > 1)
        return argv[1];
    else
        return input;
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

int ListingPrint(FILE* out, void* arg, size_t size)
{
    char* val = (char*) arg;
    for (int i = 0; i < size; i++)
        if (*(val + i) >= 0)
            fprintf(out, "%02X ", *(val + i));
        else
            fprintf(out, "%02X ", (-1) * *(val + i));

    return NOERR;
}

bool CheckForComment(char* line)
{
    char* commentptr = NULL;

    if ((commentptr = strchr(line, '#')) != NULL)
    {
        if (commentptr == line)
            return true;
        else
            *commentptr = '\0';
    }
    else
        return false;

    return false;
}

int PushLabel(struct asemblr* asemblr, char* cmd)
{
    int label = 0;
    sscanf(cmd, "%d", &label);
    CheckLabel(asemblr, label);
    asemblr->labels.labelsarray[label] = asemblr->ip - sizeof(asemblr->info);
    fprintf(asemblr->listing, "\n");

    return NOERR;
}
