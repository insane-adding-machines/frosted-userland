#include "frosted_binutils.h"


int main(int argc, char *args[])
    extern int opterr, optind;
    extern char* optarg;
    char *endptr, *line;
    int c, start, end, i, j, len, flags, slot, argc;
    int b = 0;
    int mode = O_RDONLY;
    opterr = 0;
    optind = 0;
    j = 0;
    char delim = '\t';
    argc = nargs(args);
    int fdfn[MAXFILES];
    while( (c = getopt( argc, (char**)args, "d:c:f:" )) != -1){
        switch (c){
            case 'f':
            case 'c':
                if( b == 1 ){
                    fprintf(stderr,"cut: only one type of list may be specified\n");
                    exit(1);
                }
                b = 1;
                flags = c;
                if( parse_interval(optarg, &start, &end)!=0){
                    fprintf(stderr,"cut: invalid interval\n");
                    exit(1);
                }
                break;
            case 'd':
                len = strlen( optarg );
                if( len <= 0 ){
                    fprintf(stderr, "cut: please specify a valid delimiter\n");
                    exit(1);
                }
                if( ( optarg[0] == '"' && optarg[2] == '"' ) || ( optarg[0] == '\'' && optarg[2] == '\'' ) )
                    delim = optarg[1];
                else
                    delim = optarg[0];
                break;
            default:
                fprintf(stderr,"cut: invalid option -- '%c'\n", optopt );
                exit(1);
        }
    }
    if( b == 0 ){
        fprintf(stderr,"cut: you must specify at list of characters\n");
        exit(1);
    }
    if( delim != '\t' && flags != 'f' ){
        fprintf(stderr, "cut: an input delimiter may be specified only when operating on fields\n");
        exit(1);
    }
    if(--start < 0 )
        start = 0;
    end--;
    line = (char*) malloc( sizeof(char)*BUFSIZE );
    if( args[optind] ){
        slot = 0;
        for( i = optind; i < argc; i++ ){
            if( i > MAXFILES )
                fprintf(stderr, "cut: Maximum of %d output files exceeded\n", MAXFILES);
            if(( fdfn[slot] = open( args[i], mode, 0666)) == -1 )
                fprintf(stderr, "error opening %s\n", args[i] );
            else
                slot++;
        }
        if( flags == 'c' ){
            len = (end > 0) ? end - start : BUFSIZE - start;
            for( i = 0; i < slot; i++ ){
                do{
                    for( j = 0; j < start; j++ ){
                        b = read(fdfn[i], line + j, 1 );
                        if( b <= 0 || line[j] == '\n')
                            break;
                    }
                    if( b <= 0 || j != start ){
                        printf("\n");
                        continue;
                    }
                    for( j = 0; j <= len; j++ ){
                        b = read( fdfn[i], line + j, 1 );
                        if( b <= 0 || line[j] == '\n' ){
                            j++;
                            break;
                        }
                    }
                    if( b <= 0 )
                        break;
                    line[j] = '\0';
                    if( line[j-1] != '\n' )
                        printf( "%s\n", line);
                    else
                        printf( "%s", line );
                    line[0] = line[j-1];
                    while( line[0] != '\n' )
                        if( read( fdfn[i], line, 1 ) <= 0 )
                            break;
                }while( b > 0 );
            }
        }
        else if( flags == 'f' ){
            for( i = 0; i < slot; i++ ){
                while( b >= 0 ){
                    if( start == 0 )
                        b = 0;
                    for( j = 0; j < start ; j++ ){
                        if ((b = readuntil( fdfn[i], &line, delim ) ) != 0)
                            break;
                    }
                    if( b < 0 )
                        break;
                    else if( b == 1 ){
                        printf("%s", line);
                        continue;
                    }
                    /*eventually the for will break if end < 0*/
                    for( ;  end < 0  || j <= end ; j++){
                        b = readuntil( fdfn[i], &line, delim );
                        if( b != 0 ){
                            if( b == 1 )
                                printf("%s", line);
                            break;
                        }
                        if( j == end && end > 0 )
                            line[ strlen(line) - 1 ] = '\0';
                        printf( "%s", line );
                    }
                    if( b < 0 )
                        break;
                    else if( b == 0 ){
                        write( STDOUT_FILENO, "\r\n", 2 );
                        readuntil( fdfn[i], &line, '\n' );
                    }
                }
            }
        }
    }
    else{
        /*stdin*/
        while( inputline(line, BUFSIZE) != 0 ){
            if( flags == 'c' ){
                len = strlen(line)-1;
                len = (end < len && end >= 0) ? end : len - 1;
                for( i = start; i <= len; i++ )
                    write( STDOUT_FILENO, &line[i], 1 );
                write( STDOUT_FILENO, "\r\n", 2 );
            }
            else if( flags == 'f' ){
                len = strlen(line);
                j = 0;
                for( i = 0; i < start ; i++){
                    while( line[j] != delim && j < len ) j++;
                    if( j < len ) j++;
                }
                if( j == len ){
                    printf("%s", line );
                    continue;
                }
                for( i = start; end < 0 || i <= end; i++){
                    while( line[j] != delim && j < len )
                        write( STDOUT_FILENO, &line[j++], 1 );
                    if( line[j] == delim && (i < end || end < 0) )
                        write( STDOUT_FILENO, &line[j++], 1 );
                    if( j == len )
                        break;
                }
                if( line[j-1] != '\n')
                    write( STDOUT_FILENO, "\r\n", 2 );
            }
        }
    }
    exit(0);
}
