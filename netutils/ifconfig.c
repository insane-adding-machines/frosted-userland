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

static int is_digit(char a)
{
    if (a < '0' || a > '9')
        return 0;
    return 1;
}

int inet_aton(const char *cp, struct in_addr *inp)
{
    int cnt = 0;
    char p;
    uint8_t buf[4] = {};
    while((p = *cp++) != 0 && cnt < 4)
    {
        if (is_digit(p)) {
            buf[cnt] = (uint8_t)((10 * buf[cnt]) + (p - '0'));
        } else if (p == '.') {
            cnt++;
        } else {
            return -1;
        }
    }
    /* Handle short notation */
    if (cnt == 1) {
        buf[3] = buf[1];
        buf[1] = 0;
        buf[2] = 0;
    } else if (cnt == 2) {
        buf[3] = buf[2];
        buf[2] = 0;
    } else if (cnt != 3) {
        /* String could not be parsed, return error */
        return -1;
    }

    inp->s_addr = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
    inp->s_addr = htonl(inp->s_addr);
    return 0;
}

static char inet_ntoa_str[20];
char *inet_ntoa(struct in_addr a)
{
    int val[4];
    memset(inet_ntoa_str, 0, 20);
    val[0] = (a.s_addr & 0x000000FF);
    val[1] = (a.s_addr & 0x0000FF00) >> 8;
    val[2] = (a.s_addr & 0x00FF0000) >> 16;
    val[3] = (a.s_addr & 0xFF000000) >> 24;
    snprintf(inet_ntoa_str, 20, "%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
    return inet_ntoa_str;
}


int
ifdown(char *ifname)
{
	int sck;
	struct ifreq eth;
	int retval = -1;


	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return retval;
	}
	memset(&eth, 0, sizeof(struct ifreq));
	strncpy(eth.ifr_name,ifname,4);
	eth.ifr_flags = 0;
	if(ioctl(sck, SIOCSIFFLAGS, &eth) < 0)
	{
		return retval;
	}
	close(sck);
	return 0;
}

int
ifup(char *ifname)
{
	int sck;
	struct ifreq eth;
	int retval = -1;


	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return retval;
	}
	memset(&eth, 0, sizeof(struct ifreq));
	strncpy(eth.ifr_name,ifname,4);
	eth.ifr_flags = IFF_UP|IFF_RUNNING|IFF_MULTICAST|IFF_BROADCAST;
	if(ioctl(sck, SIOCSIFFLAGS, &eth) < 0)
	{
		return retval;
	}
	close(sck);
	return 0;
}


#if 0
int
ifconfig_mac(char *ifname, unsigned char *mac_address)
{
	int sck;
	struct ifreq eth;
	int retval = -1;
	int e;

	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return retval;
	}
	memset(&eth, 0, sizeof(struct ifreq));
	strncpy(eth.ifr_name,ifname,4);
	eth.ifr_flags = IFF_UP|IFF_RUNNING|IFF_MULTICAST|IFF_BROADCAST;

        /* Fill in the structure */
        eth.ifr_hwaddr.sa_family = ARPHRD_ETHER;

	memcpy (&eth.ifr_hwaddr.sa_data, mac_address, 6);

        /* call the IOCTL */
        if (ioctl(sck, SIOCSIFHWADDR, &eth) < 0) {
                perror("ioctl(SIOCSIFHWADDR)");
                goto ipfail;
        }


	close(sck);
	return 0;

ipfail:
	e = errno;
	close(sck);
	errno = e;
	return -1;
}
#endif

int
ifconfig(char *ifname, char *address, char *netmask)
{
	int sck;
	struct ifreq eth;
	struct sockaddr_in addr = {.sin_family = 0}, nmask = {.sin_family = 0};
	struct in_addr tmpaddr;
	int retval = -1;
	int e;

	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return retval;
	}
	memset(&eth, 0, sizeof(struct ifreq));
	strncpy(eth.ifr_name,ifname,4);
	eth.ifr_flags = IFF_UP|IFF_RUNNING|IFF_MULTICAST|IFF_BROADCAST;

	if(ioctl(sck, SIOCSIFFLAGS, &eth) < 0)
	{
		goto ipfail;
	}

	addr.sin_family = AF_INET;
	inet_aton(address, &tmpaddr);
	addr.sin_addr.s_addr = tmpaddr.s_addr;
	//addr = (struct sockaddr_in *) &eth.ifr_addr;
	memcpy (&eth.ifr_addr, &addr, sizeof (struct sockaddr_in));

	if(ioctl(sck, SIOCSIFADDR, &eth) < 0){
	       	goto ipfail;
	}

	nmask.sin_family = AF_INET;
	inet_aton(netmask, &tmpaddr);
	nmask.sin_addr.s_addr =  tmpaddr.s_addr;
	memcpy (&eth.ifr_netmask, &nmask, sizeof (struct sockaddr_in));

	if(ioctl(sck, SIOCSIFNETMASK, &eth) < 0){
	       	goto ipfail;
	}
	close(sck);
	return 0;

ipfail:
	e = errno;
	close(sck);
	errno = e;
	return -1;
}


