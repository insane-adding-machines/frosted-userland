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
 *      Authors: Antonio Cardace
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TASKS_FILE "/sys/tasks"

#ifndef APP_PS_MODULE
int main(int argc, char *args[])
#else
int icebox_ps(int argc, char *args[])
#endif
{
    int fd;
    fd = open(TASKS_FILE, O_RDONLY);
    if (fd < 0) {
        printf("File not found.\r\n");
        exit(5);
    } else {
        int r;
        char buf[64];
        do {
            r = read(fd, buf, 64);
            if (r > 0) {
                write(1, buf, r);
            }
        } while (r > 0);
        close(fd);
    }
    exit(0);
}
