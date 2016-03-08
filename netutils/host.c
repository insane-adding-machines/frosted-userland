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
#include <stdio.h>
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

int main(int argc, char *argv[])
{

    struct addrinfo *res;
    int retval;
    res = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: host <hostname>");
        return 1;
    }
    retval = getaddrinfo(argv[1], NULL, NULL, &res);
    if (retval < 0) {
        fprintf(stderr, "Cannot resolve %s: %s\r\n", argv[1], strerror(errno));
        return 2;
    }
    if (res) {
        struct sockaddr_in *sai = (struct sockaddr_in *)(res->ai_addr);
        printf("%s has address %s\r\n", argv[1], inet_ntoa(sai->sin_addr));
    }

    return 0;
}
