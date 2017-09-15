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
#include <regex.h>

#define BUFSIZE 256
#define MAXFILES 13

#define CFLAG 0x1
#define XFLAG 0x2
#define IFLAG 0x4


struct file{
    int fd;
    char* name;
};


struct regstruct{
    regex_t r;
    char* string;
};


static int inputline(char *input, int size)
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


/*returns   1 if it reaches a newline,
            0 if a delimiter is found,
            -1 if the file is finished*/
static int readuntil(int fd, char **line, char delim)
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


static int nextline(char *str, int from)
{
    int l, last;
    l = from;
    last = strlen(str);
    while (l < last && str[l] != '\n') l++;
    return l;
}


static int striplen(char *str)
{
    int l;
    l = strlen(str);
    if (str[l - 1] == '\n')
        l--;

    if (str[l - 1] == '\r')
        l--;

    return l;
}

static void strtolow(char *str)
{
    char *p;
    for (p = str; *p != '\0'; p++)
        *p = *p >= 'A' && *p <= 'Z' ? *p + 32 : *p;
}


/*search for exact occurrence of str1 in str2*/
static int occurrence(char *tofind, char *line, int flags)
{
    int i, j, l1, l2, c, start, end;
    char *str1, *str2;
    l2 = striplen(line);
    end = strlen(tofind);
    if (flags & IFLAG) {
        str1 = (char *) malloc(sizeof(char) * (end + 1));
        str2 = (char *) malloc(sizeof(char) * (l2 + 1));
        strcpy(str1, tofind);
        strcpy(str2, line);
        str2[l2] = '\0';
        strtolow(str1);
        strtolow(str2);
    } else {
        str1 = tofind;
        str2 = line;
    }

    l1 = -1;
    do {
        start = l1 + 1;
        l1 = nextline(str1, start);
        for (i = 0; i < l2; i++) {
            c = 0;
            for (j = start; j < l1 && i + c < l2; j++) {
                if (str1[j] != str2[i + c])
                    break;
                else
                    c++;
            }
            if (j == l1) {
                if (!(flags & XFLAG))
                    return 0;
                else if (l1 - start == l2)
                    return 0;
                else
                    break;
            }
        }
    } while (l1 != end);
    return 1;
}

#ifndef APP_GREP_MODULE
int main(int argc, char *args[])
#else
int icebox_grep(int argc, char* args[])
#endif
{
    char *line;
    char *fixedp[MAXFILES];
    size_t t;
    regex_t regex;
    extern int optind, optopt;
    extern char *optarg;
    int reti, i, j, b, slot, c, p, tmp;
    int flags, options, match, fixed;
    struct file fdfn[MAXFILES];
    struct regstruct patterns[MAXFILES];
    if (argc < 2) {
        fprintf(stderr, "Usage: grep [OPTION]... PATTERN [FILE]...\n");
        exit(1);
    }

    line = (char *) malloc(sizeof(char) * BUFSIZE);
    flags = i = b = p = match = optind = fixed = 0;
    while ((c = getopt(argc, (char **)args, "EGFe:f:ivcx")) != -1) {
        switch (c) {
        case 'i':
            flags |= REG_ICASE;
            options |= IFLAG;
            break;
        case 'v':
            match = REG_NOMATCH;
            break;
        case 'f':
            if ((tmp = open(optarg, O_RDONLY)) == -1)
                fprintf(stderr, "grep: error opening %s\n", optarg);
            else {
                while (b == 0) {
                    patterns[p].string = (char *)malloc(sizeof(char) * BUFSIZE);
                    b = readuntil(tmp, &patterns[p].string, '\n');
                    if (b == -1)
                        free(patterns[p].string);
                    else {
                        /*remove trailing newline*/
                        patterns[p].string[ strlen(patterns[p].string) - 1] = '\0';
                        p++;
                    }
                }
            }

            break;
        case 'e':
            patterns[p].string = malloc(sizeof(char) * (strlen(optarg) + 1));
            strcpy(patterns[p].string, optarg);
            p++;
            break;
        case 'c':
            options |= CFLAG;
            break;
        case 'x':
            options |= XFLAG;
            break;
        case 'E':
            flags |= REG_EXTENDED;
            break;
        case 'G':
            flags &= ~REG_EXTENDED;
            break;
        case 'F':
            fixed = 1;
            break;
        default:
            fprintf(stderr, "grep: invalid option -- '%c'\n", optopt);
            exit(1);
        }
    }
    if (p == 0 && optind < argc) {
        patterns[p].string = (char *)malloc(sizeof(char) * (strlen(args[optind]) + 1));
        strcpy(patterns[p].string, args[optind]);
        p++;
        optind++;
    }

    if (!fixed)
        for (i = 0; i < p; i++)
            reti = regcomp(&patterns[i].r, patterns[i].string, flags);
    i = argc;
    j = optind;
    c = 0;
    for (slot = 0; j < i; j++) {
        if ((fdfn[slot].fd = open(args[j], O_RDONLY)) == -1) {
            perror("grep: ");
            fprintf(stderr, "grep: error opening %s\n", fdfn[slot].name);
        } else {
            fdfn[slot].name = (char *)malloc(sizeof(char) * (strlen(args[j]) + 1));
            strcpy(fdfn[slot].name, args[j]);
            slot++;
        }
    }
    if (slot == 0) {
        while (inputline(line, BUFSIZE) != 0) {
            for (j = 0; j < p; j++) {
                if (fixed)
                    reti = occurrence(patterns[j].string, line, options);
                else {
                    if ((options & XFLAG) && (striplen(line) != strlen(patterns[j].string)))
                        continue;

                    reti = regexec(&patterns[j].r, line, 0, NULL, 0);
                }

                if (reti == match) {
                    if (options & CFLAG)
                        c++;
                    else
                        printf("%s", line);

                    break;
                }
            }
        }
        if (options & CFLAG)
            printf("%i\n", c);
    } else {
        for (i = 0; i < slot; i++) {
            while ((b = readuntil(fdfn[i].fd, &line, '\n')) != -1) {
                for (j = 0; j < p; j++) {
                    if (fixed)
                        reti = occurrence(patterns[j].string, line, options);
                    else {
                        if ((options & XFLAG) && (striplen(line) != strlen(patterns[j].string)))
                            continue;

                        reti = regexec(&patterns[j].r, line, 0, NULL, 0);
                    }

                    if (reti == match) {
                        if (options & CFLAG)
                            c++;
                        else
                            printf("%s:%s", fdfn[i].name, line);

                        break;
                    }
                }
            }
            if (options & CFLAG)
                printf("%s:%i\n", fdfn[i].name, c);
        }
    }

    exit(0);
}
