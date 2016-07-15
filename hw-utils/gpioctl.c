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


void usage(char *argv0) {
    fprintf(stderr, "Usage: %s -p PORT:PIN -c GPIONAME         -- OR --\r\n"
            "  %s [-cmd] -s /dev/GPIONAME \r\n"
            "Commands: \r\n"
            "\t-c NAME                              - Create new gpio and link to /dev/NAME\r\n"
            "\trequires:\t-p PORT:PIN\r\n"
            "\t-s NAME                              - Setup previously created gpio /dev/NAME\r\n"
            "\tsetup commands:\r\n"
            "\t-d                                   - Destroy gpio and unlink its entry in /dev\r\n"
            "\t-o                                   - Set gpio to output\r\n"
            "\t-i                                   - Set gpio to input\r\n"
            "\t-r                                   - Read gpio value\r\n"
            "\t-w 0|1                               - Set gpio to given value\r\n"
            "\t-t raise|fall|toggle                 - Set trigger mode on read()\r\n",
            argv0, argv0);
    exit(1);
}

int main(int argc, char *argv[])
{
    int fd;
    int i;
    int create = 0;
    int setup = 0;
    int opt;
    struct gpio_req req = { };
    char name[32] = "";
    void *arg = NULL;
    uint32_t var;
    int cmd;
    int retval;
    int gpioget, gpioset;

    while ((opt = getopt(argc, argv, "t:rw:op:dis:c:")) != -1) {
        switch (opt) {
            case 'c':
                create = 1;
                cmd = IOCTL_GPIOMX_CREATE;
                strcpy(name, optarg);
                req.name = name;
                arg = &req; 
                break;
            case 'd':
                cmd = IOCTL_GPIOMX_DESTROY;
                arg = name;
                break;
            case 's':
                setup = 1;
                strcpy(name, optarg);
                break;
            case 'p':
                {
                    char *p = strchr(optarg,':');
                    int base, pin;
                    if (!p)
                        usage(argv[0]);
                    *p = (char)0;
                    p++;
                    base = atoi(optarg);
                    pin = atoi(p);
                    if (base < 0 || pin < 0) 
                        usage(argv[0]);
                    req.pin = pin;
                    req.base = base;
                    break;
                }
            case 'w':
                var = atoi(optarg);
                gpioset = 1;
                break;
            case 'r':
                gpioget = 1;
                break;
            case 'o':
                cmd = IOCTL_GPIO_SET_OUTPUT;
                var = 1;
                arg = (void*)var;
                break;
            case 'i':
                cmd = IOCTL_GPIO_SET_INPUT;
                var = 1;
                arg = (void*)var;
                break;
            case 't':
                cmd = IOCTL_GPIO_SET_TRIGGER;
                arg = &var;
                if (strcmp(optarg,"raise") == 0)
                    var = GPIO_TRIGGER_RAISE;
                else if (strcmp(optarg,"fall") == 0)
                    var = GPIO_TRIGGER_FALL;
                else if (strcmp(optarg,"toggle") == 0)
                    var = GPIO_TRIGGER_TOGGLE;
                else if (strcmp(optarg,"none") == 0)
                    var = GPIO_TRIGGER_NONE;
                else {
                    fprintf(stderr, "I cannot understand '%s'.\r\n", optarg);
                    usage(argv[0]);
                }
                break;
            default: 
                {
                    fprintf(stderr, "Unrecognized command '-%c'\r\n", opt);
                    usage(argv[0]);
                }
        }
    }
    if (!create && !setup) {
        fprintf(stderr, "Please specify one between '-c' and '-s'\n\r");
        usage(argv[0]);
    }

    if (create && setup) {
        fprintf(stderr, "Cannot use '-c' with '-s'\n\r");
        usage(argv[0]);
    }
    if (setup) {
        fd = open(name, O_RDWR);
        if (fd < 0) {
            fprintf(stderr, "cannot open %s: %s\r\n", name, strerror(errno));
            exit(1);
        }
        if (gpioset) {
            char cval = '0';
            if (var)
                cval = '1';
            if (write(fd, &cval, 1) != 1) {
                fprintf(stderr, "cannot set value to %s: %s\r\n", name, strerror(errno));
                close(fd);
                exit(1);
            }
            close(fd);
            exit(0);
        }
        if (gpioget) {
            char cval;
            if (1 != read(fd, &cval, 1)) {
                fprintf(stderr, "cannot read value to %s: %s\r\n", name, strerror(errno));
                close(fd);
                exit(1);
            }
            printf("%c\r\n", cval);
            close(fd);
            exit(0);
        }
    } else {
        fd = open("/dev/gpiomx", O_RDWR);
        if (fd < 0) {
            fprintf(stderr, "cannot open /dev/gpiomx: %s\r\n",  strerror(errno));
            exit(1);
        }
    }
    retval = ioctl(fd, cmd, arg);
    if (retval < 0) {
        fprintf(stderr, "GPIO IOCTL failed: %s\r\n", strerror(errno));
        exit(1);
    }
    exit(0);
}
