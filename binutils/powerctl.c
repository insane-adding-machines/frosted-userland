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
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>

static char Relay0[]="relay0";
static char Relay1[]="relay1";


static void usage(char *argv0) {
    fprintf(stderr, "Usage: %s [ -s serial | -t tcp_port ]\r\n", argv0);
    exit(1);
}

#define MODE_NULL 0
#define MODE_SERIAL 1
#define MODE_TCP 6


static int screen_ctl(int val)
{
    int fd1;
    char cval = '0';
    int fd0 = open("/dev/backlight", O_RDWR);
    if (fd0 < 0)
        return -1;
    fd1 = open("/dev/display", O_RDWR);
    if (fd1 < 0) {
        close(fd0);
        return -1;
    }
    if (val)
        cval = '1';

    write(fd0, &cval, 1);
    write(fd1, &cval, 1);
    close(fd0);
    close(fd1);
    return 0;
}

static int relay_ctl(char *name, int val)
{
    char fname[40] = "/dev/";
    char cval = '0';
    int fd;
    strcat(fname, name);
    fd = open(fname, O_RDWR);
    if (fd < 0)
        return -1;
    if (val)
        cval = '1';
    write(fd, &cval, 1);
    close(fd);
}

#define relay_on(fname) relay_ctl(fname, 1)
#define relay_off(fname) relay_ctl(fname, 0)

#define screen_on() screen_ctl(1)
#define screen_off() screen_ctl(0)

static void create_relay(int mx_fd, int base, int pin, char *name)
{
    struct gpio_req req = { };
    int retval;
    int yes = 1;
    char cval = '0';
    char fname[40] = "/dev/";
    int gpio_fd = -1;
    req.base = base;
    req.pin = pin;
    req.name = name;
    retval = ioctl(mx_fd, IOCTL_GPIOMX_CREATE, &req);
    if (retval < 0) {
        fprintf(stderr, "Error creating %s: %s\r\n", name, strerror(errno));
    }


    strcat(fname, name);
    gpio_fd = open(fname, O_RDWR);
    if (gpio_fd < 0) {
        fprintf(stderr, "Error opening %s: %s\r\n", fname, strerror(errno));
        exit(1);
    }
    retval = ioctl(gpio_fd, IOCTL_GPIO_SET_OUTPUT, &yes);
    if (gpio_fd < 0) {
        fprintf(stderr, "Error setting %s as output: %s\r\n", fname, strerror(errno));
        exit(1);
    }
    write(gpio_fd, &cval, 1);
    close(gpio_fd);
}

const char clrscr_txt[] = { 0x1b, '[','H', 0x1b,'[','J', 0x00 };
const char session_on_txt[] = "\r\n\r\nTest Session Starting...\r\nPower activated\r\n";
const char session_off_txt[] = "\r\n\r\nTest Session Finished\r\nCutting power.\r\n";
const char cycle_txt[] = "Requested powercycle.\r\n";

static int status = 0;


static void start_test(void)
{
    int fb;
    if (status > 0)
        return;
    screen_on();
    relay_on(Relay0);
    relay_on(Relay1);
    fb = open("/dev/fbcon", O_WRONLY);
    if (fb < 0)
        return;
    write(fb, clrscr_txt, strlen(clrscr_txt));
    write(fb, session_on_txt, strlen(session_on_txt));
    close(fb);
    status = 1;

}

static void stop_test(void)
{
    int fb;
    if (status == 0)
        return;
    fb = open("/dev/fbcon", O_WRONLY);
    if (fb < 0)
        return;
    write(fb, session_off_txt, strlen(session_off_txt));
    close(fb);
    sleep(1);
    relay_off(Relay0);
    relay_off(Relay1);
    screen_off();
    status = 0;
}

static void power_cycle(void)
{
    int fb;
    if (status == 0)
        return;
    fb = open("/dev/fbcon", O_WRONLY);
    if (fb < 0)
        return;
    write(fb, cycle_txt, strlen(cycle_txt));
    close(fb);
    relay_off(Relay0);
    relay_off(Relay1);
    sleep(1);
    relay_on(Relay0);
    relay_on(Relay1);
}

#ifndef APP_POWERCTL_MODULE
int main(int argc, char *argv[])
#else
int icebox_powerctl(int argc, char *argv[])
#endif
{
    int mx, fbcon, cmd = STDIN_FILENO;
    int i;
    int create = 0;
    int setup = 0;
    int opt;
    char serial_port[32] = "";
    void *arg = NULL;
    uint32_t var;
    int retval;
    int gpioget, gpioset;
    int mode=MODE_NULL, tcp_port = 23;

    //setsid();

    while ((opt = getopt(argc, argv, "t:s:")) != -1) {
        switch (opt) {
            case 't':
                /* TCP Mode */
                tcp_port = atoi(optarg);
                mode = MODE_TCP;
                break;

            case 's':
                /* Serial Mode */
                mode = MODE_SERIAL;
                strncpy(serial_port, optarg, 31);
                break;
        }
    }

    /* Create Relays */
    mx = open("/dev/gpiomx", O_RDWR);
    if (mx < 0) {
        fprintf(stderr, "cannot open /dev/gpiomx: %s\r\n",  strerror(errno));
        exit(1);
    }
    create_relay(mx, 2, 4, Relay0);
    create_relay(mx, 7, 7, Relay1);

    /* Turn off screen */
    screen_off();

    if (mode == MODE_SERIAL) {
        cmd = open(serial_port, O_RDWR);
        if (cmd < 0) { 
            fprintf(stderr, "cannot open %s: %s\r\n",  serial_port, strerror(errno));
            exit(1);
        }
    } else {
        /* TODO: tcp */
    }
    write(cmd, "hello\r\n", 7);
    while(1) {
       struct pollfd pfd;
       int ret;
       char cval;
       pfd.fd = cmd;
       pfd.events = POLLIN;
       ret = poll(&pfd, 1, -1);
       if (ret > 0) {
           ret = read(cmd, &cval, 1);
           if (ret == 1) {
               switch(cval) {
                   case 'g':
                       start_test();
                       continue;
                   case 'x':
                       stop_test();
                       continue;
                   case 'r':
                       power_cycle();
                       continue;
               }
           }
       }
    }
    exit(0);
}