int ifconf_getproperties(char *ifname, uint8_t *macaddr, struct sockaddr_in *address, struct sockaddr_in *netmask, struct sockaddr_in *broadcast)
{
	int sck;
	struct ifreq ifr;
	struct sockaddr_in addr = {.sin_family = 0}, nmask = {.sin_family = 0}, bcast = {.sin_family = 0};
	uint8_t pmac[6];

	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		perror("socket");
		return -1;
	}

	// save interface name
	strcpy(ifr.ifr_name, ifname);

	// IP Address
	if (ioctl(sck, SIOCGIFADDR, &ifr) < 0) {
		close(sck);
		return -1;
	} else {
		memcpy( &addr, (struct sockaddr_in *) &ifr.ifr_addr, sizeof(struct sockaddr_in));
	}

    /*
	// Mac Address
	if(ioctl(sck, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl(SIOCGIFHWADDR)");
	} else {
		memcpy(pmac, (unsigned char *)(&ifr.ifr_ifru.ifru_hwaddr.sa_data), 6);
	}
    */


	// Broadcast Address
	if (ioctl(sck, SIOCGIFBRDADDR, &ifr) < 0) {
		perror("ioctl(SIOCGIFBRDADDR)");
	} else {
		memcpy( &bcast, (struct sockaddr_in *) &ifr.ifr_broadaddr, sizeof(struct sockaddr_in));
	}

	// NetMask Address
	if (ioctl(sck, SIOCGIFNETMASK, &ifr) < 0) {
		perror("ioctl(SIOCGIFNETMASK)");
	} else {
		memcpy( &nmask, (struct sockaddr_in *) &ifr.ifr_netmask, sizeof(struct sockaddr_in));
	}

	if (macaddr)
		memcpy(macaddr, pmac, 6);
	if (address)
		memcpy(address, &addr, sizeof(struct sockaddr_in));
	if (netmask)
		memcpy(netmask, &nmask, sizeof(struct sockaddr_in));
	if (broadcast)
		memcpy(broadcast, &bcast, sizeof(struct sockaddr_in));


	if (ioctl(sck, SIOCGIFFLAGS, &ifr) < 0){
		close(sck);
		return -1;
	}

	close(sck);
	return  (ifr.ifr_flags & IFF_UP);

}

#if 0
int
iflinksense(char *ifname)
{
	int sck;
	struct ifreq eth;
	int retval = -1;
    struct ethtool_value ec;
	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(sck < 0){
		return retval;
	}
	memset(&eth, 0, sizeof(struct ifreq));
	strncpy(eth.ifr_name,ifname,4);

    memset(&ec, 0, sizeof(struct ethtool_value));
    ec.cmd = ETHTOOL_GLINK;
    eth.ifr_data = (char *) &ec;

    if (ioctl(sck, SIOCETHTOOL, &eth) < 0) {
        close(sck);
        return -1;
    }
	close(sck);

    if (ec.data)
        return 1;
    else
        return 0;
}
#endif

int ifconf_status(char *ifname) { return ifconf_getproperties(ifname, NULL, NULL, NULL, NULL); }
int ifconf_getmac(char *ifname, uint8_t *mac) { return ifconf_getproperties(ifname, mac, NULL, NULL, NULL); }
int ifconf_getaddress(char *ifname, struct sockaddr_in *address) { return ifconf_getproperties(ifname, NULL, address, NULL, NULL); }
int ifconf_getnetmask(char *ifname, struct sockaddr_in *netmask) { return ifconf_getproperties(ifname, NULL, NULL, netmask, NULL); }
int ifconf_getbroadcast(char *ifname, struct sockaddr_in *broadcast) { return ifconf_getproperties(ifname, NULL, NULL, NULL, broadcast); }
int iflinksense(char *ifname);

#define PROC_NET_DEV "/sys/net/dev"

void ifconf_show(char *name)
{
    struct sockaddr_in a,n,b;
    int ret;
    ret = ifconf_getproperties(name, NULL, &a, &n, &b);
    if (ret < 0) {
        perror("Cannot get interface properties");
        printf("Cannot find interface %s\r\n", name);
        exit(1);
    }
    printf("%s: flags:%s mtu 1500\r\n", name, (ret > 0)?"<UP,BROADCAST,MULTICAST,RUNNING>":"<DOWN>");
    printf("        inet %s ", inet_ntoa(a.sin_addr));
    printf("netmask %s ", inet_ntoa(n.sin_addr));
    printf("broadcast %s\r\n", inet_ntoa(b.sin_addr));
    printf("\r\n");
}

static void usage(char *name)
{
    printf("Usage: %s [DEV [ADDR [netmask NMASK]]]\r\n", name);
    exit(1);
}

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int ret;
    char *p;
    char *prev = NULL;
    struct ifreq ifr;
    if (argc == 1) {
        char ifbuffer[256];
        fd = open(PROC_NET_DEV, O_RDONLY);
        if (fd < 0) {
            printf("Ifconfig: error reading from %s\r\n", PROC_NET_DEV);
            exit(1);
        }
        ret = read(fd, ifbuffer, 255);
        close(fd);

        if (ret <= 0) {
            printf("Error.\r\n");
            exit(1);
        }
        for (i = 0; i < ret; i++) {
            if ((ifbuffer[i] == '\r') || (ifbuffer[i] == ':'))
                ifbuffer[i] = '\0';
            if (ifbuffer[i] == '\n') {
                    ifbuffer[i] = '\0';
                if (prev) {
                    ifconf_show(prev);
                }
                prev = &ifbuffer[i+1];
            }
        }
        exit(0);
    }
    memset(&ifr, 0, sizeof(struct ifreq));

    if (argc == 2) {
        ifconf_show(argv[1]);
        exit(0);
    }

    if (argc == 3) {
        ifconfig(argv[1], argv[2], "255.255.255.0");
        exit(0);
    }
    if (argc == 5) {
        if (strcmp(argv[3], "netmask") != 0) {
            usage(argv[0]);
        }
        ifconfig(argv[1], argv[2], argv[4]);
        exit(0);
    }

    usage(argv[0]);
}
