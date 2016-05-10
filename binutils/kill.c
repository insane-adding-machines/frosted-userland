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

#include "frosted_binutils.h"
#include <signal.h>

void usage(char *name)
{
    printf("Usage: %s [-signo] pid\r\n", name);
    exit(1);

}

int main(int argc, char *args[])
{
    int pid;
    int signo = SIGTERM;
    if ((argc != 2) && (argc != 3)) {
        usage(args[0]);
    }
    if (argc == 3) {
        if (args[1][0] != '-')
            usage(args[0]);
        signo = atoi(args[1] + 1);
        pid = atoi(args[2]);
    } else {
        pid = atoi(args[1]);
    }
    if (pid < 1) {
        usage(args[0]);
    }
    if (pid == 1) {
        printf("Error: Can't kill init!\r\n");
        exit(3);
    }

    kill(pid, signo);
    exit(0);
}
