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
#define NDEFAULT	10

#define IN		0
#define OUT		1

#define START		0
#define END		1

struct wc {
	long lines;
	long words;
	long bytes;
};

struct ht {
	int type;
	int number;
	int side;
	long start;
	long end;
};

/* Parse our arguments */
static int parse_opts(int argc, char *args[], struct ht *ht)
{
	int count = 0;
	int c;

	if( argc < 2 ){
		fprintf(stderr, "Usage: head [OPTION]... [FILE]\r\n");
		exit(1);
	}

	while ((c = getopt(argc, args, "tn:c:")) != -1) {
		switch (c) {
			case 'n':
			ht->number = (strtol(optarg, NULL, 10) - 1);
			ht->type = LINES;
			count++;
			break;
			case 'c':
			ht->number = (strtol(optarg, NULL, 10) - 1);
			ht->type = BYTES;
			count++;
			break;
			default:
			fprintf(stderr, "wc: invalid option -'%c'\r\n", c);
			break;
		}
	}

	return count;
}

/* Word Count a file */
int __attribute__((weak)) word_count(int fd, struct wc *wc)
{
	int r;
	char c;
	int state = OUT;
	wc->lines = wc->words = wc->bytes = 0;

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

/* If nothing specified, set our default type */
static int set_default(struct ht *ht)
{
	if (!ht->type) {
		ht->type = LINES;
		ht->number = NDEFAULT;
	}

	return 0;
}

/* Set start/end of head/tail */
static int set_offset(struct ht *ht, struct wc *wc)
{
	if (ht->side) {
		if (ht->type == BYTES) {
			ht->start = wc->bytes - ht->number - 1;
			ht->end = wc->bytes;
		} else {
			ht->start = wc->lines - ht->number - 1;
			ht->end = wc->lines;
		}
	} else {
		ht->end = ht->number;
	}

	return 0;
}

/* Print part of a file */
static int ouroboros(int fd, struct ht *ht)
{
	long count = 0;
	int r;
	char c;

	while ((r = read(fd, &c, 1)) > 0) {
		if ((count >= ht->start) && (count <= ht->end))  {
			write(STDOUT_FILENO, &c, r);
		}
		if ((ht->type == BYTES) || ((c == '\n') && (ht->type == LINES))) {
			count++;
		}
	}

	return 0;
}

/* main: parse args, loop over FILES, count and print results */
#ifndef APP_TAIL_MODULE
int main(int argc, char *args[])
#else
int icebox_tail(int argc, char *args[])
#endif
{
	int i = 1;
	int fd;
	struct ht ht = {0, 0, 0, 0, 0};

	i += parse_opts(argc, args, &ht);
	ht.side = END;
	/* if no options specified, fallback to default */
	set_default(&ht);

	while (args[i]) {
		fd = open(args[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "File %s not found.\r\n", args[i]);
			i++;
			continue;
		}
		struct wc wc = {0, 0, 0};
		word_count(fd, &wc);

		close(fd);
		fd = open(args[i], O_RDONLY);

		set_offset(&ht, &wc);

		ouroboros(fd, &ht);

		close(fd);
		i++;
	}

	exit(0);
}
