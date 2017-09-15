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

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#ifndef APP_KLOGD_MODULE
int main(void)
#else
int icebox_klogd(int argc, char *argv[])
#endif
{
    int klog = open("/dev/klog", O_RDONLY);
    if (klog >= 0){
        char logbuf[32];
        int ret;
        while(1) {
            ret = read(klog, logbuf, 31);
            if (ret > 0) {
                logbuf[ret] = '\0';
                fprintf(stderr, "%s", logbuf);
            }
        }
    } else {
        fprintf(stderr, "Cannot open /dev/klog. Is kernel debug enabled?\r\n");
        exit(111);
    }
}
