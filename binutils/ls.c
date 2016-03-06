#include "frosted_binutils.h"
#include <dirent.h>

int main(int argc, char *args[])
{
    char *fname;
    char *fname_start;
    struct dirent *ep, *result;
    DIR *d;
    struct stat st;
    char type;
    int i;
    char ch_size[8] = "";

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

        if (stat(fname, &st) == 0) {
            printf(fname);
            printf( "\t");
            if (S_ISDIR(st.st_mode)) {
                type = 'd';
            } else if (S_ISLNK(st.st_mode)) {
                type = 'l';
            } else {
                snprintf(ch_size, 8, "%lu", st.st_size);
                type = 'f';
            }

            printf( "%c", type);
            printf( "    ");
            printf( ch_size);
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
