
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
#define PLAY_SIZE 4096

#ifndef APP_PLAY_MODULE
int main(int argc, char *argv[])
#else
int icebox_play(int argc, char *argv[])
#endif
{
    int dsp;
    int fd;
    int i, r;
    char *buffer;
    struct stat st;
    if  (argc < 2) {
        fprintf(stderr, "Usage: %s file1 file2 ...\r\n");
        exit(1);
    }
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
    for (i = 1; i < argc; i++) {
        if (stat(argv[i], &st) < 0) {
            fprintf(stderr, "Error opening %s: %s\r\n", argv[i], strerror(errno));
            exit(3);
        }
        fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Error opening %s: %s\r\n", argv[i], strerror(errno));
            exit(3);
        }
        do {
            r = read(fd, buffer, PLAY_SIZE);
            if (r > 0 ) {
                write(dsp, buffer, r);
            }
        } while (r > 0);
        close(fd);
    }
	exit(0);
}
