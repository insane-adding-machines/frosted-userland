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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LINES		1
#define WORDS		2
#define BYTES		4

#define IN		0
#define OUT		1

struct wc {
	long lines;
	long words;
	long bytes;
};

/* Parse our arguments */
static int parse_opts(int argc, char *args[], int *flags)
{
	int count = 0;
	int c;

	if( argc < 2 ){
		fprintf(stderr, "Usage: wc [OPTION]... [FILE]\r\n");
		exit(1);
	}

	*flags = 0;

	while ((c = getopt(argc, args, "lwc")) != -1) {
		switch (c) {
			case 'l':
				*flags += LINES;
				count++;
				break;
			case 'w':
				*flags += WORDS;
				count++;
				break;
			case 'c':
				*flags += BYTES;
				count++;
				break;
			default:
				//fprintf(stderr, "wc: invalid option -'%c'\r\n", c);
				break;
			}
	}
	return count;
}

/* Word Count a file */
int wc_count(int fd, struct wc *wc)
{
	int r;
	char c;
	int state = OUT;

	while ((r = read(fd, &c, 1)) > 0) {
		if ((c == EOF)) {
			break;
		}
		wc->bytes++;
		if (c == '\n') {
			wc->lines++;
		}
		if ((c == ' ') || (c == '\t') || (c == '\n')) {
			state = OUT;
		} else if (state == OUT) {
			wc->words++;
			state = IN;
		}
	}

	if (state == IN) {
		wc->lines++;
	}

	return 0;
}

/* Print our selected wc outputs */
int wc_print(char *file, struct wc *wc, int flags)
{
	if ((flags & LINES) || !flags) {
		printf("%ld ", wc->lines);
	} if ((flags & WORDS)  || !flags) {
		printf("%ld ", wc->words);
	} if ((flags &  BYTES) || !flags) {
		printf("%ld ", wc->bytes);
	}
	printf(" %s\r\n", file);

	return 0;
}

/* main: parse args, loop over FILES, count and print results */
#ifndef APP_WC_MODULE
int main(int argc, char *args[])
#else
int icebox_wc(int argc, char *args[])
#endif
{
	int i = 1;
	int flags;
	int fd;

	i += parse_opts(argc, args, &flags);

	while (args[i]) {
		struct wc wc = {0, 0, 0};
		if (!strncmp(args[i], "-", 1)) {
			printf("trying stdin...\r\n");
			fd = STDIN_FILENO;
		} else {
			fd = open(args[i], O_RDONLY);
		}

		if (fd < 0) {
			fprintf(stderr, "File %s not found.\r\n", args[i]);
			i++;
			continue;
		}

		if (!wc_count(fd, &wc)) {
			wc_print(args[i], &wc, flags);
		}
		close(fd);
		i++;
	}

	exit(0);
}
