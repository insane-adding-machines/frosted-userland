
#include "frosted_binutils.h"

int main(int argc, char *args[])
{
    int pid;
    if (argc != 2) {
        printf("Usage: %s pid\r\n", args[0]);
        exit(1);
    }
    pid = atoi(args[1]);
    if (pid < 1) {
        printf("Usage: %s pid\r\n", args[0]);
        exit(2);
    }
    if (pid == 1) {
        printf("Error: Can't kill init!\r\n");
        exit(3);
    }

    kill(pid, SIGTERM);
    exit(0);
}
