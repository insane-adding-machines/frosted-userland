
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

int main(int argc, char *argv[])
{
    int dsp;
    int i, r = 0;
    int t = 0;
    char *buffer;
    struct stat st;
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
    for(;;t++) { 
        buffer[r++] = (t<<1)|(t>>4)*(((t>>12)|(t>>13)|(t>>6) | ((t>>2)|(t>>4))|(t<<1)|(t<<12)|((t<<5)&~(t>>22))));
        if (r >= PLAY_SIZE) {
            write(dsp, buffer, PLAY_SIZE);
            r = 0;
        }
    }
	exit(0);
}
