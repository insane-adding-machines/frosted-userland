
#include "frosted_binutils.h"

int main(int argc, char *args[])
{
    char *file = args[1];
    int fd;
    fd = open(file, O_CREAT|O_TRUNC|O_EXCL|O_WRONLY);
    if (fd < 0) {
        printf("Cannot create file.\r\n");
        exit(-1);
    } else close(fd);
    exit(0);
}
