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
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>

#define WR_SIZE (100)

char buf[WR_SIZE];

int main(int argc, char *args[])
{
    int fd;
    int i;
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0) {
        printf("Could not open /dev/fb0\r\n");
        exit(-1);
    } else {
        int r;
        do {
            for (i = 0; i < WR_SIZE; i++) { buf[i] = rand(); }
            r = write(fd, buf, WR_SIZE);
        } while (r > 0);
        close(fd);
    }
    exit(0);
}
