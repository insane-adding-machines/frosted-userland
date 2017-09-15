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
 *      Authors: Maldus512
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSIZE 256
#define MAXFILES 13

int inputline(char *input, int size)
{
    int len;
    while (1 < 2) {
        len = 0;
        int out = 1;
        int i;
        memset(input, 0, size);
        while (len < size) {
            const char del = 0x08;
            int ret = read(0, input + len, 4);
            /*if ( ret > 3 )
                continue;*/
            if ((ret > 0) && (input[len] >= 0x20 && input[len] <= 0x7e)) {
                for (i = 0; i < ret; i++) {
                    /* Echo to terminal */
                    if (input[len + i] >= 0x20 && input[len + i] <= 0x7e)
                        write(out, &input[len + i], 1);

                    len++;
                }
            } else if (input[len] == 0x0D) {
                input[len + 1] = '\n';
                input[len + 2] = '\0';
                printf("\r\n");
                return len + 2;
            } else if (input[len] == 0x4) {
                if (len != 0)
                    input[len] = '\0';

                return len;
            }
            /* backspace */
            else if (input[len] == 0x7F || input[len] == 0x08) {
                if (len > 0) {
                    write(out, &del, 1);
                    printf(" ");
                    write(out, &del, 1);
                    len--;
                }
            }
        }
        printf("\r\n");
        if (len < 0)
            return -1;

        input[len + 1] = '\0';
    }
    return len;
}


int parse_interval(char *arg, int *start, int *end)
{
    char *endptr;
    if (*arg == '-' || *arg == ',') {
        *start = 0;
        endptr = arg;
    } else {
        *start = strtol(arg, &endptr, 10);
        if (*start <= 0 && arg != endptr)
            return 1;

        if (*endptr != '-' && *endptr != ',' && *endptr != '\0')
            return 1;
    }

    if (*endptr == '\0') {
        *end = *start;
        return 0;
    } else
        endptr++;

    if (*endptr == '\0')
        *end = 0;
    else {
        *end = strtol(endptr, &endptr, 10);
        if (*end == 0)
            return 1;
    }

    return 0;
}

/*returns   1 if it reaches a newline,
            0 if a delimiter is found,
            -1 if the file is finished*/
int readuntil(int fd, char **line, char delim)
{
    int b, len;
    len = -1;
    do {
        len++;
        b = read(fd, (*line) + len, 1);
    } while ((*line)[len] != delim && (*line)[len] != '\n' && b > 0);
    (*line)[len + 1] = '\0';
    if (b <= 0)
        return -1;

    if ((*line)[len] == delim)
        return 0;

    if ((*line)[len] == '\n')
        return 1;
}

