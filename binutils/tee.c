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
 *      Authors:
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

#ifndef APP_TEE_MODULE
int main(int argc, char *args[])
#else
int icebox_tee(int argc, char *args[])
#endif
{
    extern int opterr, optind, optopt;
    int c, i, written, j = 0,  b = 0;
    char line[BUFSIZE];
    int slot, fdfn[MAXFILES + 1][2] ;
    ssize_t n, count;
    int mode = O_WRONLY | O_CREAT | O_TRUNC;
    setlocale(LC_ALL, "");
    opterr = 0;
    optind = 0;
    fdfn[j][0] = 1;
    while ((c = getopt(argc,(char**) args, "ai")) != -1)
        switch (c)
        {
        case 'a':   /* append to rather than overwrite file(s) */
            mode = O_WRONLY | O_CREAT | O_APPEND;
            break;
        case 'i':   /* ignore the SIGINT signal */
            signal(SIGINT, SIG_IGN);
            break;
        case 0:
            opterr = 0;
            break;
        default:
            fprintf(stderr, "tee: invalid option -- '%c'\n", c,optopt);
            exit(1);
        }
    /*setting extra stdout redirections, getopt does not count them*/
    for( i = 0; i < optind ; i++ ){
        if( strcmp( args[i] , "-" ) == 0 )
            fdfn[++j][0] = 1;
    }

    for (slot = j+1; i < argc ; i++)
    {
        if (slot > MAXFILES)
            fprintf(stderr, "tee: Maximum of %d output files exceeded\n", MAXFILES);
        else
        {
            if ((fdfn[slot][0] = open(args[i], mode, 0666)) == -1)
                fprintf(stderr,"error opening %s\n", args[i]);
            else
                fdfn[slot++][1] = i;
        }
    }
    i = 0;
    while( inputline(line, BUFSIZE) != 0 ){
        for( i = 0; i < slot; i++ ){
            count = strlen( line );
            while( count > 0 ){
                written = write( fdfn[i][0], line, count);
                count -= written;
            }
        }
    }

    if (n < 0)
        printf("error\n");

    for (i = 1; i < slot; i++)
        if (close(fdfn[i][0]) == -1)
            printf("error\n");
    optind = 0;
    exit(0);
}
