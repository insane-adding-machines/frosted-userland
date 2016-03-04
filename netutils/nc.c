#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#ifndef htonl
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#endif

#ifndef STDIN_FILENO
# define STDIN_FILENO 0
# define STDOUT_FILENO 1
# define STDERR_FILENO 2
#endif


#define NC_CONNECT 0
#define NC_LISTEN 1


struct netcat_conf
{
    int mode;
    uint16_t socktype;
    uint16_t port;
    uint16_t lport;
    char *host;
};


/* TODO: move to newlib */

static inline int is_digit(char c)
{
    if (c < '0' || c > '9')
        return 0;
    return 1;
}

static inline uint32_t long_from(void *_p)
{
    unsigned char *p = (unsigned char *)_p;
    uint32_t r, p0, p1, p2, p3;
    p0 = p[0];
    p1 = p[1];
    p2 = p[2];
    p3 = p[3];
    r = (p0 << 24) + (p1 << 16) + (p2 << 8) + p3;
    return r;
}


int __inet_aton(const char *ipstr, struct in_addr *ia)
{
    unsigned char buf[22] = {
        0
    };
    int cnt = 0;
    char p;
    uint32_t *ip = (uint32_t *)ia;

    while((p = *ipstr++) != 0 && cnt < 22)
    {
        if (is_digit(p)) {
            buf[cnt] = (uint8_t)((10 * buf[cnt]) + (p - '0'));
        } else if (p == '.') {
            cnt++;
        } else {
            return -1;
        }
    }
    /* Handle short notation */
    if (cnt == 1) {
        buf[3] = buf[1];
        buf[1] = 0;
        buf[2] = 0;
    } else if (cnt == 2) {
        buf[3] = buf[2];
        buf[2] = 0;
    } else if (cnt != 3) {
        /* String could not be parsed, return error */
        return -1;
    }

    *ip = htonl(long_from(buf));
    return 0;
}


static int parse_conf(struct netcat_conf *conf, int argc, char *argv[])
{
    int i;
    memset(conf, 0, sizeof(struct netcat_conf));
    conf->socktype = SOCK_STREAM;

    /* First, check for listen flag */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            conf->mode = NC_LISTEN;
    }
    for (i = 1; i < argc; i++) {
        if (conf->mode) {
            if (strcmp(argv[i],"-p") == 0) {
                if (i < argc - 1) {
                    conf->lport = atoi(argv[++i]);
                    continue;
                } else {
                    fprintf(stderr, "error: -p requires a port number\r\n");
                    return -1;
                }
            }
        } else if(strcmp(argv[i],"-p") == 0) {
            fprintf(stderr, "error: -p requires -l\r\n");
            return -1;
        } else if (strcmp(argv[i], "-u") == 0)
            conf->socktype = SOCK_DGRAM;
        else if (!conf->mode) {
            if (!conf->host)
                conf->host = argv[i];
            else if (!conf->port) 
                conf->port = atoi(argv[i]);
            else
                fprintf(stderr, "Invalid argument '%s'\r\n", argv[i]);
        } else if (strcmp(argv[i], "-l") == 0) {
            /* Accepted, no effect, already parsed in previous step */
        } else {
            fprintf(stderr, "Invalid argument '%s'\r\n", argv[i]);
            return -1;
        }
    }
    if (!conf->mode) {
        if (!conf->port) {
            fprintf(stderr, "No port to connect to\r\n");
            return -1;
        }
        if (!conf->host) {
            fprintf(stderr, "No host to connect to\r\n");
            return -1;
        }
    }
    if (conf->mode)
        fprintf(stderr, "netcat running in listen mode, proto: %s listening port: %d\r\n", 
            conf->socktype == SOCK_DGRAM?"UDP":"TCP", 
            conf->lport);
    else 
        fprintf(stderr, "netcat running in connect mode, proto: %s host: %s port: %d\r\n", 
            conf->socktype == SOCK_DGRAM?"UDP":"TCP", 
            conf->host,
            conf->port);
    return 0;
}

int main(int argc, char *argv[])
{

    struct netcat_conf conf;
    int sd;
    struct sockaddr_in all = {};
    struct sockaddr_in tgt = {};
    size_t socksize = sizeof(struct sockaddr_in);
    struct pollfd pfd[2];
    int off = 0;

    if (parse_conf(&conf, argc, argv) < 0) {
        return 1;
    }

    sd = socket(AF_INET, conf.socktype, 0);
    if (sd < 0) 
    {
        fprintf(stderr, "Cannot open socket!\n");
        exit(1);
    }

    if (conf.mode) {
        all.sin_family = AF_INET;
        all.sin_port = htons(conf.lport);

        if (bind(sd, (struct sockaddr *)&all, sizeof(struct sockaddr_in)) < 0) {
            perror("bind");
            exit(3);
        }
    
        if (conf.socktype == SOCK_STREAM) {
            int asd;
            if (listen(sd, 3) < 0) {
                perror("listen");
                exit(2);
            }
            asd = accept(sd, (struct sockaddr *)&tgt, &socksize);
            if (asd < 0) {
                perror("accept");
                exit(3);
            }
            close(sd);
            sd = asd;
            fprintf(stderr, "Accepted connection.\r\n");
        }
    } else {
        tgt.sin_family = AF_INET;
        tgt.sin_port = htons(conf.port);
        inet_aton(conf.host, &tgt.sin_addr); /* TODO: getaddrinfo */
        if (connect(sd, (struct sockaddr *)&tgt, sizeof(struct sockaddr_in)) < 0) {
            perror("connect");
            exit(3);
        }
        fprintf(stderr, "Connected to server.\r\n");
    }

    while(1) {
        int pollret;
        int r;
        char buf_in[100];
        char buf_sock[100];
        pfd[0].fd = STDIN_FILENO;
        pfd[1].fd = sd;
        pfd[0].events = POLLIN;
        pfd[1].events = POLLIN | POLLHUP | POLLERR;
        pollret = poll(pfd, 2, -1);
        if (pollret < 0) { 
            fprintf(stderr, "nc: poll returned -1\r\n");
            break;
        }
        if ((pfd[1].revents & (POLLHUP | POLLERR)) != 0) {
            fprintf(stderr, "nc: remote peer has closed connection.\r\n");
            break;
        }
        if (pfd[1].revents & POLLIN) {
            r = read(sd, buf_sock, 100);
            if (r > 0) {
                if (buf_sock[r - 1] == '\n') {
                    r--;
                    write(STDOUT_FILENO, buf_sock, r);
                    printf("\r\n");
                } else {
                    write(STDOUT_FILENO, buf_sock, r);
                }
            }
        }
        if (pfd[0].revents & POLLIN) {
            r = read(pfd[0].fd, buf_in + off, 100 - off);
            if (r > 0) {
                write(STDOUT_FILENO, buf_in + off, r);
                off += r;
                if ((off == 100) || (buf_in[off - 1] == '\n') || (buf_in[off - 1] == '\r')) {
                    if (buf_in[off - 1] == '\r') {
                        buf_in[off - 1] = '\n';
                        printf("\n");
                    }
                    write(sd, buf_in, off);
                    off = 0;
                }
            }
        }
    }
    fprintf(stderr, "nc: interrupted\r\n");
    return 1;
}


