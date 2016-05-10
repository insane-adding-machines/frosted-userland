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

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

static int received_signal = 0;

void usr1_hdlr(int signo)
{
    //sleep(1);
    received_signal = signo;

}

int main(int argc, char *argv[])
{
    int pid;
    int led[4];
    int i, j;
    struct sigaction act = {};
    act.sa_handler = usr1_hdlr;

    sigaction(SIGUSR1, &act, NULL); 

# define LED0 "/dev/led0"
# define LED1 "/dev/led1"
# define LED2 "/dev/led2"
# define LED3 "/dev/led3"

    led[0] = open(LED0, O_RDWR, 0);
    led[1] = open(LED1, O_RDWR, 0);
    led[2] = open(LED2, O_RDWR, 0);
    led[3] = open(LED3, O_RDWR, 0);

    if (led[0] >= 0) {
        while(1) {
            for (i = 0; i < 9; i++) {
                if (i < 4) {
                    write(led[i], "0", 1);
                } else {
                    char val = (1 - (i % 2)) + '0';
                    for(j = 0; j < 4; j++)
                        write(led[j], &val, 1);
                }
                usleep(200000);
            }
        }
    } else {
        while(1) { sleep(1); } /* GPIO unavailable, just sleep. */
    }
}
