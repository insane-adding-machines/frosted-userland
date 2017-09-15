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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

 int mount(const char *source, const char *target,
                          const char *filesystemtype, unsigned long mountflags,
                                           const void *data);
#ifndef APP_MOUNT_MODULE
int main(int argc, char *argv[])
#else
int icebox_mount(int argc, char *argv[])
#endif
{
    int pid;
    if ((argc != 4) && (argc != 1)) {
        printf("Usage: %s source mountpoint fstype\r\n", argv[0]);
        exit(1);
    }
    if (argc == 1) {
        int fd = open("/sys/mtab", O_RDONLY);
        char buf[20];
        int r;
        if (fd < 0) {
            printf("No support for mount on this system!\n");
            exit(1);
        }
        do {
            r = read(fd, buf, 20);
            if (r > 0) {
                write(1, buf, r);
            }
        } while (r > 0);
    } else if (argc == 4) {
        if(mount(argv[1], argv[2], argv[3], 0, NULL) == 0) {
            fprintf(stderr, "Successfully mounted %s on %s, type=%s\r\n", argv[1], argv[2], argv[3]);
        } else {
            fprintf(stderr, "Could not mount %s on %s (type %s): %s\r\n", argv[1], argv[2], argv[3], strerror(errno));
        }
    }
    fflush(stderr);
    exit(0);
}
