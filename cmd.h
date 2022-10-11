#define _ ,
#define POP StackPop(&cpu->stk)
#define PUSH(x) StackPush(&cpu->stk, x)
#define CPUDBG CpuDump(*cpu, __LINE__, __func__, __FILE__);

#define JUMP(x)                                     \
        val1 = POP;                                 \
        val2 = POP;                                 \
        if (val1 x val2)                            \
            cpu->ip = SetArg(cpu,cmd) - 1;          \
        else                                        \
            cpu->ip += sizeof(int);                 \
        CPUDBG


DEF_CMD(PUSH, 1, 1,
    {
        PUSH(SetArg(cpu, cmd));
        CPUDBG
    })

DEF_CMD(ADD, 2 , 0,
    {
        PUSH(POP + POP);
        CPUDBG
    })

DEF_CMD(SUB, 3, 0,
    {
        int a = POP;
        int b = POP;
        PUSH(a - b);
        CPUDBG
    })

DEF_CMD(MUL, 4, 0,
    {
        PUSH(POP * POP);
        CPUDBG
    })

DEF_CMD(DIV, 5, 0,
    {
        int a = POP;
        int b = POP;
        PUSH(a / b);
        CPUDBG
    })

DEF_CMD(OUT, 6, 0,
    {
        printf("YOUR VALUE = %d\n", POP);
        CPUDBG
    })

DEF_CMD(HLT, 7, 0,
    {
        return 0;
        CPUDBG
    })

DEF_CMD(IN, 8, 0,
    {
        int val;
        scanf("%d", &val);
        PUSH(val);
        CPUDBG
    })

DEF_CMD(POP, 9, 1,
    {
        *GetAdr(cpu, cmd) = POP;
        CPUDBG
    })

DEF_CMD(JMP, 10, 2,
    {
        cpu->ip = SetArg(cpu, cmd) - 1;
        CPUDBG
    })

DEF_CMD(JB, 11, 2,
    {
        JUMP(<)
    })

DEF_CMD(JBE, 12, 2,
    {
        JUMP(<=)
    })

DEF_CMD(JA, 13, 2,
    {
        JUMP(>)
    })

DEF_CMD(JAE, 14, 2,
    {
        JUMP(>=)
    })

DEF_CMD(JE, 15, 2,
    {
        JUMP(==)
    })

DEF_CMD(JNE, 16, 2,
    {
        JUMP(!=)
    })

DEF_CMD(SQRT, 17, 0,
    {
        int a = POP;
        PUSH(sqrt(a));
        CPUDBG
    })
