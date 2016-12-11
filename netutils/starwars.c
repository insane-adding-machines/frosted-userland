/*
 *      This file is part of frosted.
 *
 *      frosted is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License version 2, as
 *      published by the Free Software Foundation.
 *
 *
 *      frosted is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with frosted.  If not, see <http://www.gnu.org/licenses/>.
 *
 *      Authors: Daniele Lacamera
 *
 */

#include <errno.h>
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

#define BUFSIZE (16 * 1024)

int main(int argc, char *argv[])
{

    int sd;
    struct sockaddr_in all = {};
    struct sockaddr_in tgt = {};
    size_t socksize = sizeof(struct sockaddr_in);
    struct pollfd pfd[2];
    int off = 0;
    int fbcon = -1;
    int r;

    char *buf_sock = malloc(BUFSIZE);

    fbcon = open("/dev/fbcon", O_RDWR);
    if (fbcon < 0) {
        fprintf(stderr, "Cannot open framebuffer console!\n");
        exit(1);
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        fprintf(stderr, "Cannot open socket!\n");
        exit(1);
    }
    tgt.sin_family = AF_INET;
    tgt.sin_port = htons(23);
    inet_aton("94.142.241.111", &tgt.sin_addr); /* TODO: getaddrinfo */
    if (connect(sd, (struct sockaddr *)&tgt, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        exit(3);
    }
    while(1) {
        r = read(sd, buf_sock, BUFSIZE);
        if (r > 0) {
            write(fbcon, buf_sock, r);
        }
    }
    return 1;
}
