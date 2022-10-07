#include "cpu.h"

//FILE* PrintFile = startLog(PrintFile); 

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
    
    size_t nMemb  = 0;
    int*   cmdArr = ReadBin(cmdArr, &nMemb, data);
   
    RunCalc(cmdArr, &nMemb);

    free(cmdArr);
    fclose(data);

#ifdef LOG_MODE
    endLog(PrintFile);
#endif 

    return 0;
}

int* ReadBin(int* cmdArr, size_t* nMemb, FILE* binary)
{
    CmdInfo_t binInfo = {};
    fread(&binInfo, sizeof (CmdInfo_t), 1, binary);
    
    if (binInfo.sign != BASIC_SIGN || binInfo.vers != BASIC_VERS)
    {
        fprintf(stderr, ">>>The file extension is incorrect. Exiting the program...\n");
        abort();
    }
    
    cmdArr = (int*) calloc(binInfo.nInt + 1, sizeof(int));
    fread(cmdArr, sizeof(int), binInfo.nInt, binary);
    *nMemb = binInfo.nInt;

    return cmdArr;
}

int RunCalc(int* cmdArr, size_t* nMemb)
{   
    Stack_t stk = {};
    StackCtor(&stk, 0);
    
    size_t index = 0;
    
    int num1 = 0;
    int num2 = 0;

#define CALC_(stk, sign)       \
    num2 = StackPop(stk);       \
    num1 = StackPop(stk);        \
    StackPush(stk, num1 sign num2); 
    
    while (index <= *nMemb)
    {
        switch (cmdArr[index])
        {
            case CMD_PUSH:
                StackPush(&stk, cmdArr[++index]);
                break;
            case CMD_ADD:
                CALC_(&stk, +);
                break;
            case CMD_SUB:
                CALC_(&stk, -);
                break;
            case CMD_MUL:
                CALC_(&stk, *);
                break;
            case CMD_DIV:
                CALC_(&stk, /);
                break;
            case CMD_OUT:
                printf("%d\n", StackPop(&stk));
                break;
            case CMD_INP:
            {
                int num = 0;
                if (scanf("%d", &num) != 1)
                    fprintf(stderr, ">>>Incorrect input value...Sorry...");
                
                StackPush(&stk, num);
                break;
            }
            case CMD_HLT:
                break;
            default:
                fprintf(stderr, ">>>An unknown operation was encountered: %d\n", cmdArr[index]);
        }

        ++index;
    }

    StackDtor(&stk);

    return 0;

#undef CALC_
} 
 
 
    


















