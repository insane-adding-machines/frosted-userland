
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#define DEFAULT_LEN (64)

static uint8_t payload[DEFAULT_LEN];

static uint32_t time_ms(void)
{
    struct timeval tv;
    uint32_t ret;
    gettimeofday(&tv, NULL);
    ret = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    return ret;
}

int ping(struct sockaddr_in *dst, int count, int len)
{
    struct icmphdr *icmp_hdr = (struct icmphdr *)payload;
    int i;
    int sequence = 0;
    int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_ICMP);
    struct pollfd pfd;
    struct sockaddr_in reply_from;
    uint32_t now, echo_time;
    socklen_t sockaddr_in_len = sizeof(struct sockaddr_in);


    if (sock < 0) {
        perror("socket");
        return -1;
    }
    pfd.events = POLLIN;
    pfd.fd = sock;


    i = 0;
    while((count == 0) || (i < count)) {
        memset(icmp_hdr, 0, sizeof(struct icmphdr));
        icmp_hdr->type = ICMP_ECHO;
        icmp_hdr->un.echo.id = 1234;
    
        for(i = sizeof(struct icmphdr); i < sizeof(struct icmphdr) + len; i++) {
            payload[i] = i & 0xFF;
        }
        icmp_hdr->un.echo.sequence = sequence++;
        echo_time = time_ms();
        if(sendto(sock, payload, len, 0, (struct sockaddr *)dst, sizeof(struct sockaddr_in)) < 0) 
        {
            perror("send");
            sleep(1);
        } else {
            int pollret = poll(&pfd, 1, 1000);
            if (pollret == 0) {
                printf("Timeout\r\n");
            } else if (pollret == -1) {
                perror("poll");
                return -1;
            } else {
                char *ip;
                int r = recvfrom(sock, payload, DEFAULT_LEN, 0, (struct sockaddr *)&reply_from, &sockaddr_in_len);
                if (r <= 0) {
                    perror("recvfrom");
                    return -1;
                }
                now = time_ms();
                ip = inet_ntoa(reply_from.sin_addr);
                printf("%d bytes from %s: icmp seq=%d time=%d ms\r\n", r, ip, icmp_hdr->un.echo.sequence, now - echo_time);
                usleep(1000000 - (1000 * (now - echo_time)));
            }
        }
    }
}


int main(int argc, char *argv[])
{
    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(struct sockaddr_in));
    dst.sin_family = AF_INET;
    if (argc != 2) {
        printf("usage: %s destination_ip\n", argv[0]);
        return 1;
    }

    if (inet_aton(argv[1], &dst.sin_addr) == 0) {
        perror("inet_aton");
        printf("%s isn't a valid IP address\n", argv[1]);
        return 1;
    }
    ping(&dst, 0, DEFAULT_LEN);
    return 0;
}
