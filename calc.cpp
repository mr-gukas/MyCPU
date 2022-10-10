#include "cpu.h"

int main(int argc, const char* argv[])
{   

    if (argc != 2)
    {
        fprintf(stderr, ">>>Incorrect command line arguments. Exiting the program....\n");
    }   

    FILE* data = NULL;
    
    if ((data = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, ">>>Incorrect filename: %s\n", argv[1]);
        abort();
    }
    
    Cpu_t cpu = {};
    CpuCtor(&cpu, data);

    RunCalc(&cpu);

    CpuDtor(&cpu, data);

#ifdef LOG_MODE
    endLog(PrintFile);
#endif 

    return 0;
}

int CpuCtor(Cpu_t* cpu, FILE* binary)
{
    ASSERT(cpu != NULL);
    
    StackCtor(&cpu->stk, 0);
    
    CmdInfo_t binInfo = {};

    fread(&binInfo, sizeof (CmdInfo_t), 1, binary);
    
    if (binInfo.sign != BASIC_SIGN || binInfo.vers != BASIC_VERS)
    {
        fprintf(stderr, ">>>The file extension is incorrect. Exiting the program...\n");
        abort();
    }
    
    cpu->cmdArr = (char*) calloc (1, binInfo.size + sizeof(arg_t) * binInfo.nArgs + 1);

    if (cpu->cmdArr == NULL)
        return 1;

    fread(cpu->cmdArr, sizeof(char), binInfo.size + sizeof(arg_t) * binInfo.nArgs, binary);

    return 0;

}

int CpuDtor(Cpu_t* cpu, FILE* data)
{
    ASSERT(cpu  != NULL);
    ASSERT(data != NULL);

    StackDtor(&cpu->stk);
    free     (cpu->cmdArr);
    fclose   (data);

    data = nullptr;

    return 0;
}

int RunCalc(Cpu_t* cpu)
{   
    ASSERT(cpu != NULL);

    size_t ip = 0;
    
    arg_t num1 = 0;
    arg_t num2 = 0;

#define CALC_(stk, sign)       \
    num2 = StackPop(stk);       \
    num1 = StackPop(stk);        \
    StackPush(stk, num1 sign num2);
    
    while (*(cpu->cmdArr + ip))
    {
        int curCmd = *(cpu->cmdArr + ip);

        switch (curCmd & ONLY_CMD)
        {
            case CMD_PUSH:
            {   
                ++ip;
                int arg = GetPushArg(curCmd, &ip, cpu);
                StackPush(&cpu->stk, arg);
                //StackDump(&cpu->stk);
                break;
            }
            
            case CMD_POP:
            {   
                ++ip;
                arg_t* arg = GetPopArg(curCmd, &ip, cpu);
                *arg = StackPop(&cpu->stk);
                break;
            }

            case CMD_JMP:
            {
                ++ip;
                GetJumpArg(&ip, cpu);
                break;
            }

            case CMD_ADD:
                CALC_(&cpu->stk, +);
                break;
            case CMD_SUB:
                CALC_(&cpu->stk, -);
                break;
            case CMD_MUL:
                CALC_(&cpu->stk, *);
                break;
            case CMD_DIV:
                CALC_(&cpu->stk, /);
                break;
            case CMD_OUT:
                printf("%d\n", StackPop(&cpu->stk));
                //StackDump(&cpu->stk);
                break;
            case CMD_INP:
            {   
                arg_t num = 0;
                if (scanf("%d", &num) != 1)
                {
                    fprintf(stderr, ">>>Incorrect input value...Sorry...");
                    abort();
                }

                StackPush(&cpu->stk, num);
                break;
            }
            case CMD_HLT:
                break;
            default:
                fprintf(stderr, ">>>An unknown operation was encountered: %d\n", *(cpu->cmdArr + ip));
                abort();
        }

        ++ip;
    }

    return 0;
#undef CALC_
} 

arg_t GetPushArg(int command, size_t* ip, Cpu_t* cpu)
{
    ASSERT(ip   != NULL);
    ASSERT(cpu  != NULL);

    arg_t  arg    = 0;
    arg_t  value  = 0;
    int    curReg = 0; 
    short  ipCtrl = 0;

    if (command & ARG_IMMED)
    {   
        memcpy(&value, cpu->cmdArr + *ip, sizeof(arg_t));
        arg += value;
        
        ++ipCtrl;
    }

    if (command & ARG_REG)
    {
        if (command & ARG_IMMED)
        {
            memcpy(&curReg, cpu->cmdArr + *ip + sizeof(arg_t), sizeof(int));
            arg += cpu->regs[curReg];
            
            ++ipCtrl;
        }
        else 
        {
            memcpy(&curReg, cpu->cmdArr + *ip, sizeof(int));
            arg += cpu->regs[curReg];

            ++ipCtrl;
        }
    }

    if (command & ARG_MEM)
    {
        if (arg > MAX_RAM_SIZE)
        {
            SYNTAX_ERROR(command);
        }
        else
            arg = cpu->RAM[arg];
    }

    if ((command & (ARG_IMMED | ARG_REG | ARG_MEM)) == 0)
    {
        SYNTAX_ERROR(command);
    }

    *ip += ipCtrl * sizeof(int) - 1;

    return arg;
}

arg_t* GetPopArg(int command, size_t* ip, Cpu_t* cpu)
{
    ASSERT(ip   != NULL);
    ASSERT(cpu  != NULL);

    size_t arg    = 0; 
    arg_t  value  = 0;
    int    curReg = 0; 

    
    if (command & ARG_MEM)
    {
        if (command & ARG_IMMED)
        {   
            memcpy(&value, cpu->cmdArr + *ip, sizeof(arg_t));
            *ip += sizeof(arg_t);
            
            arg += (size_t) value;
        }

        if (command & ARG_REG)
        {
            memcpy(&curReg, cpu->cmdArr + *ip, sizeof(int));
            *ip += sizeof(int);

            arg += cpu->regs[curReg];
        }

        if (arg >= MAX_RAM_SIZE)
        {
            SYNTAX_ERROR(command);
        }
        else
        {
            sleep(1);
            *ip -= 1;

            return &cpu->RAM[arg];
        }
    }
    
    else if (command & ARG_REG) 
    {
        memcpy(&curReg, cpu->cmdArr + *ip, sizeof(int));
        *ip += sizeof(int) - 1;

        return cpu->regs + curReg;

    }

    else
    {
        SYNTAX_ERROR(command);
    }

    return NULL;
}

void GetJumpArg(size_t* ip, Cpu_t* cpu)
{
    ASSERT(ip  != NULL);
    ASSERT(cpu != NULL);
    
    size_t arg = 0;
    
    memcpy(&arg, cpu->cmdArr + *ip, sizeof(int));
    
    if (arg < 0)
    {
        fprintf(stderr, "Bad jumping on %d...\n", arg);
        abort();
    }

    *ip = arg - 1;
}















