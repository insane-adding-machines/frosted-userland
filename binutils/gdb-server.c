/*
 * Frosted gdb server
 *
 * Originally derived from STLINK gdb server by Peter Zotov
 * Copyright (c)  2011 Peter Zotov <whitequark@whitequark.org>
 * Copyright (c)  2016 Daniele Lacamera
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include "gdb-remote.h"
#include "gdb-server.h"

static int pid = -1;
static int client = -1;
static volatile int Stopped = 1;
static uint16_t listen_port = 4444;
static const char hex[] = "0123456789abcdef";
static uint8_t mbuf[600];

void usage(char *arg0)
{
    fprintf(stderr, "Usage:\n\r%s -p pid : attach to running process\r\n%s command [args...]: run command from debugger\r\n", arg0, arg0);
    exit(1);
}


void TrapHandler(int signo)
{
    int status;
    int ret;
    ret = waitpid(pid, &status, WNOHANG);
    Stopped = 1;
    if ((ret < 0) || (ret == pid && signo == SIGCHLD)) {
        fprintf(stderr, "Process terminated. Exiting...\r\n");
        if (client > -1)
            close(client);
        exit(0);
    }
}


int serve(void);
int main(int argc, char *argv[])
{
    int ret;
    uint32_t text_base;
    uint32_t text_size;
    struct sigaction sigtrap = {};
    struct sigaction sigchld = {};

    sigtrap.sa_handler = TrapHandler;
    sigchld.sa_handler = TrapHandler;
    sigaction(SIGTRAP, &sigtrap, NULL);
    sigaction(SIGCHLD, &sigtrap, NULL);


    if ((argc == 3) && (strcmp(argv[1], "-p") == 0)) {
        pid = atoi(argv[2]);
        if (pid < 2)
            usage(argv[0]);
        ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
        if (ret < 0) {
            fprintf(stderr, "Cannot vfork(): %s\r\n", strerror(errno));
            exit(1);
        }
    } else {
        struct stat st;
        char **arg_aux = malloc(argc * sizeof(char*)); /* argc -1 + 1 (NULL) at the end */
        memcpy(arg_aux, argv + 1, argc - 1);
        arg_aux[argc - 1] = NULL;
        if (stat(argv[2], &st) < 0) {
            fprintf(stderr, "Cannot execute %s: %s\r\n", argv[2], strerror(errno));
            exit(1);
        }
        pid = vfork();
        if (pid < 0) {
            fprintf(stderr, "Cannot vfork(): %s\r\n", strerror(errno));
            exit(1);
        }
        if (pid == 0) { 
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
            execvp(argv[2], arg_aux);
            exit(1);
        }
    }
    text_base = ptrace(PTRACE_PEEKUSER, pid, (void *)(19 * 4), NULL);
    text_size = ptrace(PTRACE_PEEKUSER, pid, (void *)(16 * 4), NULL);

    printf("Process %d is now stopped, .text: %08x size: %u PC:%08x LR:%08x\n", pid, text_base, text_size, ptrace(PTRACE_PEEKUSER, pid, (void*)(15 * 4), NULL), ptrace(PTRACE_PEEKUSER, pid, (void*)(14 * 4), NULL));
    serve();
    printf("gdb: Session terminated.\r\n");
    return 0;
}

static const char* const target_description =
    "<?xml version=\"1.0\"?>"
    "<!DOCTYPE target SYSTEM \"gdb-target.dtd\">"
    "<target version=\"1.0\">"
    "   <architecture>arm</architecture>"
    "   <feature name=\"org.gnu.gdb.arm.m-profile\">"
    "       <reg name=\"r0\" bitsize=\"32\"/>"
    "       <reg name=\"r1\" bitsize=\"32\"/>"
    "       <reg name=\"r2\" bitsize=\"32\"/>"
    "       <reg name=\"r3\" bitsize=\"32\"/>"
    "       <reg name=\"r4\" bitsize=\"32\"/>"
    "       <reg name=\"r5\" bitsize=\"32\"/>"
    "       <reg name=\"r6\" bitsize=\"32\"/>"
    "       <reg name=\"r7\" bitsize=\"32\"/>"
    "       <reg name=\"r8\" bitsize=\"32\"/>"
    "       <reg name=\"r9\" bitsize=\"32\"/>"
    "       <reg name=\"r10\" bitsize=\"32\"/>"
    "       <reg name=\"r11\" bitsize=\"32\"/>"
    "       <reg name=\"r12\" bitsize=\"32\"/>"
    "       <reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>"
    "       <reg name=\"lr\" bitsize=\"32\"/>"
    "       <reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>"
    "       <reg name=\"xpsr\" bitsize=\"32\"/>"
    "   </feature>"
    "</target>";

