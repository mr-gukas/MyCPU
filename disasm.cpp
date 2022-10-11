#include "disasm.h"

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, ">>>Incorrect command line arguments. Exiting the program....\n");
    }   

    FILE* data   = NULL;
    FILE* rezult = fopen(argv[2], "w+");
; 

    if ((data = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, ">>>Incorrect filename: %s\n", argv[1]);
        abort();
    }

    char* cmdArr = ReadBin(cmdArr, data);
    
    RunDisasm(cmdArr, rezult);

    free(cmdArr);
}
    
char* ReadBin(char* cmdArr, FILE* binary)
{
    ASSERT(binary != NULL);

    CmdInfo_t binInfo = {};

    fread(&binInfo, sizeof (CmdInfo_t), 1, binary);
    
    if (binInfo.sign != BASIC_SIGN || binInfo.vers != BASIC_VERS)
    {
        fprintf(stderr, ">>>The file extension is incorrect. Exiting the program...\n");
        abort();
    }
    
    size_t size = binInfo.size + sizeof(arg_t) * binInfo.nArgs + 1;

    cmdArr = (char*) calloc (1, size);
    
    if (cmdArr == NULL)
    {
        fprintf(stderr, ">>>Problems with allocating memory. Exiting the program...\n");
        abort();
    }

    fread(cmdArr, sizeof(char), size, binary);
    
    fclose(binary);
   
    return cmdArr;
}

void RunDisasm(char* cmdArr, FILE* rezult)
{
    ASSERT(cmdArr != NULL);
    ASSERT(rezult != NULL);

    size_t ip = 0;
    
    while (*(cmdArr + ip))
    {
        int curCmd = *(cmdArr + ip);

#define DEF_CMD(name, num, arg, code)               \
    case CMD_##name:                                \
    {                                               \
        fprintf(rezult, #name);                     \
        if (arg)                                    \
        {                                           \
            DisasmArg(curCmd, cmdArr, &ip, rezult); \
        }                                           \
        fprintf(rezult, "\n");                      \
        break;                                      \
    }

#define DEF_JMP(name, num, sign)                         \
    case JMP_##name:                                     \
    {                                                    \
        fprintf(rezult, #name);                          \
        int argIm = 0;                                   \
        memcpy(&argIm, cmdArr + ip + 1, sizeof(arg_t));  \
        fprintf(rezult, " %02X\n", argIm);               \
        ip += sizeof(int);                               \
        break;                                           \
    }

        switch (curCmd & ONLY_CMD)
        {
            #include "cmd.h"

            default:
                fprintf(stderr, ">>>An unknown operation was encountered: %02X\n", *(cmdArr + ip));
                abort();
        }

        ++ip;
    }

#undef DEF_CMD
#undef DEF_JMP
    
    fclose(rezult);
}


void DisasmArg(int curCmd, char* cmdArr, size_t* ip, FILE* rezult)
{
    ASSERT(cmdArr != NULL);
    ASSERT(ip     != NULL);

    fprintf(rezult, (curCmd & ARG_MEM) ? " [" : " ");

    if ((curCmd & ARG_IMMED) && (curCmd & ARG_REG))
    {
        int argIm = 0;
        int argRe = 0;
        
        memcpy(&argIm, cmdArr + *ip + 1, sizeof(arg_t));
        memcpy(&argRe, cmdArr + *ip + 1 + sizeof(arg_t), sizeof(int));

        fprintf(rezult, "%d+R%cX", argIm, 'A' + argRe - 1);

        *ip += sizeof(int) + sizeof(arg_t);
    }

    else if (curCmd & ARG_IMMED)
    {
        int argIm = 0;
        
        memcpy(&argIm, cmdArr + *ip + 1, sizeof(arg_t));

        fprintf(rezult, "%d", argIm);

        *ip += sizeof(arg_t);
    }

    else if (curCmd & ARG_REG)
    {
        int argRe = 0;
        
        memcpy(&argRe, cmdArr + *ip + 1, sizeof(int));

        fprintf(rezult, "R%cX", 'A' + argRe - 1);

        *ip += sizeof(int);
    }


    else
    {
        fprintf(stderr, "BAD ARGUMENT: %0X", curCmd ^ ONLY_CMD);
        abort();
    }

    fprintf(rezult, (curCmd & ARG_MEM) ? "]" : "");
}


