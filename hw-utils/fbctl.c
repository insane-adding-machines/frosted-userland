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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <errno.h>

static int screen_ctl(int val)
{
    int fd1;
    char cval = '0';
    int fd0 = open("/dev/backlight", O_RDWR);
    if (fd0 < 0)
        return -1;
    fd1 = open("/dev/display", O_RDWR);
    if (fd1 < 0) {
        close(fd0);
        return -1;
    }
    if (val)
        cval = '1';

    write(fd0, &cval, 1);
    write(fd1, &cval, 1);
    close(fd0);
    close(fd1);
    return 0;
}

#define screen_on() screen_ctl(1)
#define screen_off() screen_ctl(0)

int main(int argc, char *argv[])
{
    if (argc != 2 || (strcmp(argv[1], "on") && strcmp(argv[1], "off"))) {
        fprintf(stderr, "Usage %s on | off\r\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "on") == 0)
        screen_on();
    else
        screen_off();
    return 0;
}

