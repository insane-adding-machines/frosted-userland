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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


int main(void) {
    int fd = open("/dev/ins", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Opening /dev/ins: %s\n", strerror(errno));
        exit(1);
    }
    while (1) {
        int16_t x, y, z;
        uint8_t buf[6];
        int r = read(fd, buf, 6);
        if (r < 0) {
            fprintf(stderr, "Reading from /dev/ins: %s\n", strerror(errno));
            exit(2);
        }

        x = buf[0] + (buf[1] << 8);
        y = buf[2] + (buf[3] << 8);
        z = buf[4] + (buf[5] << 8);

        printf("X: %d ~ Y: %d ~ Z: %d\n", x, y, z);
    }
    return 0;
}
