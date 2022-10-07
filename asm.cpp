#include "cpu.h"

int main(int argc, const char* argv[])
{
    FILE* source    = NULL;
    FILE* processed = NULL;

    if (argc != 3)
        fprintf(stderr, ">>>Incorrect command line arguments. Exiting from programm....\n");

    if ((source = fopen(argv[1], "r")) == NULL)
        fprintf(stderr, ">>>Problems with file with source commands: %s\n", argv[1]);

    processed = fopen(argv[2], "w+");
    
    AsmCmd_t sourceCmd = {};

    asmCtor(&sourceCmd, source);

    asmMakeArr(&sourceCmd, processed);

    asmDtor(&sourceCmd);

    fclose(source);
    fclose(processed);

}

int asmCtor(AsmCmd_t* asmCmd, FILE* source)
{
    ASSERT(asmCmd != NULL);
    ASSERT(source != NULL);

    textCtor(&(asmCmd->commands), source);
    
    asmCmd->info.sign = BASIC_SIGN;
    asmCmd->info.vers = BASIC_VERS;
    asmCmd->info.size = asmCmd->info.nInt = asmCmd->commands.nLines - 1;
    asmCmd->asmArr    = (int*) calloc(asmCmd->info.size * 2, sizeof(int));

    ASSERT(asmCmd->asmArr != NULL);

    return 0;
}

int asmDtor(AsmCmd_t* asmCmd)
{
    ASSERT(asmCmd != NULL);

    textDtor(&asmCmd->commands);
    free(asmCmd->asmArr);

    return 0;
}

int asmMakeArr(AsmCmd_t* asmCmd, FILE* processed)
{
    ASSERT(asmMakeArr != NULL);
    
    size_t index      = 0;
    size_t arrIndex   = 0;

    for (index = 0, arrIndex = 0; index < asmCmd->info.size; ++index)
    {
        char cmd[STR_MAX_SIZE] = {};
        int  nChar             = 0 ;

        sscanf(asmCmd->commands.lines[index].lineStart, "%s%n", cmd, &nChar);
        
        if ((strcasecmp(cmd, "push") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_PUSH;

            if (sscanf(asmCmd->commands.lines[index].lineStart + nChar, "%d", &(asmCmd->asmArr[arrIndex++])) == 0)
            {
                fprintf(stderr, ">>>Incorrectly entered operation:\n"
                                ">>>%s\n"
                                ">>>Emergency termination of the process...\n", cmd);
                abort();
            }
            
            ++(asmCmd->info.nInt);
        }
             
        else if ((strcasecmp(cmd, "add") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_ADD;
        }
       
        else if ((strcasecmp(cmd, "sub") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_SUB;
        }

        else if ((strcasecmp(cmd, "mul") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_MUL;
        }
       
        else if ((strcasecmp(cmd, "div") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_DIV;
        }
       
        else if ((strcasecmp(cmd, "out") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_OUT;
        }
       
        else if ((strcasecmp(cmd, "hlt") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_HLT;
        }
         
        else if ((strcasecmp(cmd, "input") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_INP;
        }
        
        else if ((strcasecmp(cmd, "dump") == 0))
        {   
            asmCmd->asmArr[arrIndex++] = CMD_DUMP;
        }
        
        else
        {
            fprintf(stderr, ">>>Incorrectly entered operation:\n"
                            ">>>%lu: %s\n"
                            ">>>Emergency termination of the process...\n", index,  cmd);
            abort();
        }

    }
    
    FillBin(asmCmd, processed);

    return 0;
}

int FillBin(AsmCmd_t* asmCmd, FILE* binary)
{
    fwrite(&asmCmd->info, sizeof asmCmd->info, 1, binary);
    fwrite(asmCmd->asmArr, sizeof (int), asmCmd->info.nInt, binary);

    return 0;
}

/*size_t cmdValue(char* command)
{
    size_t index = 0;
    int    value = POISON_VALUE;
    while (*(command + index))
    {
        if (isdigit(*(command + index)))
            break;
        ++index;
    }

    ASSERT(*(command + index) != POISON_VALUE);

    return index;
}*/























