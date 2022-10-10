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

    asmMakeArr(&sourceCmd);

    asmMakeArr(&sourceCmd);

    FillBin(&sourceCmd, processed);
   
    asmDtor(&sourceCmd);

    fclose(source);
    fclose(processed);

}

int asmCtor(AsmCmd_t* asmCmd, FILE* source)
{
    ASSERT(asmCmd != NULL);
    ASSERT(source != NULL);
    
     
    textCtor(&(asmCmd->commands), source);

    asmCmd->listfile   = fopen(LISTING_FILE, "w+");
    asmCmd->info.sign  = BASIC_SIGN;
    asmCmd->info.vers  = BASIC_VERS;
    asmCmd->info.size  = asmCmd->commands.nLines - 1;
    asmCmd->info.nArgs = 0;
    asmCmd->asmArr     = (char*) calloc(asmCmd->info.size * 2, sizeof(arg_t));
    
    for (size_t index = 0; index < MAX_LABEL_COUNT; ++index)
    {
        asmCmd->labels[index].adress = POISON_ARG;
        asmCmd->labels[index].name   = POISON_NAME;
    }

    ASSERT(asmCmd->asmArr != NULL);

    return 0;
}

int asmDtor(AsmCmd_t* asmCmd)
{
    ASSERT(asmCmd != NULL);
    
    fclose(asmCmd->listfile);
    textDtor(&asmCmd->commands);
    free(asmCmd->asmArr);

    return 0;
}

