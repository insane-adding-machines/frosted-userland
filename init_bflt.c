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

const char idling_txt[10] = "idling";
const char fresh_txt[10] = "fresh";
const char binutils_txt[10] = "binutils";

const char idling_path[30] = "/bin/idling";
const char fresh_path[30] = "/bin/fresh";
const char binutils_path[30] = "/bin/binutils";


static const char *fresh_args[2] = {fresh_txt, NULL};
static const char *idling_args[2] = {idling_txt, NULL};
static const char *binutils_args[2] = {binutils_txt, NULL};


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
    close(fd);
    
    /* socket/close test */
    sd = socket(AF_UNIX, SOCK_DGRAM, 0);
    close(sd);

    /* Thread create test */
    if (vfork() == 0)
        execve(idling_path, idling_args, NULL);
 
    if (vfork() == 0)
        execve(fresh_path, fresh_args, NULL);

    /* Install binutils */
    if (vfork() == 0)
        execve(binutils_path, binutils_args, NULL);
      
    while(1) {
        pid = wait(&status);
    }
    return 0;
}

