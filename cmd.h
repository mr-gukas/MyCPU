#include "dsl.h"

DEF_CMD(HLT,  0, 0, 
{
    ;
})

DEF_CMD(PUSH, 1, 1, 
{
    INDEX_UP;
    VAR ARG = GET_PUSH_ARG; 

    PUSH(ARG);
})

DEF_CMD(ADD,  2, 0, 
{
    VAR NUM2 = POP;
    VAR NUM1 = POP;

    PUSH(NUM1 + NUM2);
})

DEF_CMD(SUB,  3, 0,
{
    VAR NUM2 = POP;
    VAR NUM1 = POP;
    PUSH(NUM1 - NUM2);
})

DEF_CMD(MUL,  4, 0,
{
    VAR NUM2 = POP;
    VAR NUM1 = POP;

    PUSH(NUM2 * NUM1);
})

DEF_CMD(DIV,  5, 0,
{
    VAR NUM2 = POP;
    VAR NUM1 = POP;
    
    if (NUM2 == 0)
        BAD_DIV;
    
    PUSH((NUM1 / NUM2));
})

DEF_CMD(OUT,  6, 0,
{
    printf("%d\n", POP);
})

DEF_CMD(INP,  7, 0,
{
    VAR NUM  = 0;

    if (scanf("%d", &NUM) != 1)
        BAD_INP;

    PUSH(NUM);
})

DEF_CMD(DUMP, 8, 0,
{
    CpuDump(cpu, ip);
})

DEF_CMD(POP, 9, 1,
{
    INDEX_UP;
    VAR* ARG = GET_POP_ARG;
    *ARG = POP;
})

//DEF_CMD(JMP, 10, 1,
//{
//    INDEX_UP;
//    GET_JMP_ARG;
//})

DEF_JMP(JMP, 10, || 1 ||)

DEF_JMP(JA,  11,       >)

DEF_JMP(JAE, 12,      >=)

DEF_JMP(JE,  13,      ==)

DEF_JMP(JBE, 14,       <)

DEF_JMP(JB,  15,      <=)

DEF_JMP(JNE, 16,      !=)
