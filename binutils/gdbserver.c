#include <sys/ptrace.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef htonl
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#endif



int main(int argc, char *argv[])
{
    int ret;
    pid_t pid;
    
    if (argc < 3) {
        printf ("Usage: %s <pid> <tcp_port|serial_device>\r\n");
        exit(1);
    }

    pid = atoi(argv[1]);

    ret = ptrace(PTRACE_ATTACH, pid, 0, 0);

    exit(0);
    
}
