#include <stdio.h>

#include "assembler.h"

int main()
{
    FILE* logs = fopen("logs.txt", "w");
    fclose(logs);

    const char* input = "factorialrec.txt";

    struct asemblr asemblr = {};

    AsmCtor(&asemblr, input);

    AssemblerMain(&asemblr);

    AssemblerMain(&asemblr);

    AsmDetor(&asemblr);

    return 0;
}
