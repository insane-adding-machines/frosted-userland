#include "frosted_binutils.h"


int main(int argc, char *args)
{
    char *file = args[1];

    if (unlink(file) < 0) {
        printf("File not found.\r\n");
        exit(-1);
    }
    exit(0);
}
