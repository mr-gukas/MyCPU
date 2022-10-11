#include <stdlib.h>

int main(void)
{
    system("obj/asm obj/cmd.txt obj/bin.txt");
    system("obj/cpu obj/bin.txt");

    return 0;
}

