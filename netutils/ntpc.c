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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#ifndef htonl
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#endif

/* NTP options
*/
#define NTP_FLAGS       0x23 /* no LI, NTPv4, client */
#define NTP_STRATUM     0x03 /* secondary reference */
#define NTP_POLL_INT    0x06 /* 64 seconds.. */
#define NTP_CLK_PREC    0xE9 /* 00000 */
#define NTP_ROOT_DELAY  0x00000462
#define NTP_ROOT_DISP   0x00000A05
#define NTP_MONLIST     0x2A
#define NTP_PORT        123
#define MAX_LEN         100
#define MAX_DIFF        5
#define TIME_OUT        15

struct ntp_request {
    uint8_t flags;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_disp;
    uint32_t ref_id;
    uint32_t ref_s;
    uint32_t ref_f;
    uint32_t origin_s;
    uint32_t origin_f;
    uint32_t recv_s;
    uint32_t recv_f;
    uint32_t transmit_s;
    uint32_t transmit_f;
};

#define OFFSET 2208988800ULL

int mk_ntp(uint8_t *buf)
{
    struct timeval ref;
    gettimeofday(&ref, NULL);
    struct ntp_request *ntp = NULL;
    int len = 0;

    ntp = (struct ntp_request *)buf;
    memset(buf, 0, MAX_LEN);

    ntp->flags      = NTP_FLAGS;
    ntp->stratum    = NTP_STRATUM;
    ntp->poll       = NTP_POLL_INT;
    ntp->precision  = NTP_CLK_PREC;
    ntp->root_delay = NTP_ROOT_DELAY;
    ntp->root_disp  = NTP_ROOT_DISP;
    ntp->ref_id = 0x00;

    len = sizeof (struct ntp_request);

    return len;
}

void create_socket(int *s)
{
    *s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*s < 0) {
        fprintf(stderr, "Cannot open socket!\n");
        exit(1);
    }
}

int query_ntp(int nfd, char *host)
{
    unsigned char buffer[MAX_LEN] = { 0 };
    struct sockaddr_in dest;
    int destsz;
    struct timeval tv;

    int len = mk_ntp(buffer);

    dest.sin_family = AF_INET;
    inet_aton(host, &dest.sin_addr);
    dest.sin_port = htons(NTP_PORT);
    destsz = sizeof (dest);

    if (sendto(nfd, (char *)buffer, len, 0, (struct sockaddr *)&dest, destsz) < 0) {
        printf("ntpc: write fail to: %s", host);
        close(nfd);
        return -1;
    }

    memset(buffer, 0, MAX_LEN);

    struct pollfd pfd;
    int pollret;
    pfd.fd = nfd;
    pfd.events = POLLIN | POLLHUP | POLLERR;
    pollret = poll(&pfd, 1, (TIME_OUT * 1000));

    if (pollret < 0) {
        fprintf(stderr, "ntpc: poll returned -1\r\n");
        return -1;
    }

    if ((pfd.revents & (POLLHUP | POLLERR)) != 0) {
       fprintf(stderr, "ntpc: remote peer has closed connection.\r\n");
       return -1;
    }

    if (pfd.revents & POLLIN) {
        int r = recvfrom(nfd, (char *)buffer, MAX_LEN, 0, (struct sockaddr *)&dest, (socklen_t *)&destsz);
        if (r > 0) {
            struct ntp_request *ntp = (struct ntp_request *)buffer;
            time_t sec;

            sec = ntohl(ntp->transmit_s) - OFFSET;
            gettimeofday(&tv, NULL);

            if ((tv.tv_sec > (sec + MAX_DIFF)) || (sec > (tv.tv_sec + MAX_DIFF))) {
                tv.tv_sec = sec;
                settimeofday(&tv, NULL);
            }

            return 0;
        }
    }
}

int main(int argc, char **argv)
{
    struct addrinfo *res;
    struct sockaddr_in *sai;
    int nfd;

    create_socket(&nfd);

    while (2 > 1) {
        int retval = getaddrinfo(argv[1], NULL, NULL, &res);

        if (retval < 0) {
            fprintf(stderr, "Cannot resolve %s: %s\r\n", argv[1], strerror(errno));
            sleep(5);
            continue;
        }

        if (res) {
            sai = (struct sockaddr_in *)(res->ai_addr);
            if (query_ntp(nfd, inet_ntoa(sai->sin_addr)) < 0) {
                sleep(5);
            } else {
                sleep(64);
            }
        } else {
            sleep(5);
        }
    }

    // if you're happy and you know it,
    exit(0);
}

