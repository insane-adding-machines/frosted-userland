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
#include "frosted_api.h"
#include "fresh.h"
#include "syscalls.h"
#include "ioctl.h"
#include "sys/socket.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#define IDLE() while(1){do{}while(0);}
#define GREETING "Welcome to frosted!\n"
extern void *_init;


static const char fresh_args[2][10] = {"fresh", NULL};
static const char idling_args[2][10] = {"idling", NULL};
static const char binutils_args[2][10] = {"binutils", NULL};


int main(void *arg)
{
    volatile int i = (int)arg;
    volatile int pid;
    int status;
    int fd, sd;
    uint32_t *temp;
    int testval = 42;
    /* c-lib and init test */
    temp = (uint32_t *)malloc(32);
    free(temp);

    /* open/close test */
    fd = open("/dev/null", 0, 0);
    printf("/dev/null %s a tty.\r\n",isatty(fd)?"is":"is not");
    close(fd);
    
    /* socket/close test */
    sd = socket(AF_UNIX, SOCK_DGRAM, 0);
    close(sd);

    /* Thread create test */
    if (vfork() == 0)
        execve("/bin/idling", (char **)idling_args, NULL);
 
    if (vfork() == 0)
        execve("/bin/fresh", idling_args, NULL);

    /* Install binutils */
    if (vfork() == 0)
        execve("/bin/binutils", (char **)binutils_args, NULL);
      
    while(1) {
        pid = wait(&status);
    }
    return 0;
}

