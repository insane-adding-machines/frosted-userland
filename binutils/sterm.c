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
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#ifndef APP_STERM_MODULE
int main(int argc, char *argv[])
#else
int icebox_sterm(int argc, char *argv[])
#endif
{

    int sd;
    struct pollfd pfd[2];
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\r\n", argv[0]);
        exit(1);
    }

    sd = open(argv[1], O_RDWR);
    if (sd < 0)
    {
        fprintf(stderr, "Cannot open port %s\n", argv[1]);
        exit(1);
    }
    printf("Connected to %s\r\n", argv[1]);

    while(1) {
        int pollret;
        int r;
        char c;
        const char cr = '\r';
        pfd[0].fd = STDIN_FILENO;
        pfd[1].fd = sd;
        pfd[0].events = POLLIN;
        pfd[1].events = POLLIN;
        pollret = poll(pfd, 2, -1);
        if (pollret < 0) {
            fprintf(stderr, "sterm: poll returned -1\r\n");
            break;
        }
        if ((pfd[1].revents & (POLLHUP | POLLERR)) != 0) {
            fprintf(stderr, "sterm: Error polling serial port \r\n");
            break;
        }
        if (pfd[1].revents & POLLIN) {
            r = read(sd, &c, 1);
            if (r > 0) {
                if (c == '\n')
                    write(STDOUT_FILENO, &cr, 1);
                write(STDOUT_FILENO, &c, 1);
            }
        }
        if (pfd[0].revents & POLLIN) {
            r = read(pfd[0].fd, &c, 1);
            if (r > 0) {
                if (c == '\n')
                    write(STDOUT_FILENO, &cr, 1);
                write(sd, &c, 1);
            }
        }
    }
    fprintf(stderr, "sterm: interrupted\r\n");
    return 1;
}
