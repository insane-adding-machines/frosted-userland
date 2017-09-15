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
 *      Authors: Daniele Lacamera
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#define ALLFLAG (1<<0)
#define HUMANFLAG (1<<1)
#define LONGFLAG (1<<2)

char mag[] = " KMGT";

#ifndef APP_LS_MODULE
int main(int argc, char *args[])
#else
int icebox_ls(int argc, char *args[])
#endif
{
    char *fname;
    char *fname_start;
    struct dirent *ep, *result;
    DIR *d;
    struct stat st;
    char type;
    int i;
    char ch_size[70] = "";
    unsigned int flags;
    int c;
    extern int optind, optopt;
    extern char *optarg;


    if (argc < 1) {
        fprintf(stderr, "Usage: ls [OPTION]... [FILE]...\n");
        exit(1);
    }

    flags = 0;
    while ((c = getopt(argc, (char **)args, "ahl")) != -1) {
        switch (c) {
        case 'a':
            flags |= ALLFLAG;
            break;
        case 'h':
            flags |= HUMANFLAG;
            break;
        case 'l':
            flags |= LONGFLAG;
            break;
        default:
            fprintf(stderr, "ls: invalid option -- '%c'\n", optopt);
            exit(1);
        }
    }

    fname_start = malloc(MAX_FILE);
    ep = malloc(sizeof(struct dirent));
    if (!ep || !fname_start)
        exit(1);;

    getcwd(fname_start, MAX_FILE);

    d = opendir(fname_start);
    if (!d) {
        fprintf(stderr, "Error opening %s\r\n", fname_start);
        exit(2);
    }
    while(readdir_r(d, ep, &result) == 0) {
        fname = fname_start;
        fname[0] = '\0';
        strncat(fname, fname_start, MAX_FILE);
        strncat(fname, "/", MAX_FILE);
        strncat(fname, ep->d_name, MAX_FILE);

        while (fname[0] == '/')
            fname++;

        if (lstat(fname, &st) == 0) {
            if ((!strncmp(fname, ".", 1) || !strncmp(fname, "..", 2)) && !(flags & ALLFLAG)) {
                continue;
            }
            printf(fname);
            if (flags & LONGFLAG) {
                printf(" ");
                printf(" ");
                for (i = strlen(fname); i < 30; i++)
                    printf(" ");
                if (S_ISDIR(st.st_mode)) {
                    type = 'd';
                } else if (S_ISLNK(st.st_mode)) {
                    char tgt[60];
                    type = 'l';
                    if (readlink(fname, tgt, 60) < 0)
                        sprintf(ch_size, "-->BROKEN LNK<--");
                    else
                        sprintf(ch_size, "-->%s", tgt);
                } else {
                    unsigned long size = st.st_size;;
                    int order = 0;
                    char magn;
                    if (flags & HUMANFLAG) {
                        while (size > 1000) {
                            size /= 1000;
                            order++;
                        }
                        magn = mag[order];
                    }

                    snprintf(ch_size, 9, "%lu%c", size, magn);
                    type = 'f';
                }

                printf( "%c", type);
                printf( " ");
                printf( ch_size);
            }
            printf( "\r\n");
        } else {
            fprintf(stderr, "stat error on %s: %s.\r\n", fname, strerror(errno));
        }
    }
    closedir(d);
    free(ep);
    free(fname_start);
    exit(0);
}
