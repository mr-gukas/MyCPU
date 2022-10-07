#include "filework/filework.h"
#include "stack/stack.h"
#include "log/LOG.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

const size_t STR_MAX_SIZE = 20;
const short  BASIC_SIGN   = 'VG';
const size_t BASIC_VERS   = 1;

struct CmdInfo_t
{
    short  sign;
    size_t vers;
    size_t size;
    size_t nInt;
};

struct AsmCmd_t
{
    CmdInfo_t info    ;
    TEXT      commands;
    int*      asmArr  ; 
};

enum Commands_t
{
    CMD_HLT,
    CMD_PUSH,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_OUT,
    CMD_INP,
    CMD_DUMP,
};

#ifdef LOG_MODE
    FILE* PrintFile = startLog(PrintFile);
#else
    FILE* PrintFile = stdout;
#endif

int asmCtor(AsmCmd_t* asmCmd, FILE* source);
int asmMakeArr(AsmCmd_t* asmCmd, FILE* processed);
size_t cmdValue(char* command);
int asmDtor(AsmCmd_t* asmCmd);
int FillBin(AsmCmd_t* asmCmd, FILE* binary);
int* ReadBin(int* cmdArr, size_t* nMemb, FILE* binary);
int RunCalc(int* cmdArr, size_t* nMemb);
