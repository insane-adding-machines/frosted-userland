#include "frosted_binutils.h"


int main(int argc, char *args)
{
    int fd;
    int i = 1;
    while (args[i]) {
    	int last_white = 0;
    	int bytes = 0, words = 0, newlines = 0;
        fd = open(args[i], O_RDONLY);
        if (fd < 0) {
            printf("File not found.\r\n");
            exit(-1);
        } else {
            int r;
            char buf[1];
            do {
                r = read(fd, buf, 1);
                if (r > 0) {
                    bytes += r;
                    if (buf[0] == '\n') {
                        newlines++;
                	words++;
                	last_white = 1;
                    } else if ((buf[0] == ' ') || (buf[0] == '\t')) {
                	words++;
                	last_white = 1;
                    } else {
                    	last_white = 0;
                    }
                }
            } while (r > 0);
            close(fd);
        }
        if (!last_white) {
        	words++;
        	newlines++;
        }
    	printf("%d %d %d %s\r\n", newlines, words, bytes, args[i]);
    	i++;
    }
    exit(0);
}
