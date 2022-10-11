#include "cpu.h"

char* ReadBin(char* cmdArr, FILE* binary);
void RunDisasm(char* cmdArr, FILE* rezult);
void DisasmArg(int curCmd, char* cmdArr, size_t* ip, FILE* rezult);
