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

#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

static sem_t *semaphore;
static int myarg = 42;


void *consumer_thread(void *arg)
{
    int r_arg = *((int *)(arg));
    int i;

    if (r_arg != myarg)
        printf("Bad arg value!\n");

    while(1) {
        i == 0;
        sem_wait(semaphore);
        printf("C\r\n");
    }
}

int main(int argc, char *args[])
{
    int fd;
    semaphore = sem_init(0);
    pthread_t cons ;

    pthread_create(&cons, NULL, consumer_thread, &myarg);

    while(1) {
        sleep(1);
        printf("P\r\n");
        sem_post(semaphore);
    }
    exit(0);
}
