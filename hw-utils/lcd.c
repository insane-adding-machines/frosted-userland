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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

uint8_t buffer[33];
uint8_t outbuf[80];

int main(int argc, char *args[])
{
    int i = 1;
    int j = 0;
    int len = 0;
    int lcd = open("/dev/lcd", O_WRONLY);
    char space = 0x20;
    if (lcd < 0) {
        perror("opening LCD device");
        exit(1);
    }
    while (args[i]) {
        for (j = 0; j < strlen(args[i]); j++) {
            buffer[len + j] = args[i][j];
        }
        len += j;
        if (len != 16)
            buffer[len++] = ' ';
        if (len > 32) {
            len = 32;
            break;
        }
        i++;
    }
    memset(outbuf, space, 80);
    outbuf[79] = '\0';
    memcpy(outbuf, buffer, 16);
    memcpy(outbuf + 40, buffer + 16, 16);
    if (len > 16) {
        len = 24 + len;
    }
    write(lcd, outbuf, len);
    exit(0);
}
