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
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#ifndef APP_DIRNAME_MODULE
int main(int argc, char *args[])
#else
int icebox_dirname(int argc, char *args[])
#endif
{
    int i, c;
    char delim = '\n';
    char *head, *tail;
    char **flags;

    if ( argc < 2 || args[1] == NULL){
        fprintf(stderr, "usage: dirname [OPTION] NAME...\n");
        exit(1);
    }

    while( (c=getopt(argc,(char**) args, "r") ) != -1 ){
        switch (c){
            case 'r':   /*use NUL char as delimiter instead of \n*/
                delim = (char) 0;
                break;
            default:
                fprintf( stderr, "dirname: invalid option -- '%c'\n", (char)c);
                exit(1);
        }
    }

    i = optind;
    while( args[i] != NULL ){
        head = tail = args[i];
        while (*tail)
            tail++;

    /* removing the last part of the path*/
        while (tail > head && tail[-1] == '/')
            tail--;
        while (tail > head && tail[-1] != '/')
            tail--;
        while (tail > head && tail[-1] == '/')
            tail--;

        if (head == tail)
            printf(*head == '/' ? "/%c" : ".%c", delim);
        else{
            *tail = '\0';
       // printf("%.*s\n", (tail - head), head);
            printf("%s%c", head, delim);
        }
        i++;
    }
    /*resetting getopt*/
    optind = 0;
    exit(0);
}
