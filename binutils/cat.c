
#include "frosted_binutils.h"

int main(int argc, char *args[])
{
    int fd;
    int i = 1;
    while (args[i]) {
        fd = open(args[1], O_RDONLY);
        if (fd < 0) {
            printf("File not found.\r\n");
            exit(-1);
        } else {
            int r;
            char buf[10];
            do {
                r = read(fd, buf, 10);
                if (r > 0) {
                    write(STDOUT_FILENO, buf, r);
                }
            } while (r > 0);
            close(fd);
        }
       i++;
    }
    exit(0);
}
