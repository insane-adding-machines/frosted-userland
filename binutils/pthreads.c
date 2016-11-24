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
#include <sys/types.h>
#include <unistd.h>

void *consumer_thread(void *arg)
{
    pthread_mutex_t *mutex = (pthread_mutex_t *)arg;
    pthread_mutex_lock(mutex);
    printf("Consumer\r\n");
    pthread_mutex_unlock(mutex);
    pthread_exit(NULL);
}

int main(int argc, char *args[])
{
    pthread_t cons;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_create(&cons, NULL, consumer_thread, &mutex);
    pthread_mutex_lock(&mutex);
    printf("Producer\r\n");
    pthread_mutex_unlock(&mutex);
    pthread_join(cons, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
}
