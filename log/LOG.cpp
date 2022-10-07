#include "LOG.h"

FILE* startLog(FILE* LogFile)
{
    LogFile = fopen(PRINT_FILE, "w");
    setvbuf (LogFile, NULL, _IONBF, 0);

    fprintf(LogFile, "---------------------------Started logging---------------------------------\n");
    printTime(LogFile);

    return LogFile;
}

void printTime(FILE* file)
{
    time_t t = time(NULL);

    struct tm tm = *localtime(&t);
    
    fprintf(file, "DATE: %d-%02d-%02d\n"
                  "TIME: %02d:%02d:%02d\n",
                 tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void endLog(FILE* LogFile)
{
    fprintf(LogFile, "END OF LOG!\n");
    fclose (LogFile);
    LogFile = nullptr;
}