#define DATA_WATCH_NUM 4

enum watchfun { WATCHDISABLED = 0, WATCHREAD = 5, WATCHWRITE = 6, WATCHACCESS = 7 };

struct code_hw_watchpoint {
    uint32_t addr;
    uint8_t mask;
    enum watchfun fun;
};

static struct code_hw_watchpoint data_watches[DATA_WATCH_NUM];

static void init_data_watchpoints(void) {
    uint32_t data;
    printf("init watchpoints\n");

    //stlink_read_debug32(sl, 0xE000EDFC, &data);
    data |= 1<<24;
    // TODO: set trcena in debug command to turn on dwt unit
    // stlink_write_debug32(sl, 0xE000EDFC, data);

    // make sure all watchpoints are cleared
    for(int i = 0; i < DATA_WATCH_NUM; i++) {
        data_watches[i].fun = WATCHDISABLED;
        // TODO: Clear watchpoint
        //stlink_write_debug32(sl, 0xe0001028 + i * 16, 0);
    }
}

static int add_data_watchpoint(enum watchfun wf,
                               uint32_t addr, unsigned int len) {
    int i = 0;
    uint32_t mask, dummy;

    // computer mask
    // find a free watchpoint
    // configure

    mask = -1;
    i = len;
    while(i) {
        i >>= 1;
        mask++;
    }

    if((mask != (uint32_t)-1) && (mask < 16)) {
        for(i = 0; i < DATA_WATCH_NUM; i++) {
            // is this an empty slot ?
            if(data_watches[i].fun == WATCHDISABLED) {
                printf("insert watchpoint %d addr %x wf %u mask %u len %d\n", i, addr, wf, mask, len);

                data_watches[i].fun = wf;
                data_watches[i].addr = addr;
                data_watches[i].mask = mask;

                // TODO insert comparator address
                // stlink_write_debug32(sl, 0xE0001020 + i * 16, addr);

                // TODO insert mask
                // stlink_write_debug32(sl, 0xE0001024 + i * 16, mask);

                // TODO insert function
                // stlink_write_debug32(sl, 0xE0001028 + i * 16, wf);

                // TODO just to make sure the matched bit is clear !
                // stlink_read_debug32(sl,  0xE0001028 + i * 16, &dummy);
                return 0;
            }
        }
    }

    printf("failure: add watchpoints addr %x wf %u len %u\n", addr, wf, len);
    return -1;
}

static int delete_data_watchpoint(uint32_t addr)
{
    int i;

    for(i = 0 ; i < DATA_WATCH_NUM; i++) {
        if((data_watches[i].addr == addr) && (data_watches[i].fun != WATCHDISABLED)) {
            printf("delete watchpoint %d addr %x\n", i, addr);

            data_watches[i].fun = WATCHDISABLED;
            // TODO delete wp
            //stlink_write_debug32(sl, 0xe0001028 + i * 16, 0);
            return 0;
        }
    }
    printf("failure: delete watchpoint addr %x\n", addr);
    return -1;
}

static int code_break_num;
static int code_lit_num;
#define CODE_BREAK_NUM_MAX	15
#define CODE_BREAK_LOW	0x01
#define CODE_BREAK_HIGH	0x02
#define USER_BKPT(x) ((void *)(56 + 4 * x))

struct code_hw_breakpoint {
    uint32_t addr;
    int          type;
};

static struct code_hw_breakpoint code_breaks[CODE_BREAK_NUM_MAX];

static void init_code_breakpoints(void) {
    unsigned int val;
    printf("Support for 8 hw breakpoint registers\n");
    for(int i = 0; i < 8; i++) {
        code_breaks[i].type = 0;
        ptrace(PTRACE_POKEUSER, pid, USER_BKPT(i), NULL);
    }
}

static int has_breakpoint(uint32_t addr)
{
    for(int i = 0; i < code_break_num; i++) {
        if (code_breaks[i].addr == addr) {
            return 1;
        }
    }
    return 0;
}

static int update_code_breakpoint(uint32_t addr, int set) {
    uint32_t fpb_addr;
    uint32_t mask;

    if(addr & 1) {
        printf("update_code_breakpoint: unaligned address %08x\n", addr);
        return -1;
    }
    int id = -1;
    for(int i = 0; i < code_break_num; i++) {
        if(fpb_addr == code_breaks[i].addr ||
                (set && code_breaks[i].type == 0)) {
            id = i;
            break;
        }
    }

    if(id == -1) {
        if(set) return -1; // Free slot not found
        else	return 0;  // Breakpoint is already removed
    }

    struct code_hw_breakpoint* brk = &code_breaks[id];

    brk->addr = addr;


    if(set == 0) {
        printf("clearing hw break %d\n", id);
        ptrace(PTRACE_POKEUSER, pid, USER_BKPT(id), NULL);
    } else {
        printf("setting hw break %d at %08x (%d)\n",
                    id, brk->addr, brk->type);
        ptrace(PTRACE_POKEUSER, pid, USER_BKPT(id), (void *)brk->addr);
    }
    return 0;
}

