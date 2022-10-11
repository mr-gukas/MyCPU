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

    RunCpu(&cpu);

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
    
    cpu->size   =  binInfo.size + sizeof(arg_t) * binInfo.nArgs + 1;
    cpu->cmdArr = (char*) calloc (1, cpu->size);
    
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

int RunCpu(Cpu_t* cpu)
{   
    ASSERT(cpu != NULL);

    size_t ip = 0;
    
    while (*(cpu->cmdArr + ip))
    {
        int curCmd = *(cpu->cmdArr + ip);

#define DEF_CMD(name, num, arg, code) \
    case CMD_##name:                   \
        code                            \
        break;

#define DEF_JMP(name, num, sign)         \
    case JMP_##name:                      \
    {                                      \
        INDEX_UP;                           \
        if ((curCmd & ONLY_CMD) == JMP_JMP)  \
        {                                     \
            GET_JMP_ARG;                       \
        }                                       \
        else                                     \
        {                                         \
            VAR NUM2 = POP;                        \
            VAR NUM1 = POP;                         \
                                                     \
            if ((NUM1) sign (NUM2))                   \
            {                                          \
                GET_JMP_ARG;                            \
            }                                            \
            else                                          \
                ip += sizeof(int) - 1;                     \
        }                                                   \
        break;                                               \
    }

        switch (curCmd & ONLY_CMD)
        {
            #include "cmd.h"

            default:
                fprintf(stderr, ">>>An unknown operation was encountered: %d\n", *(cpu->cmdArr + ip));
                abort();
        }

        ++ip;
    }

#undef DEF_CMD
#undef DEF_JMP

    return 0;
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
        
        *ip += sizeof(arg_t);
    }

    if (command & ARG_REG)
    {
       memcpy(&curReg, cpu->cmdArr + *ip, sizeof(int));
       arg += cpu->regs[curReg];
       
       *ip += sizeof(int);
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
        printf("hehe...\n");
        SYNTAX_ERROR(command);
    }

    *ip -= 1;

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

void CpuDump(Cpu_t* cpu, size_t ip)
{
    printf("-----------------------------------CPU DUMP-----------------------------------\n");

    
    for (size_t index = 0; index < cpu->size; ++index)
    {   
        if (index == ip)
        {
            printf("[");
        }
        
        printf("(%02X, %02X) ", index, *(cpu->cmdArr + index));

        if (index == ip)
        {
            printf("]");
        }


        if (*(cpu->cmdArr + index) == 6)
            break;

        if (index != 0 && index % 9 == 0)
            printf("\n");
    }

    printf("\n");

    StackDump(&cpu->stk);

    printf("                            REGISTERS:\n");
    printf("REG:  VALUE:\n");

    for (size_t index = 1; index < REGS_COUNT; ++index)
    {
        printf("R%cX %6d\n", 'A' + index - 1, cpu->regs[index]);
    }

    printf("                            RAM:\n");

    for (size_t index = 0; index < MAX_RAM_SIZE; ++index)
    {
        printf("%3d ", cpu->RAM[index]);

        if ((index + 1) % 20 == 0)
            printf("\n");
    }

    printf("--------------------------------------------------------------------------------\n");
}












