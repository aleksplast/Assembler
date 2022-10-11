#include <stdio.h>

#include "assembler.h"

int main()
{
    FILE* logs = fopen("logs.txt", "w");
    fclose(logs);

    const char* input = "input.txt";

    struct asemblr asemblr = {};

    AsmCtor(&asemblr, input);

    Assembler(&asemblr);

    Assembler(&asemblr);

    AsmDetor(&asemblr);

    return 0;
}
