#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "text-sort.h"
#include "stack.h"

const int version = 1;

int Assembler(const char* input);

int CheckReg(const char* reg);

int GetArg(char* arg, struct asemblr* asemblr);

int GetLabel(char* arg, struct asemblr* asemblr);

struct asemblr
{
    char* code;
    int labels[10];
    int ip;
};

#define DEF_CMD(name, num, arg, code) \
    CMD_##name = num,
enum Commands
{
#include "cmd.h"
    NOCMD = 18
};
#undef DEF_CMD

enum ComArgs{
    ARG_MEM     = 128,
    ARG_REG     = 64,
    ARG_IMMED   = 32
};

#endif //ASSEMBLER_H
