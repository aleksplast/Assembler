#define _ ,

DEF_CMD(PUSH, 1, 1,
    {
        StackPush(&stk, SetArg(&cpu, cmd));
    })

DEF_CMD(ADD, 2 , 0,
    {
        StackPush(&stk, StackPop(&stk) + StackPop(&stk));
    })

DEF_CMD(SUB, 3, 0,
    {
        int a = StackPop(&stk)_ b = StackPop(&stk);
        StackPush(&stk, a - b);
    })

DEF_CMD(MUL, 4, 0,
    {
        int a = StackPop(&stk)_ b = StackPop(&stk);
        StackPush(&stk, a * b);
    })

DEF_CMD(DIV, 5, 0,
    {
        int a = StackPop(&stk)_ b = StackPop(&stk);
        StackPush(&stk, a / b);
    })

DEF_CMD(OUT, 6, 0,
    {
        printf("YOUR VALUE = %d\n", StackPop(&stk));
    })

DEF_CMD(HLT, 7, 0,
    {
        return 0;
    })

DEF_CMD(IN, 8, 0,
    {
        int val;
        scanf("%d", &val);
        StackPush(&stk, val);
    })

DEF_CMD(POP, 9, 1,
    {
        *GetAdr(&cpu, cmd) = StackPop(&stk);
    })

DEF_CMD(JMP, 10, 2,
    {
        cpu.ip = SetArg(&cpu, cmd) - 1;
    })

DEF_CMD(JB, 11, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 < val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })

DEF_CMD(JBE, 12, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 <= val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })

DEF_CMD(JA, 13, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 > val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })

DEF_CMD(JAE, 14, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 >= val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })

DEF_CMD(JE, 15, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 == val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })

DEF_CMD(JNE, 16, 2,
    {
        val1 = StackPop(&stk);
        val2 = StackPop(&stk);
        if (val1 != val2)
            cpu.ip = SetArg(&cpu,cmd) - 1;
        else
            cpu.ip += sizeof(int);
    })
