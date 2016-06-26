#include <sys/ptrace.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#ifndef htonl
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#endif



struct gdbpkt {
    char cmd;
    int auto_err;
    int (*hdlr)(int pid, int dd);
};

static int gdb_continue(int pid, int dd)
{
    return 0;
}

static int gdb_detach(int pid, int dd)
{
    return 0;
}

static int gdb_getregs(int pid, int dd)
{
    return 0;
}

static int gdb_setregs(int pid, int dd)
{
    return 0;
}

static int gdb_kill(int pid, int dd)
{
    return 0;
}

static int gdb_memread(int pid, int dd)
{
    return 0;
}

static int gdb_memwrite(int pid, int dd)
{
    return 0;
}

static int gdb_getreg(int pid, int dd)
{
    return 0;
}

static int gdb_setreg(int pid, int dd)
{
    return 0;
}

static int gdb_multi(int pid, int dd)
{
    return 0;
}

static int gdb_addbkp(int pid, int dd)
{
    return 0;
}

static int gdb_delbkp(int pid, int dd)
{
    return 0;
}


struct gdbpkt pkt_parse_tab[] = {
    { 'c', 0, gdb_continue },
    { 'D', 1, gdb_detach },
    { 'g', 0, gdb_getregs },
    { 'G', 1, gdb_setregs },
    { 'k', 1, gdb_kill },
    { 'm', 0, gdb_memread },
    { 'M', 1, gdb_memwrite },
    { 'p', 0, gdb_getreg },
    { 'P', 1, gdb_setreg },
    { 'v', 0, gdb_multi },
    { 'z', 1, gdb_addbkp },
    { 'Z', 1, gdb_delbkp },
    { '\0', 0, NULL }
};



void gdbserver(pid_t pid, int dd)
{
    int ret, cmdr;
    char cmd;
    struct gdbpkt *p;
    
    ret = ptrace(PTRACE_ATTACH, pid, 0, 0);
    if (ret < 0) {
        perror("ptrace");
    }

    do {
        ret = read(dd, &cmd, 1);
        if (ret == 1) {
            int i = 0;
            while (pkt_parse_tab[i].hdlr != NULL) {
                if (cmd == pkt_parse_tab[i].cmd) {
                    cmdr = pkt_parse_tab[i].hdlr(pid, dd);

                    /* Handle automatic error if flag present */
                    if (pkt_parse_tab[i].auto_err) {
                        if (cmdr == 0) {
                            write(dd, "OK\n", 3);
                        } else {
                            char reply_err[8];
                            snprintf(reply_err, 7, "E %d", 0 - cmdr);
                        }
                    }
                    break;
                }
                i++;
            }
        }
    } while (ret > 0);
}


int main(int argc, char *argv[])
{
    int ret;
    pid_t pid;
    int dd, ld;
    struct sockaddr_in gdbsock = { .sin_family = AF_INET, .sin_addr = INADDR_ANY };
    struct sockaddr_in client = { };
    socklen_t socklen = sizeof(struct sockaddr_in);
    
    if (argc < 3) {
        printf ("Usage: %s <pid> [<tcp_port|serial_device>]\r\n", argv[0]);
        printf ("                   (Default: tcp port 3333) \r\n");
        exit(1);
    }

    pid = atoi(argv[1]);

    if (strstr(argv[2], "tty") != NULL) {
        dd = open(argv[2], O_RDWR);
        if (dd < 0) {
            fprintf(stderr, "Serial port error: %s\n", strerror(errno));
            exit(1);
        }
        gdbserver(pid, dd);
    } else {
        if (argc > 2)
            gdbsock.sin_port = htons(atoi(argv[2]));
        else
            gdbsock.sin_port = htons(3333);

        ld = socket(AF_INET, SOCK_STREAM, 0);
        if (ld < 0) {
            fprintf(stderr, "Socket error: %s\n", strerror(errno));
            exit(1);
        }

        if ((listen(ld, 1) < 0) || (bind(ld, (struct sockaddr *)&gdbsock, sizeof(struct sockaddr_in) < 0)) ) {
            fprintf(stderr, "Socket error: %s\n", strerror(errno));
            exit(1);
        }

        while(1) {
            printf("GDB - Waiting for connection...\r\n");
            dd = accept(ld, (struct sockaddr *)&client, &socklen);
            if (dd >= 0) {
                printf("GDB - Connection accepted!\r\n");
                gdbserver(pid, dd);
            } else {
                printf("GDB - Connection failed: %s\r\n", strerror(errno));
            }
            close(dd);
        }
    }
    exit(0);
    
}
