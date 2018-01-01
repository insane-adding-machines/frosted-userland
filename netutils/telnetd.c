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
 *      Authors: Daniele Lacamera, Maxime Vincent
 *
 */

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

char FRESH_BIN[] = "/bin/fresh";
char * const fresh_args[2] = {FRESH_BIN, NULL};

int main(int argc, char *argv[])
{
    int lsd;
    struct sockaddr_in all = {};
    lsd = socket(AF_INET, SOCK_STREAM, 0);

    if (lsd < 0)
    {
        fprintf(stderr, "Cannot open socket!\n");
        exit(1);
    }

    all.sin_family = AF_INET;
    all.sin_port = htons(23);

    if (bind(lsd, (struct sockaddr *)&all, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        exit(3);
    }

    if (listen(lsd, 3) < 0) {
        perror("listen");
        exit(2);
    }


    while(1) {
        int pid;
        int asd;
        struct sockaddr_in client = {};
        socklen_t cli_len = sizeof(struct sockaddr_in);
        asd = accept(lsd, (struct sockaddr *)&client, &cli_len);
        if (asd >= 0) {
            pid = vfork();
            if (pid == 0) {
                close(lsd);
                write(asd, "Welcome to frosted!\r\n", 21);
                if (asd != STDIN_FILENO) {
                    close(STDIN_FILENO);
                    dup(asd);
                }
                if (asd != STDOUT_FILENO) {
                    close(STDOUT_FILENO);
                    dup(asd);
                }
                if (asd != STDERR_FILENO) {
                    close(STDERR_FILENO);
                    dup(asd);
                }
                execv(FRESH_BIN, fresh_args);
            } else {
                close(asd);
            }
        } else {
            perror("accept");
            exit(1);
        }
    }
}
