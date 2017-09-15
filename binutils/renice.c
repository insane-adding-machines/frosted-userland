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

#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void usage(char *name)
{
    printf("Usage: %s priority pid\r\n", name);
    exit(1);

}

#ifndef APP_RENICE_MODULE
int main(int argc, char *args[])
#else
int icebox_renice(int argc, char *args[])
#endif
{
    int pid;
    int prio;
    if ((argc != 3)) {
        usage(args[0]);
    }
        prio = atoi(args[1]);
        pid = atoi(args[2]);
    if (pid < 1) {
        usage(args[0]);
    }
    if ((prio > 20) || (prio < (-20))) {
        fprintf(stderr, "Invalid priority '%d'\r\n", prio);
        usage(args[0]);
    }


    if (setpriority(PRIO_PROCESS, pid, prio) < 0) {
        fprintf(stderr,"setpriority: %s\r\n", strerror(errno));
        exit(2);
    }
    exit(0);
}
