#define _ ,
#define POP StackPop(&cpu->commands)
#define PUSH(x) StackPush(&cpu->commands, x)

#define JUMP(x)                                     \
        val1 = POP;                                 \
        val2 = POP;                                 \
        if (val1 x val2)                            \
            SetLabel(cpu);                          \
        else                                        \
            cpu->ip += sizeof(int);                 \
        CPUCHECK


DEF_CMD(PUSH, 1, 1,
    {
        GetArg(cpu, cmd);
        PUSH(arg);
        CPUCHECK
    })

DEF_CMD(ADD, 2 , 0,
    {
        PUSH(POP + POP);
        CPUCHECK
    })

DEF_CMD(SUB, 3, 0,
    {
        elem_t a = POP;
        elem_t b = POP;
        PUSH(a - b);
        CPUCHECK
    })

DEF_CMD(MUL, 4, 0,
    {
        PUSH(POP * POP);
        CPUCHECK
    })

DEF_CMD(DIV, 5, 0,
    {
        elem_t a = POP;
        elem_t b = POP;
        if (b == 0)
        {
            printf("Error: cannot divide by 0");
            return ARITHERR;
        }
        PUSH(a / b);
        CPUCHECK
    })

DEF_CMD(OUT, 6, 0,
    {
        printf("YOUR VALUE = %lg\n", POP);
        CPUCHECK
    })

DEF_CMD(HLT, 7, 0,
    {
        CPUCHECK
        return 0;
    })

DEF_CMD(IN, 8, 0,
    {
        elem_t val;
        scanf("%lg", &val);
        PUSH(val);
        CPUCHECK
    })

DEF_CMD(POP, 9, 1,
    {
        GetArg(cpu, cmd);
        *argptr = POP;
        CPUCHECK
    })

DEF_CMD(JMP, 10, 1,
    {
        SetLabel(cpu);
        CPUCHECK
    })

DEF_CMD(JB, 11, 1,
    {
        JUMP(<)
    })

DEF_CMD(JBE, 12, 1,
    {
        JUMP(<=)
    })

DEF_CMD(JA, 13, 1,
    {
        JUMP(>)
    })

DEF_CMD(JAE, 14, 1,
    {
        JUMP(>=)
    })

DEF_CMD(JE, 15, 1,
    {
        JUMP(==)
    })

DEF_CMD(JNE, 16, 1,
    {
        JUMP(!=)
    })

DEF_CMD(SQRT, 17, 0,
    {
        elem_t a = POP;
        if (a < 0)
        {
            printf("Error: cannot take square roots of negative numbers");
            return ARITHERR;
        }
        PUSH(sqrt(a));
        CPUCHECK
    })

DEF_CMD(CALL, 18, 1,
    {
        StackPush(&cpu->returns, (elem_t) (cpu->ip + sizeof(int)));
        SetLabel(cpu);
        CPUCHECK
    })

DEF_CMD(RET, 19, 0,
    {
        cpu->ip = (int) StackPop(&cpu->returns);
        CPUCHECK
    })

DEF_CMD(SQRD, 20, 0,
    {
        elem_t a = POP;
        PUSH(a * a);
        CPUCHECK
    })

DEF_CMD(MOD, 21, 0,
    {
        PUSH((int) POP % 10);
        CPUCHECK
    })

DEF_CMD(IDIV, 22, 0,
    {
        PUSH((int) (POP / 10));
        CPUCHECK
    })

DEF_CMD(RAMW, 23, 0,
    {
        RamWrite(cpu);
        CPUCHECK
    })

DEF_CMD(NRT, 24, 0,
    {
        printf("This equation has no roots");
        CPUCHECK
    })

DEF_CMD(INF, 25, 0,
    {
        printf("This equation has infinite number of roots");
        CPUCHECK
    })
