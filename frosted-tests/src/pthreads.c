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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void *consumer_thread(void *arg)
{
    int ret;
    pthread_mutex_t *mutex = (pthread_mutex_t *)arg;
    if (!mutex) {
        printf("Bad bad mutex!\n");
        return NULL;
    }

    ret = pthread_mutex_lock(mutex);
    if (!mutex) {
        printf("Bad bad mutex!\n");
        return NULL;
    }

    printf("RET==%d\n", ret);
    printf("Consumer\r\n");
    ret = pthread_mutex_unlock(mutex);
    printf("RET==%d\n", ret);
    pthread_exit(NULL);
}

void *useless_thread(void *arg)
{
    while(1)
        sleep(1);
}

int pthreads_tests(void)
{
    int ret;
    pthread_t cons, useless;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    ret = pthread_create(&cons, NULL, consumer_thread, &mutex);
    printf("RET==%d\n", ret);
 //   if (!mutex) {
 //       printf("Bad bad mutex!\n");
 //       return 1;
 //   }

    ret = pthread_create(&useless, NULL, useless_thread, NULL);
    printf("RET==%d\n", ret);
    ret = pthread_mutex_lock(&mutex);
    if (!mutex) {
        printf("Bad bad mutex!\n");
        return 1;
    }

    printf("RET==%d\n", ret);
    printf("Producer\r\n");
    ret = pthread_mutex_unlock(&mutex);
    if (!mutex) {
        printf("Bad bad mutex!\n");
        return 1;
    }

    printf("RET==%d\n", ret);
    ret = pthread_join(cons, NULL);
    printf("RET==%d\n", ret);
    ret = pthread_mutex_destroy(&mutex);
    printf("RET==%d\n", ret);
    ret = pthread_kill(useless, 0);
    printf("RET==%d\n", ret);
    ret = pthread_join(useless, NULL);
    printf("RET==%d\n", ret);
    pthread_exit(NULL);
    return 0;
}
