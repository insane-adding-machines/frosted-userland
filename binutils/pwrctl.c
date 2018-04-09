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
 *      Authors: danielinux
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#ifndef APP_PWRCTL_MODULE
int main(int argc, char *argv[])
#else
int icebox_pwrctl(int argc, char *argv[])
#endif
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s suspend|standby interval\r\n", argv[0]);
        exit(1);
    }
    if (strcmp(argv[1], "suspend") == 0) {
        suspend(atoi(argv[2]));
    }
    if (strcmp(argv[1], "standby") == 0) {
        standby(atoi(argv[2]));
    }
    exit(0); /* Never reached in case of stanbdby */
}
