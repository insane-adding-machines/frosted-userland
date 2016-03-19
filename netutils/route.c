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

#define _BSD_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>      /* for fprintf etc */
#include <fcntl.h>

#ifndef htonl
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#endif

static int route_add(char *dst, char *nm, char *gw, int metric, char *dev)
{
    struct rtentry rte;
    int sck;
	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return -1;
	}
	memset(&rte, 0, sizeof(struct rtentry));
	if(ioctl(sck, SIOCADDRT, &rte) < 0)
        return -1;
    return 0;
}

static int route_del(char *dst, char *nm, int metric)
{
    struct rtentry rte;
    int sck;
	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return -1;
	}
	memset(&rte, 0, sizeof(struct rtentry));
	if(ioctl(sck, SIOCDELRT, &rte) < 0)
        return -1;
    return 0;
}

#define SYS_NET_ROUTE "/sys/net/route"
static int route_list(void)
{
    char ifbuffer[256];
    int fd, ret;
    fd = open(SYS_NET_ROUTE, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "route: error reading from %s\r\n", SYS_NET_ROUTE);
        exit(1);
    }
    do { 
        ret = read(fd, ifbuffer, 255);
        if (ret > 0) {
            write(STDOUT_FILENO, ifbuffer, ret);
        }
    } while (ret > 0);
    close(fd);
    exit(0);
}

void usage(void)
{
    fprintf(stderr, "Usage: route [-n]\r\n\troute add <-net|-host> $dest [$netmask] <gw|dev> $gateway|$dev [<metric> $metric]\r\n\rroute del <-net|-host> $dest [$netmask] [<metric> $metric]\r\n");
    exit(2);
}

static char host_netmask[] = "255.255.255.255";
int main(int argc, char *argv[])
{
    int i;
    int fd;
    int ret;
    char *p;
    char *prev = NULL;
    char *dest, *nm, *gw=NULL, *dev=NULL, *metric=NULL;
    struct ifreq ifr;
    int parsed = 1;

    if ( (argc == 1) || ((argc == 2) && (strcmp(argv[1], "-n") == 0)) ) {
        route_list();
    }

    if (argc < 4) {
        usage();
    }

    if (strcmp(argv[1],"add") == 0) 
    {
        /* TODO: parse arguments */
        route_add(dest, nm, gw, atoi(metric), dev);
    } else if (strcmp(argv[1],"del") == 0) {
        /* TODO: parse arguments */
        route_del(dest, nm, atoi(metric));
    } else usage();
}
