#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "text-sort.h"
#include "stack.h"

const int version = 1;

int Assembler(struct asemblr* asemblr);

int AsmCtor(struct asemblr* asemblr, const char* input);

int AsmDetor(struct asemblr* asemblr);

int CheckReg(const char* reg);

int GetArg(char* arg, struct asemblr* asemblr);

int GetLabel(char* arg, struct asemblr* asemblr);

struct asemblr
{
    struct poem text;
    char* code;
    int labels[10];
    int ip;
};

enum AsmErrors
{
    REGERR = 1,
    LABELERR,
    ARGERR,
};

#define DEF_CMD(name, num, arg, code)                                   \
    CMD_##name = num,
enum Commands
{
#include "C:\Users\USER\Documents\GitHub\Assembler\cmd.h"
    NOCMD = 18
};
#undef DEF_CMD

enum ComArgs{
    ARG_MEM     = 128,
    ARG_REG     = 64,
    ARG_IMMED   = 32
};

#endif //ASSEMBLER_H
