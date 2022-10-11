#include "filework/filework.h"
#include "stack/stack.h"
#include "log/LOG.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define SYNTAX_ERROR(command)                       \
    fprintf(stderr, "Syntax error: %x\n", #command); \
    abort();  

typedef int arg_t;

const size_t STR_MAX_SIZE    = 20;
const short  BASIC_SIGN      = 'VG';
const size_t BASIC_VERS      = 1;
const size_t REGS_COUNT      = 5;
const size_t MAX_RAM_SIZE    = 100;
const size_t MAX_LABEL_COUNT = 10; 
const size_t LABEL_SIZE      = 10;
const char   LISTING_FILE[]  = "obj/listing.txt";  
const size_t POISON_ARG      = 31415;
char*        POISON_NAME     = "ded32";

#define DEF_CMD(name, num, arg, code) \
    CMD_##name = num,

#define DEF_JMP(name, num, sign)      \
    JMP_##name = num,

enum Commands
{
    #include "cmd.h" 
};

#undef DEF_CMD
#undef DEF_JMP

struct Label_t
{
    int  adress;
    char*  name;
};

enum Arg_Ctrl
{
    ARG_IMMED = 1 << 5,
    ARG_REG   = 1 << 6,
    ARG_MEM   = 1 << 7,
    ONLY_CMD  = 31, 
};

enum Regs
{
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
};

struct CmdInfo_t
{
    short  sign;
    size_t vers;
    size_t size;
    size_t nArgs;
};

struct AsmCmd_t
{
    CmdInfo_t info    ;
    TEXT      commands;
    char      *asmArr ; 
    FILE*     listfile;
    Label_t   labels[MAX_LABEL_COUNT];
};

struct Cpu_t
{
    char* cmdArr;
    Stack_t stk;
    arg_t regs[REGS_COUNT];
    arg_t RAM[MAX_RAM_SIZE];
};

#ifdef LOG_MODE
    FILE* PrintFile = startLog(PrintFile);
#else
    FILE* PrintFile = stdout;
#endif

int asmCtor(AsmCmd_t* asmCmd, FILE* source);

int asmMakeArr(AsmCmd_t* asmCmd);

int asmDtor(AsmCmd_t* asmCmd);

int FillBin(AsmCmd_t* asmCmd, FILE* binary);

char* ReadBin(char* cmdArr, FILE* binary);

int RunCalc(Cpu_t* cpu);

int IsRegister(char* reg);

void CopyInt(char* arr, int* value);

void MakeArg(char* line, int command, AsmCmd_t* asmCmd, size_t *ip);

int CpuCtor(Cpu_t* cpu, FILE* binary);

int CpuDtor(Cpu_t* cpu, FILE* data);

arg_t GetPushArg(int command, size_t* ip, Cpu_t* cpu);

arg_t* GetPopArg(int command, size_t* ip, Cpu_t* cpu);

int isInBrackets(char* arg);

int MakeCommonArg(char* line, int command, AsmCmd_t* asmCmd, size_t* ip);

int MakeBracketsArg(char* line, int command, AsmCmd_t* asmCmd, size_t* ip);

int MakeJumpArg(char* line, int command, AsmCmd_t* asmCmd, size_t *ip);

void LabelAnalyze(char* cmd, AsmCmd_t* asmCmd, size_t ip);

void GetJumpArg(size_t* ip, Cpu_t* cpu);

void CpuDump(Cpu_t* cpu, size_t ip);
