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
#include <sys/ioctl.h>

int main(int argc, char *args[])
{
    int fd;
    int i;
    char *buf;
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0) {
        printf("Could not open /dev/fb0\r\n");
        exit(1);
    } else {
        int r;
        struct fb_var_screeninfo scr;
        if (ioctl(fd, IOCTL_FB_GET_VSCREENINFO, &scr) < 0) {
            printf("IOCTL_FB_GET_VSCREENINFO: error.\r\n");
            exit(2);
        }
        printf("Screen information:\r\nResolution: %lux%lu@%lubpp - mmap address: %08x size: %lu\r\n",
                scr.xres, scr.yres, scr.bits_per_pixel, scr.smem_start, scr.smem_len);
        buf = (char *)scr.smem_start;
        for (i = 0; i < scr.smem_len; i++) { 
            buf[i] = rand(); 
        }
        close(fd);
    }
    exit(0);
}
