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
 *      Authors: Maxime Vincent
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <errno.h>

#ifdef TEST
/* GPIO */
#define IOCTL_GPIO_DISABLE 0
#define IOCTL_GPIO_ENABLE 1
#define IOCTL_GPIO_SET_OUTPUT 2
#define IOCTL_GPIO_SET_INPUT 3
#define IOCTL_GPIO_SET_PULLUPDOWN 4
#define IOCTL_GPIO_SET_ALT_FUNC 5
#define IOCTL_GPIO_SET_TRIGGER  6
/* GPIO mx */
#define IOCTL_GPIOMX_DESTROY    0
#define IOCTL_GPIOMX_CREATE     1

/* IOCTL_GPIO_SET_TRIGGER Arguments: */
#define GPIO_TRIGGER_NONE   ((uint32_t)0)
#define GPIO_TRIGGER_FALL   ((uint32_t)1)
#define GPIO_TRIGGER_RAISE  ((uint32_t)2)
#define GPIO_TRIGGER_TOGGLE ((uint32_t)3)


struct gpio_req {
    uint32_t base;
    uint32_t pin;

};


#endif


int main(int argc, char *argv[])
{
    int fd;
    int ind, outd;
    unsigned char cval = 0;
    int retval;
    uint32_t raise = GPIO_TRIGGER_RAISE;

    if (setpriority(PRIO_PROCESS, getpid(), -20) < 0) {
        fprintf(stderr,"setpriority: %s\r\n", strerror(errno));
        exit(2);
    }

    fd = open("/dev/button", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "cannot open /dev/button: %s\r\n",  strerror(errno));
        exit(1);
    }
    retval = ioctl(fd, IOCTL_GPIO_SET_TRIGGER, &raise);
    if (retval < 0) {
        fprintf(stderr, "GPIO IOCTL failed: %s\r\n", strerror(errno));
        exit(1);
    }
    close(fd);

    ind = open("/dev/button", O_RDONLY);
    if (ind < 0) {
        fprintf(stderr, "cannot open button: %s\r\n", strerror(errno));
        exit(2);
    }
    outd = open("/dev/led0", O_WRONLY);
    if (outd < 0) {
        fprintf(stderr, "cannot open led0: %s\r\n", strerror(errno));
        close(ind);
        exit(2);
    }
    write(outd, &cval, 1);

    /* Start test */
    if (1 != read(ind, &cval, 1)) {
        fprintf(stderr, "cannot read value: %s\r\n", strerror(errno));
        close(ind);
        close(outd);
        exit(1);
    }
    write(outd, &cval, 1);
    exit(0);
}
