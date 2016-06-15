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

#include "syscalls.h"
#include "ioctl.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#define IDLE() while(1){do{}while(0);}
#define GREETING "Welcome to frosted!\n"
extern void *_init;

char idling_txt[10] = "idling";
char fresh_txt[10] = "fresh";
char serial_dev[12] = "/dev/ttyS0";

const char idling_path[30] = "/bin/idling";
const char fresh_path[30] = "/bin/fresh";


static char * const fresh_args[4] = {fresh_txt, "-t", serial_dev, NULL};
static char * const idling_args[2] = {idling_txt, NULL};


int main(void *arg)
{
    volatile int i = (int)arg;
    volatile int pid;
    int status;


    if (vfork() == 0) {
        execve(idling_path, idling_args, NULL);
        exit(1);
    }

    if (vfork() == 0) {
        execve(fresh_path, fresh_args, NULL);
        exit(1);
    }
    while(1) {
        pid = waitpid(-1, &status, 0);
    }
    return 0;
}
