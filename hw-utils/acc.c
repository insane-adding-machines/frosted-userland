#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


int main(void) {
    int fd = open("/dev/ins", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Opening /dev/ins: %s\n", strerror(errno));
        exit(1);
    }
    while (1) {
        int16_t x, y, z;
        uint8_t buf[6];
        int r = read(fd, buf, 6);
        if (r < 0) {
            fprintf(stderr, "Reading from /dev/ins: %s\n", strerror(errno));
            exit(2);
        }

        x = buf[0] + (buf[1] << 8);
        y = buf[2] + (buf[3] << 8);
        z = buf[4] + (buf[5] << 8);

        printf("X: %d ~ Y: %d ~ Z: %d\n", x, y, z);
    }
    return 0;
}
