
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
 *      Authors: brabo
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#define PLAY_SIZE 8192
static int16_t x, y, z;
static int lx, ly, lz, rx=100, ry=200, rz=300;

void *acc_task(void *arg) {
    int fd;
    sleep(5);
    fd = open("/dev/ins", O_RDONLY);
    (void)arg;
    if (fd < 0) {
        fprintf(stderr, "Opening /dev/ins: %s\n", strerror(errno));
        exit(1);
    }
    while (1) {
        uint8_t buf[6];
        int r = read(fd, buf, 6);
        if (r < 0) {
            fprintf(stderr, "Reading from /dev/ins: %s\n", strerror(errno));
            exit(2);
        }
        x = (buf[0] + (buf[1] << 8));
        y = (buf[2] + (buf[3] << 8));
        z = (0 - (buf[4] + (buf[5] << 8)));
        if (x > 0) {
            rx = 0;
            lx = (x >> 9);
            if (lx > 31)
                lx = 31;
        } else {
            lx = 0;
            rx = ((0 - x) >> 9);
            if (rx > 31)
                rx = 31;
        }
        if (y > 0) {
            ry = 0;
            ly = (y >> 9);
            if (ly > 31)
                ly = 31;
        } else {
            ly = 0;
            ry = ((0 - y) >> 9);
            if (ry > 31)
                ry = 31;
        }
        if (z > 0) {
            rz = 0;
            lz = (z >> 9);
            if (lz > 31)
                lz = 31;
        } else {
            lz = 0;
            rz = ((0 - z) >> 9);
            if (rz > 31)
                rz = 31;
        }
        //printf("x y z: %hd %hd %hd\r\n", x,y,z);
        usleep(30000);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int dsp;
    int i, r = 0;
    int t = 0;
    char *buffer;
    struct stat st;
    pthread_t acc;
    dsp = open("/dev/dsp", O_WRONLY);
    if (dsp < 0) {
        fprintf(stderr, "Error opening /dev/dsp: %s\r\n", strerror(errno));
        exit(2);
    }
    buffer = malloc(PLAY_SIZE);
    if (!buffer) {
        fprintf(stderr, "Error allocating memory for %s: %s\r\n", argv[i], strerror(errno));
        exit(2);
    }
    pthread_create(&acc, NULL, acc_task, NULL);
    

    setpriority(PRIO_PROCESS, getpid(), -20);
    for(;;t++) { 
        buffer[r] = 0;
        if (rx)
            buffer[r] |= ((t >> (rx >> 2)) | (t >> 4));
        if (lx)
            buffer[r] |= ((t << (lx >> 4)) | (t >> 4));
        if (ry)
            buffer[r] |= (t >> ry) | (t >> 12);
        if (ly)
            buffer[r] |= (t << ly) | (t >> 12);
        if (rz)
            buffer[r] |= ((t >> rz) & (t >> 6));
        if (lz)
            buffer[r] |= ((t << lz) & (t >> 6));

        buffer[r++] *= (t >> 4);

        if (r >= PLAY_SIZE) {
            write(dsp, buffer, PLAY_SIZE);
            r = 0;
        }
    }
	exit(0);
}
