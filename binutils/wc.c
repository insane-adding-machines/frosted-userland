#include "frosted_binutils.h"


int parse_opts(int argc, char *args[], int *flags) {
	int count = 0;
	int c;

	if( argc < 2 ){
		fprintf(stderr, "Usage: wc [OPTION]... [FILE]\n");
		exit(1);
	}

	*flags = 0;

	while ((c = getopt(argc, args, "lcw")) != -1) {
		switch (c) {
			case 'c':
				*flags += 0x01;
				count++;
				break;
			case 'w':
				*flags += 0x02;
				count++;
				break;
			case 'l':
				*flags += 0x04;
				count++;
				break;
			default:
				fprintf(stderr,"wc: invalid option -'%c'\r\n", c);
				exit(1);
			}
	}
	return count;
}


int main(int argc, char *args[])
{
	int fd;
	int i = 1;
	int flags;
	i += parse_opts(argc, args, &flags);

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
		if ((flags & 0x04) || !flags) {
			printf("%4d ", newlines);
		} if ((flags & 0x02)  || !flags) {
			printf("%4d ", words);
		} if ((flags &  0x01) || !flags) {
			printf("%4d ", bytes);
		}
		printf(" %s\r\n", args[i]);
		i++;
	}
	exit(0);
}