int asmMakeArr(AsmCmd_t* asmCmd)
{
    ASSERT(asmCmd != NULL);

    size_t index = 0;
    int    ip    = 0;
    
    fprintf(asmCmd->listfile, "\n---------------------COMMAND LIST---------------------\n");

    for (index = 0, ip = 0; index < asmCmd->info.size; ++index)
    {
        char cmd[STR_MAX_SIZE] = {};
        int  nChar             = 0 ;
        
        fprintf(asmCmd->listfile, "\n%3x %15s ",  ip, asmCmd->commands.lines[index].lineStart);

        sscanf(asmCmd->commands.lines[index].lineStart, "%s%n", cmd, &nChar);
    
        if ((strcasecmp(cmd, "PUSH") == 0))
        {   
            MakeArg(asmCmd->commands.lines[index].lineStart + nChar, CMD_PUSH, asmCmd, &ip);
        }

        else if ((strcasecmp(cmd, "POP") == 0))
        {
            MakeArg(asmCmd->commands.lines[index].lineStart + nChar, CMD_POP, asmCmd, &ip);
        }

        else if ((strcasecmp(cmd, "ADD") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_ADD;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
       
        else if ((strcasecmp(cmd, "SUB") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_SUB;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }

        else if ((strcasecmp(cmd, "MUL") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_MUL;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
       
        else if ((strcasecmp(cmd, "DIV") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_DIV;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
       
        else if ((strcasecmp(cmd, "OUT") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_OUT;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
       
        else if ((strcasecmp(cmd, "HLT") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_HLT;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
         
        else if ((strcasecmp(cmd, "INPUT") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_INP;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }
        
        else if ((strcasecmp(cmd, "DUMP") == 0))
        {   
            *(asmCmd->asmArr + ip++) = CMD_DUMP;
            fprintf(asmCmd->listfile, "%0x ",  *(asmCmd->asmArr + ip - 1));
        }

        else if ((strcasecmp(cmd, "JMP") == 0))
        {
            MakeArg(asmCmd->commands.lines[index].lineStart + nChar, CMD_JMP, asmCmd, &ip);
        }
        
        else if (strchr(cmd, ':') != NULL)
        {   
            LabelAnalyze(cmd, asmCmd, ip); 
        }
    
        else
        {
            fprintf(stderr, ">>>Incorrectly entered operation:\n"
                            ">>>%lu: %s\n"
                            ">>>Emergency termination of the process...\n", index,  cmd);
            abort();
        }

 
                
    }
    
    return 0;
}

int FillBin(AsmCmd_t* asmCmd, FILE* binary)
{
    ASSERT(asmCmd != NULL);
    ASSERT(binary != NULL);

    fwrite(&asmCmd->info, sizeof asmCmd->info, 1, binary);
    fwrite(asmCmd->asmArr, sizeof (char), asmCmd->info.size + asmCmd->info.nArgs * sizeof (arg_t), binary);

    return 0;
}

void CopyInt(char* arr, int* value)
{   
    ASSERT(arr   != NULL);
    ASSERT(value != NULL);

    memcpy(arr, value, sizeof (int));  
}


int IsRegister(char* reg)
{
    ASSERT(reg != NULL);

    if (strlen(reg) == 3 && reg[0] == 'R' && reg[2] == 'X' && reg[1] >= 'A' && reg[1] <= 'D')
        return reg[1] - 'A' + 1;

    else
    {
        fprintf(stderr, "Syntax error. Incorrect register: %s\n", reg);
        abort();
    }
    
    return -1;
}

void MakeArg(char* line, int command, AsmCmd_t* asmCmd, int* ip)
{
    ASSERT(line   != NULL);
    ASSERT(asmCmd != NULL);
    ASSERT(ip     != NULL);
    
    int argCtrl = 0; 

    if (command == CMD_JMP)
    {
        argCtrl = MakeJumpArg(line, asmCmd, ip);
    }
    
    else
    {
        
        switch (isInBrackets(line))
        {
            case 0:
                argCtrl = MakeCommonArg(line, command, asmCmd, ip);
                break;
            case 1:
                argCtrl = MakeBracketsArg(line, command, asmCmd, ip);
                break;
            case -1:
            default:
                argCtrl = 1;
        }
    }

    if (argCtrl)
    {
        fprintf(stderr, ">>>Argument Error: %s\n", line);
        abort();
    }
}

int isInBrackets(char* arg)
{
    ASSERT(arg != NULL);
    
    char* open  = strchr(arg, '[');
    char* close = strchr(arg, ']');

    if (open == NULL && close == NULL)
        return 0;
    
    if ((open && !close) || (!open && close))
        return -1;
    
    return 1;
}

int MakeCommonArg(char* line, int command, AsmCmd_t* asmCmd, int* ip)
{
    ASSERT(line   != NULL);
    ASSERT(asmCmd != NULL);
    ASSERT(ip     != NULL);

    arg_t  curValue            = 0;
    char curReg[STR_MAX_SIZE]  = {};
    int  intReg                = 0;
    
    if (strchr(line, '+') == NULL)
    {
        if (command != CMD_POP && sscanf(line, "%d", &curValue) == 1)
        {
            *(asmCmd->asmArr + *ip) = CMD_PUSH | ARG_IMMED; 
            CopyInt(asmCmd->asmArr + *ip + 1, &curValue);

            fprintf(asmCmd->listfile, "%0x %0x ",  *(asmCmd->asmArr + *ip),  curValue);
        }
        
        else if (sscanf(line, "%s", curReg) == 1  && (intReg = IsRegister(curReg)) != -1)
        {
            *(asmCmd->asmArr + *ip) = command | ARG_REG;
            CopyInt(asmCmd->asmArr + *ip + 1, &intReg);

            fprintf(asmCmd->listfile, "%0x %0x",  *(asmCmd->asmArr + *ip),  intReg);
        }
        
        else
            return 1;
        
        *ip += 1 + sizeof(arg_t);
        (asmCmd->info.nArgs) += 1;
    }
    
    else
    {
        if (command != CMD_POP && sscanf(line, "%d+%s", &curValue, curReg) == 2 && (intReg = IsRegister(curReg)) != -1)
        {
            *(asmCmd->asmArr + *ip) = CMD_PUSH | ARG_IMMED | ARG_REG;  
            
            CopyInt(asmCmd->asmArr + *ip + 1, &curValue);
            CopyInt(asmCmd->asmArr + *ip + 1 + sizeof(int), &intReg);

            fprintf(asmCmd->listfile, "%0x %0x %0x ",  *(asmCmd->asmArr + *ip),  curValue, intReg);
        }

        else
            return 1;

        *ip += 2 * sizeof(arg_t) + 1;
        (asmCmd->info.nArgs) += 2;
    }

    return 0;
}

int MakeBracketsArg(char* line, int command, AsmCmd_t* asmCmd, int* ip)
{
    ASSERT(line   != NULL);
    ASSERT(asmCmd != NULL);
    ASSERT(ip     != NULL);
    

    char* arg = strchr(line, '[') + 1;
    
    arg_t  curValue            = 0;
    char curReg[STR_MAX_SIZE]  = {};
    int  intReg                = 0;
    
    if (strchr(line, '+') == NULL)
    {
        if (sscanf(arg, "%d", &curValue) == 1)
        {
            *(asmCmd->asmArr + *ip) = command | ARG_IMMED | ARG_MEM; 
            CopyInt(asmCmd->asmArr + *ip + 1, &curValue);

            fprintf(asmCmd->listfile, "%0x %0x ",  *(asmCmd->asmArr + *ip),  curValue);
        }
        
        else if (sscanf(arg, "%s", curReg) == 1)
        {
            curReg[strlen(curReg) - 1] = '\0';

            if ((intReg = IsRegister(curReg)) != -1)
            {
                *(asmCmd->asmArr + *ip) = command | ARG_REG | ARG_MEM;
                CopyInt(asmCmd->asmArr + *ip + 1, &intReg);

                fprintf(asmCmd->listfile, "%0x %0x ",  *(asmCmd->asmArr + *ip),  intReg);
            }
        
            else
                return 1;
        }

        else
            return 1;

        *ip += 1 + sizeof(arg_t);
        (asmCmd->info.nArgs) += 1;
    }
    
    else
    {
        if (sscanf(arg, "%d+%s", &curValue, curReg) == 2)
        {
            curReg[strlen(curReg) - 1] = '\0';

            if ((intReg = IsRegister(curReg)) != -1)
            {
                *(asmCmd->asmArr + *ip) = command | ARG_IMMED | ARG_REG | ARG_MEM;  
            
                CopyInt(asmCmd->asmArr + *ip + 1, &curValue);
                CopyInt(asmCmd->asmArr + *ip + 1 + sizeof(int), &intReg);

                fprintf(asmCmd->listfile, "%0x %0x %0x ",  *(asmCmd->asmArr + *ip),  curValue,  intReg);
            }

            else
                return 1;
        }

        else
            return 1;

        *ip += 2 * sizeof(arg_t) + 1;
        (asmCmd->info.nArgs) += 2;
    }

    return 0;
}

void LabelAnalyze(char* cmd, AsmCmd_t* asmCmd, int ip)
{
    ASSERT(cmd    != NULL);
    ASSERT(asmCmd != NULL);

    int  curLabel                 = -1;
    char curTextLabel[LABEL_SIZE] = {};
    int  labelLen                 = 0;

    if (sscanf(cmd, "%d:", &curLabel) == 1)
    {
        if (curLabel >= 0 && curLabel < MAX_LABEL_COUNT &&  asmCmd->labels[curLabel].adress == POISON_ARG)
        {
            asmCmd->labels[curLabel].name   = cmd;
            asmCmd->labels[curLabel].adress = ip;
        }

        else if (curLabel >= 0 && curLabel < MAX_LABEL_COUNT && asmCmd->labels[curLabel].adress != POISON_ARG) ;

        else
        {
            fprintf(stderr, ">>>Label error: %s\n", cmd);
            abort();
        }
    }
    else if (sscanf(cmd, "%s%n", curTextLabel, &labelLen) == 1)
    {
        curTextLabel[labelLen - 1] = '\0';
        
        int labelCtrl = 0;
        for (size_t num = 0; num < MAX_LABEL_COUNT; ++num)
        {
            if (strcmp(asmCmd->labels[num].name, POISON_NAME) == 0)
            {
                asmCmd->labels[num].name   = curTextLabel;

                asmCmd->labels[num].adress = ip;
                curLabel                   = num;    

                labelCtrl = 1;
                break;
            }

            else if (asmCmd->labels[num].adress != POISON_ARG)
            {
                labelCtrl = 1;
                break;
            }

        }
        
        if (labelCtrl == 0)
        {
            fprintf(stderr, "LABEL OVERFLOW!\n");
            abort();
        }
    }
    
    else 
    {
        fprintf(stderr, "Bad label parody - %s - ...\n", cmd);
        abort();
    }

}

int MakeJumpArg(char* line, AsmCmd_t* asmCmd, int *ip)
{
    ASSERT(line   != NULL);
    ASSERT(asmCmd != NULL);
    ASSERT(ip     != NULL);
    
    *(asmCmd->asmArr + *ip) = CMD_JMP;

    int  curValue                 = 0;
    char curTextLabel[LABEL_SIZE] = {};

    if (strchr(line, ':') == NULL)
    {   
        if (sscanf(line, "%d", &curValue) == 1)
        {
            CopyInt(asmCmd->asmArr + *ip + 1, &curValue);
        }

        else
            return 1;
    }
    
    else
    {   
        if (sscanf(line, " :%d", &curValue) == 1 && curValue >= 0 && curValue <= MAX_LABEL_COUNT)
        {   
            if (asmCmd->labels[curValue].adress  == POISON_ARG)
            {   
                curValue = POISON_ARG;

                CopyInt(asmCmd->asmArr + *ip + 1, &curValue);
            }
            
            else
            {
                CopyInt(asmCmd->asmArr + *ip + 1, &asmCmd->labels[curValue].adress);
            }
        }
        
        else if (sscanf(line, " :%s", curTextLabel) == 1)
        {
            for (size_t num = 0; num < MAX_LABEL_COUNT; ++num)
            {
                if (strcmp(asmCmd->labels[num].name, curTextLabel) == 0)
                {
                    CopyInt(asmCmd->asmArr + *ip + 1, &asmCmd->labels[num].adress);
                    curValue = num;

                    break;
                }
            }
            
        }

        else
            return 1;

    }

    fprintf(asmCmd->listfile, "%0x %0x ",  *(asmCmd->asmArr + *ip),  asmCmd->labels[curValue].adress);


    *ip += sizeof(arg_t) +  1;
    asmCmd->info.nArgs   += 1;

    return 0;
}





