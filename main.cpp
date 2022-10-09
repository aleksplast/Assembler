#include <stdio.h>

#include "assembler.h"

int main()
{
    FILE* fp = fopen("logs.txt", "w");
    fclose(fp);
    const char* input = "input.txt";

    Assembler(input);

    return 0;
}
