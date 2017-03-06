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
 *      Authors: Daniele Lacamera, brabo
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>

#define TEST_WAIT
//#define TEST_TRYWAIT
//#define TEST_TIMEDWAIT

#define TEST test_wait



sem_t sem;

#define ERROR 1
#define SUCCESS 0

static volatile int result = ERROR; 


#ifdef TEST_WAIT
void *test_wait(void *arg)
{
    int i;
    for (i = 0; i < 3; i++) {
        int r = sem_wait(&sem);
		if (r == 0)
           printf("Consumed\r\n");
        else {
           printf("Error! sem_wait returned %d!\r\n", r);
           pthread_exit(NULL);
        }
    }
    result = SUCCESS;
    pthread_exit(NULL);
}

#endif

#ifdef TEST_TRYWAIT

void *test_trywait(void *arg)
{
    int i;
    int count_up = 0;
    int count_down = 0;

    while (1<2) {
        if (sem_trywait(&sem) == 0) {
            printf("Consumed\r\n");
            count_up++;
        } else {
            printf("Semaphore down!\r\n");
            count_down++;
            sleep(1);
        }
        printf("Up: %d, down: %d\n", count_up, count_down);
        /* Successfully acquired lock x 3 */
        if ((count_up == 3) && (count_down == 2)) {
            result = SUCCESS;
            pthread_exit(NULL);
        }
        /* Check that the semaphore is acquired twice at startup */
        if ((count_down > 0) && (count_up > 2))
            pthread_exit(NULL);

        /* Check that the third acquisition is after 2 failures */
        if ((count_down < 2) && (count_up > 2))
            pthread_exit(NULL);

    }
}
#endif

#ifdef TEST_TIMEDWAIT
void *test_timedwait(void *arg)
{
    int i;
	int c = 0;
    struct timeval tv;
    struct timespec ts;
    for (i = 0; i < 4; i++) {
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + 4;
        ts.tv_nsec = tv.tv_usec * 1000;
        if (sem_timedwait(&sem, &ts) == 0) {
           printf("Consumed\r\n");
           c++;
		} else {
           printf("Timeout!\r\n");
           if (c == 3) {
                result = SUCCESS;
           }
        }
    }
    pthread_exit(NULL);
}


#endif



int main(int argc, char *args[])
{
    int i;
    pthread_t cons;
    int ret = sem_init(&sem, 0, 2);

    printf("Semaphore initialized ret == %d\r\n", ret);
    ret = pthread_create(&cons, NULL, TEST, &sem);
    printf("pthread_create returned %d\r\n", ret);
    sleep(2);
    ret = sem_post(&sem);
    printf("sem_post returned %d\r\n", ret);
    ret = pthread_join(cons, NULL);
    printf("pthread_join returned %d\r\n", ret);
    ret = sem_destroy(&sem);
    printf("sem_destroy returned %d\r\n", ret);

    printf(" \r\n\r\n************************\r\n");
    if (result == SUCCESS) {
        printf("Test successful.\n");
    } else {
        printf("Test FAILED.\n");
    }

    exit(result);
}
