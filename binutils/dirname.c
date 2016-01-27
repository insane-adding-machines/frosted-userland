
#include "frosted_binutils.h"

int main(int argc, char *args)
    int argc = 0, i, c;
    extern int optind;
    char delim = '\n';
    char *head, *tail;
    char **flags;
    argc = nargs( args);

    setlocale (LC_ALL, "");

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