int serve(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        return 1;
    }

    unsigned int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));

    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        return 1;
    }

    if(listen(sock, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("Listening at *:%d...\n", listen_port);

    client = accept(sock, NULL, NULL);
    if(client < 0) {
        perror("accept");
        return 1;
    }

    close(sock);

    init_code_breakpoints();
    init_data_watchpoints();

    printf("GDB connected.\n");

    /*
     * To allow resetting the chip from GDB it is required to
     * emulate attaching and detaching to target.
     */
    unsigned int attached = 1;

    while(1) {
        char* packet;

        int status = gdb_recv_packet(client, &packet);
        if(status < 0) {
            printf("cannot recv: %d\n", status);
            exit(1);
        }

        printf("recv: %s\n", packet);

        char* reply = NULL;

        switch(packet[0]) {
            case 'q': {
                if(packet[1] == 'P' || packet[1] == 'L') {
                    reply = strdup("");
                    break;
                }

                char *separator = strstr(packet, ":"), *params = "";
                if(separator == NULL) {
                    separator = packet + strlen(packet);
                } else {
                    params = separator + 1;
                }

                unsigned queryNameLength = (unsigned) (separator - &packet[1]);
                char* queryName = calloc(queryNameLength + 1, 1);
                strncpy(queryName, &packet[1], queryNameLength);

                printf("query: %s;%s\n", queryName, params);

                if(!strcmp(queryName, "Supported")) {
                    reply = strdup("PacketSize=04B0;qXfer:features:read+;multiprocess+");
                } else if(!strcmp(queryName, "TStatus")) {
                    reply = strdup("T0");
                } else if(!strcmp(queryName, "fThreadInfo")) {
                    reply = calloc(10,1);
                    snprintf(reply,10,"m %d", pid);
                } else if(!strcmp(queryName, "sThreadInfo")) {
                    reply = strdup("l");
                } else if(!strcmp(queryName, "TfV")) {
                    reply = strdup("l");
                } else if(!strcmp(queryName, "TfP")) {
                    reply = strdup("l");
                } else if(!strcmp(queryName, "Attached")) {
                    reply = strdup("1");
                } else if(!strcmp(queryName, "sThreadInfo")) {
                    reply = strdup("l");
                } else if(!strcmp(queryName, "Symbol")) {
                    reply = strdup("OK");
                } else if(!strcmp(queryName, "Offsets")) {
                    uint32_t text = ptrace(PTRACE_PEEKUSER, pid, (void *)(19 * 4), NULL);
                    uint32_t data = 0;
                    reply = calloc(30,1);
                    snprintf(reply,30,"TextSeg=%x",text);
                } else if(!strcmp(queryName, "Xfer")) {
                    char *type, *op, *__s_addr, *s_length;
                    char *tok = params;
                    char *annex __attribute__((unused));

                    type     = strsep(&tok, ":");
                    op       = strsep(&tok, ":");
                    annex    = strsep(&tok, ":");
                    __s_addr   = strsep(&tok, ",");
                    s_length = tok;

                    unsigned addr = (unsigned) strtoul(__s_addr, NULL, 16),
                             length = (unsigned) strtoul(s_length, NULL, 16);

                    printf("Xfer: type:%s;op:%s;annex:%s;addr:%d;length:%d\n",
                                type, op, annex, addr, length);

                    const char* data = NULL;

                    if(!strcmp(type, "features") && !strcmp(op, "read"))
                        data = target_description;

                    if(data) {
                        unsigned data_length = (unsigned) strlen(data);
                        if(addr + length > data_length)
                            length = data_length - addr;

                        if(length == 0) {
                            reply = strdup("l");
                        } else {
                            reply = calloc(length + 2, 1);
                            if (!reply) {
                                printf("OOM!\r\n");
                                exit(1);
                            }
                            reply[0] = 'm';
                            strncpy(&reply[1], data, length);
                        }
                    }
                }

                if(reply == NULL)
                    reply = strdup("");

                free(queryName);

                break;
            } /* end of 'q..' packets */

            case 'v': {
                char *params = NULL;
                char *cmdName = strtok_r(packet, ":;", &params);

                cmdName++; // vCommand -> Command

                if(!strcmp(cmdName, "FlashErase")) {
                    reply = strdup("E00");
                } else if(!strcmp(cmdName, "FlashWrite")) {
                    reply = strdup("E00");
                } else if(!strcmp(cmdName, "Kill")) {
                    kill(pid, SIGKILL);
                    reply = strdup("OK");
                } else if(!strcmp(cmdName, "Cont")) {
                    /* Process continues. */
                    Stopped = 0;
                    ptrace(PTRACE_CONT, pid, NULL, NULL);
                    while(!Stopped) {
                        status = gdb_check_for_interrupt(client);

                        if(status < 0) {
                            printf("cannot check for int: %d\n", status);
                            return 1;
                        }
                        if(status == 1) {
                            /* Ctrl+C from client */
                            Stopped = 1;
                            kill(pid, SIGSTOP);
                        }
                        sleep(1);
                    }
                    reply = strdup("S05"); // TRAP
                }
                if(reply == NULL)
                    reply = strdup("");

                break;
            }
            case 'H':
                reply = strdup("OK");
                break;

            case 'c':
                /* Process continues. */
                Stopped = 0;
                ptrace(PTRACE_CONT, pid, NULL, NULL);
                while(!Stopped) {
                    status = gdb_check_for_interrupt(client);

                    if(status < 0) {
                        printf("cannot check for int: %d\n", status);
                        return 1;
                    }
                    if(status == 1) {
                        /* Ctrl+C from client */
                        Stopped = 1;
                        kill(pid, SIGSTOP);
                    }
                    sleep(1);
                }
                reply = strdup("S05"); // TRAP
                break;

            case 's':
                ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
                reply = strdup("S05"); // TRAP
                break;

            case '?':
                reply = strdup("S05"); // TRAP
                break;

            case 'g': {
                    struct user u;
                    ptrace(PTRACE_GETREGS, pid, NULL, &u);

                    reply = calloc(8 * 16 + 1, 1);
                    for(int i = 0; i < 16; i++)
                        sprintf(&reply[i * 8], "%08x", htonl(u.regs[i]));
                    break;
                }

            case 'p': {
                unsigned id = (unsigned) strtoul(&packet[1], NULL, 16);
                unsigned myreg = 0xDEADDEAD;
                unsigned reg_val;

                if(id < 16) {
                    reg_val = ptrace(PTRACE_PEEKUSER, pid, (void *)(id * 4), NULL);
                    myreg = htonl(reg_val);
                }
                reply = calloc(8 + 1, 1);
                sprintf(reply, "%08x", myreg);
                break;
            }

            case 'P': {
                char* s_reg = &packet[1];
                char* s_value = strstr(&packet[1], "=") + 1;
                unsigned reg   = (unsigned) strtoul(s_reg,   NULL, 16);
                unsigned value = (unsigned) strtoul(s_value, NULL, 16);

                if ((reg < 16) && (ptrace(PTRACE_POKEUSER, pid, (void *)(reg * 4), (void *)value) == 0)) 
                    reply = strdup("OK");
                else
                    reply = strdup("E00");
                break;
            }

            case 'G':
                for(int i = 0; i < 13; i++) {
                    char str[9] = {0};
                    strncpy(str, &packet[1 + i * 8], 8);
                    uint32_t reg = (uint32_t) strtoul(str, NULL, 16);
                    ptrace(PTRACE_POKEUSER, pid, (void *)(i * 4), (void *)reg);
                }
                reply = strdup("OK");
                break;

            case 'm': {
                int i;
                char* s_start = &packet[1];
                char* s_count = strstr(&packet[1], ",") + 1;
                uint32_t start = (uint32_t) strtoul(s_start, NULL, 16);
                unsigned     count = (unsigned) strtoul(s_count, NULL, 16);
                unsigned adj_start = start % 4;
                unsigned count_rnd = (count + adj_start + 4 - 1) / 4 * 4;
                if (count_rnd > 0x258)
                    count_rnd = 0x258;
                if (count_rnd < count)
                    count = count_rnd;

                for (i = 0; i < count_rnd / 4; i+=4) {
                    uint32_t res = ptrace(PTRACE_PEEKDATA, pid, (void *)(start - adj_start), NULL);
                    memcpy(mbuf + i, &res, sizeof(uint32_t));
                }
                reply = calloc(count * 2 + 1, 1);
                for(unsigned int i = 0; i < count; i++) {
                    reply[i * 2 + 0] = hex[mbuf[i + adj_start] >> 4];
                    reply[i * 2 + 1] = hex[mbuf[i + adj_start] & 0xf];
                }
                break;
            }

            case 'M': {
                int i;
                char* s_start = &packet[1];
                char* s_count = strstr(&packet[1], ",") + 1;
                char* hexdata = strstr(packet, ":") + 1;

                uint32_t start = (uint32_t) strtoul(s_start, NULL, 16);
                unsigned     count = (unsigned) strtoul(s_count, NULL, 16);
                int align = start % 4;
                int mpos = 0;
                for(i = 0; i < count; i ++) {
                    char hextmp[3] = { hexdata[i*2], hexdata[i*2+1], 0 };
                    uint8_t byte = strtoul(hextmp, NULL, 16);
                    mbuf[i] = byte;
                }

                if(align != 0) {
                    uint32_t res = ptrace(PTRACE_PEEKDATA, pid, (void *)(start / 4 * 4), NULL);
                    if (align == 1) {
                        res &= 0xFF00FFFF;
                        res |= mbuf[mpos++] << 16;
                        align = 2; 
                    }
                    if (align == 2) {
                        res &= 0xFFFF00FF;
                        res |= mbuf[mpos++] << 8;
                        align = 3;
                    }
                    if (align == 3) {
                        res &= 0xFFFFFF00;
                        res |= mbuf[mpos++];
                    }
                    ptrace(PTRACE_POKEDATA, pid, (void *)(start - align), (void *)res);
                    start = start + (4 - align);
                }
                while(mpos + 3 < count) {
                    uint32_t val  = mbuf[mpos++] << 24;
                    val |= mbuf[mpos++] << 16;
                    val |= mbuf[mpos++] << 8;
                    val |= mbuf[mpos++];
                    ptrace(PTRACE_POKEDATA, pid, (void *)start, (void *)val);
                    start+=4;
                }
                if (mpos < count) {
                    uint32_t val = ptrace(PTRACE_PEEKDATA, pid, (void *)(start / 4 * 4), 0);
                    val &= 0x00FFFFFF;
                    val |= mbuf[mpos++] << 24;
                    if (mpos < count) {
                        val &= 0xFF00FFFF;
                        val |= mbuf[mpos++] << 16;
                    }
                    if (mpos < count) {
                        val &= 0xFFFF00FF;
                        val |= mbuf[mpos++] << 8;
                    }
                    ptrace(PTRACE_POKEDATA, pid, (void *)start, (void *)val);
                }
                reply = strdup("OK");
                break;
            }

            case 'Z': {
                char *endptr;
                uint32_t addr = (uint32_t) strtoul(&packet[3], &endptr, 16);
                uint32_t len  = (uint32_t) strtoul(&endptr[1], NULL, 16);

                switch (packet[1]) {
                    case '1':
                        if(update_code_breakpoint(addr, 1) < 0) {
                            reply = strdup("E00");
                        } else {
                            reply = strdup("OK");
                        }
                        break;

                    case '2':   // insert write watchpoint
                    case '3':   // insert read  watchpoint
                    case '4': { // insert access watchpoint
                        enum watchfun wf;
                        if(packet[1] == '2') {
                            wf = WATCHWRITE;
                        } else if(packet[1] == '3') {
                            wf = WATCHREAD;
                        } else {
                            wf = WATCHACCESS;
                        }

                        if(add_data_watchpoint(wf, addr, len) < 0) {
                            reply = strdup("E00");
                        } else {
                            reply = strdup("OK");
                            break;
                        }
                    }

                    default:
                        reply = strdup("");
                }
                break;
            }
            case 'z': {
                char *endptr;
                uint32_t addr = (uint32_t) strtoul(&packet[3], &endptr, 16);
                //uint32_t len  = strtoul(&endptr[1], NULL, 16);

                switch (packet[1]) {
                    case '1': // remove breakpoint
                        update_code_breakpoint(addr, 0);
                        reply = strdup("OK");
                        break;

                    case '2' : // remove write watchpoint
                    case '3' : // remove read watchpoint
                    case '4' : // remove access watchpoint
                        if(delete_data_watchpoint(addr) < 0) {
                            reply = strdup("E00");
                        } else {
                            reply = strdup("OK");
                            break;
                        }

                    default:
                        reply = strdup("");
                }
                break;
            }

            case '!': {
                reply = strdup("OK");
                break;
            }
            default:
                reply = strdup("");
        }

        if(reply) {

            int result = gdb_send_packet(client, reply);
            printf("send: %s\n", reply);
            if(result != 0) {
                printf("cannot send: %d\n", result);
                free(reply);
                free(packet);
                return 1;
            }

            free(reply);
        }

        free(packet);
    }
    return 0;
}
