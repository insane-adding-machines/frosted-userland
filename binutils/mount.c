#include "frosted_binutils.h"
#include <signal.h>


 int mount(const char *source, const char *target,
                          const char *filesystemtype, unsigned long mountflags,
                                           const void *data);


int main(int argc, char *argv[])
{
    int pid;
    if ((argc != 4) && (argc != 1)) {
        printf("Usage: %s source mountpoint fstype\r\n", argv[0]);
        exit(1);
    }
    if (argc == 1) {
        int fd = open("/sys/mtab", O_RDONLY);
        char buf[20];
        int r;
        if (fd < 0) {
            printf("No support for mount on this system!\n");
            exit(1);
        }
        do {
            r = read(fd, buf, 20);
            if (r > 0) {
                write(1, buf, r);
            }
        } while (r > 0);
    } else if (argc == 4) {
        if(mount(argv[1], argv[2], argv[3], 0, NULL) == 0) {
            printf("Successfully mounted %s on %s, type=%s\n", argv[1], argv[2], argv[3]);
        }
    }
    exit(0);
}