#ifndef APP_CUT_MODULE
int main(int argc, char *args[])
#else
int icebox_cut(int argc, char *args[])
#endif
{
    extern int opterr, optind;
    extern char *optarg;
    char *endptr, *line;
    int c, start, end, i, j, len, flags, slot;
    int b = 0;
    int mode = O_RDONLY;
    opterr = 0;
    optind = 0;
    j = 0;
    char delim = '\t';
    int fdfn[MAXFILES];
    while ((c = getopt(argc, (char **)args, "d:c:f:")) != -1) {
        switch (c) {
        case 'f':
        case 'c':
            if (b == 1) {
                fprintf(stderr, "cut: only one type of list may be specified\r\n");
                exit(1);
            }

            b = 1;
            flags = c;
            if (parse_interval(optarg, &start, &end) != 0) {
                fprintf(stderr, "cut: invalid interval\r\n");
                exit(1);
            }

            break;
        case 'd':
            len = strlen(optarg);
            if (len <= 0) {
                fprintf(stderr, "cut: please specify a valid delimiter\r\n");
                exit(1);
            }

            if ((optarg[0] == '"' && optarg[2] == '"') || (optarg[0] == '\'' && optarg[2] == '\''))
                delim = optarg[1];
            else
                delim = optarg[0];

            break;
        default:
            fprintf(stderr, "cut: invalid option -- '%c'\r\n", optopt);
            exit(1);
        }
    }
    if (b == 0) {
        fprintf(stderr, "cut: you must specify at list of characters\r\n");
        exit(1);
    }

    if (delim != '\t' && flags != 'f') {
        fprintf(stderr, "cut: an input delimiter may be specified only when operating on fields\r\n");
        exit(1);
    }

    if (--start < 0)
        start = 0;

    end--;
    line = (char *) malloc(sizeof(char) * BUFSIZE);
    if (args[optind]) {
        slot = 0;
        for (i = optind; i < argc; i++) {
            if (i > MAXFILES)
                fprintf(stderr, "cut: Maximum of %d output files exceeded\r\n", MAXFILES);

            if ((fdfn[slot] = open(args[i], mode, 0666)) == -1)
                fprintf(stderr, "error opening %s\r\n", args[i]);
            else
                slot++;
        }
        if (flags == 'c') {
            len = (end > 0) ? end - start : BUFSIZE - start;
            for (i = 0; i < slot; i++) {
                do {
                    for (j = 0; j < start; j++) {
                        b = read(fdfn[i], line + j, 1);
                        if (b <= 0 || line[j] == '\n')
                            break;
                    }
                    if (b <= 0 || j != start) {
                        printf("\r\n");
                        continue;
                    }

                    for (j = 0; j <= len; j++) {
                        b = read(fdfn[i], line + j, 1);
                        if (b <= 0 || line[j] == '\n') {
                            j++;
                            break;
                        }
                    }
                    if (b <= 0)
                        break;

                    line[j] = '\0';
                    if (line[j - 1] != '\n')
                        printf("%s\r\n", line);
                    else
                        printf("%s", line);

                    line[0] = line[j - 1];
                    while (line[0] != '\n')
                        if (read(fdfn[i], line, 1) <= 0)
                            break;

                } while (b > 0);
            }
        } else if (flags == 'f') {
            for (i = 0; i < slot; i++) {
                while (b >= 0) {
                    if (start == 0)
                        b = 0;

                    for (j = 0; j < start; j++) {
                        if ((b = readuntil(fdfn[i], &line, delim)) != 0)
                            break;
                    }
                    if (b < 0)
                        break;
                    else if (b == 1) {
                        printf("%s", line);
                        continue;
                    }

                    /*eventually the for will break if end < 0*/
                    for (; end < 0  || j <= end; j++) {
                        b = readuntil(fdfn[i], &line, delim);
                        if (b != 0) {
                            if (b == 1)
                                printf("%s", line);

                            break;
                        }

                        if (j == end && end > 0)
                            line[ strlen(line) - 1 ] = '\0';

                        printf("%s", line);
                    }
                    if (b < 0)
                        break;
                    else if (b == 0) {
                        write(1, "\r\n", 2);
                        readuntil(fdfn[i], &line, '\n');
                    }
                }
            }
        }
    } else {
        /*stdin*/
        while (inputline(line, BUFSIZE) != 0) {
            if (flags == 'c') {
                len = strlen(line) - 1;
                len = (end < len && end >= 0) ? end : len - 1;
                for (i = start; i <= len; i++)
                    write(1, &line[i], 1);
                write(1, "\r\n", 2);
            } else if (flags == 'f') {
                len = strlen(line);
                j = 0;
                for (i = 0; i < start; i++) {
                    while (line[j] != delim && j < len) j++;
                    if (j < len) j++;
                }
                if (j == len) {
                    printf("%s", line);
                    continue;
                }

                for (i = start; end < 0 || i <= end; i++) {
                    while (line[j] != delim && j < len)
                        write(1, &line[j++], 1);
                    if (line[j] == delim && (i < end || end < 0))
                        write(1, &line[j++], 1);

                    if (j == len)
                        break;
                }
                if (line[j - 1] != '\n')
                    write(1, "\r\n", 2);
            }
        }
    }

    exit(0);
}
