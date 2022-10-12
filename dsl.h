#define POP          StackPop (&cpu->stk)
#define PUSH(arg)    StackPush(&cpu->stk, (arg))
#define VAR          arg_t
#define GET_PUSH_ARG GetPushArg(curCmd, &ip, cpu) 
#define GET_POP_ARG  GetPopArg(curCmd, &ip, cpu)
#define GET_JMP_ARG  GetJumpArg(&ip, cpu)
#define DUMP_CPU     CpuDump(cpu, ip);
#define INDEX_UP     ++ip
#define BAD_DIV                                          \
    {                                                     \
        fprintf(stderr, "CANNOT BE DIVIDED BY ZERO!!!\n"); \
        abort();                                            \
    }

#define BAD_INP                                   \
    {                                              \
        fprintf(stderr, "Incorrect input value\n"); \
        abort();                                     \
    }


