#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "text-sort.h"

int AssemblerMain(struct asemblr* asemblr);

int AsmCtor(struct asemblr* asemblr, const char* input);

int AsmDetor(struct asemblr* asemblr);

int CheckReg(const char* reg);

int GetArg(char* arg, struct asemblr* asemblr);

int CheckLabel(struct asemblr* asemblr, int label);

struct labels
{
    int* labelsarray;
    int labelsnum;
};

struct asemblr
{
    FILE* listing;
    struct poem text;
    char* code;
    struct labels labels;
    int ip;
};

enum AsmErrors
{
    REGERR = 1,
    LABELERR,
    ARGERR,
};

#endif //ASSEMBLER_H
