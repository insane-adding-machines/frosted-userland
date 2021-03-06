/* echoserver.c
 *
 * Copyright (C) 2014-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSH.
 *
 * wolfSSH is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolfSSH.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
/* Frosted random number generator via /dev/random */
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssh/ssh.h>
#ifndef SO_NOSIGPIPE
    #include <signal.h>
#endif

#define SCRATCH_BUFFER_SIZE     1200
static uint8_t proc_buf[SCRATCH_BUFFER_SIZE];
char FRESH_BIN[] = "/bin/fresh";
char * const fresh_args[2] = {FRESH_BIN, NULL};




static const char echoserverBanner[] = "frosted SSH daemon (powered by WolfSSL)\n";

typedef int SOCKET_T;
#ifdef TEST_IPV6
    typedef struct sockaddr_in6 SOCKADDR_IN_T;
    #define AF_INET_V           AF_INET6
    static const char*          wolfsshIP = "::1";
#else
    typedef struct sockaddr_in  SOCKADDR_IN_T;
    #define AF_INET_V           AF_INET
    static const char*          wolfsshIP = "127.0.0.1";
#endif
#define SERVER_PORT_NUMBER      22

#if defined(__MACH__) || defined(USE_WINDOWS_API)
    #ifndef _SOCKLEN_T
        typedef int socklen_t;
    #endif
#endif
/* HPUX doesn't use socklent_t for third parameter to accept, unless
   _XOPEN_SOURCE_EXTENDED is defined */
#if !defined(__hpux__) && !defined(CYASSL_MDK_ARM) && !defined(CYASSL_IAR_ARM)
    typedef socklen_t SOCKLEN_T;
#else
    #if defined _XOPEN_SOURCE_EXTENDED
        typedef socklen_t SOCKLEN_T;
    #else
        typedef int       SOCKLEN_T;
    #endif
#endif


#if defined(_POSIX_THREADS) && !defined(__MINGW32__)
    typedef void*         THREAD_RETURN;
    typedef pthread_t     THREAD_TYPE;
    #define CYASSL_THREAD
    #define INFINITE -1
    #define WAIT_OBJECT_0 0L
#elif defined(CYASSL_MDK_ARM)
    typedef unsigned int  THREAD_RETURN;
    typedef int           THREAD_TYPE;
    #define CYASSL_THREAD
#else
    typedef unsigned int  THREAD_RETURN;
    typedef intptr_t      THREAD_TYPE;
    #define CYASSL_THREAD __stdcall
#endif

typedef struct {
    WOLFSSH* ssh;
    SOCKET_T fd;
    uint32_t id;
} thread_ctx_t;


#ifndef EXAMPLE_HIGHWATER_MARK
    #define EXAMPLE_HIGHWATER_MARK 0x3FFF8000 /* 1GB - 32kB */
#endif
#ifndef EXAMPLE_BUFFER_SZ
    #define EXAMPLE_BUFFER_SZ 4096
#endif


#ifdef __GNUC__
    #define WS_NORETURN __attribute__((noreturn))
#else
    #define WS_NORETURN
#endif


#define MY_EX_USAGE 2

extern int   myoptind;
extern char* myoptarg;

static INLINE int mygetopt(int argc, char** argv, const char* optstring)
{
    static char* next = NULL;

    char  c;
    char* cp;

    if (myoptind == 0)
        next = NULL;   /* we're starting new/over */

    if (next == NULL || *next == '\0') {
        if (myoptind == 0)
            myoptind++;

        if (myoptind >= argc || argv[myoptind][0] != '-' ||
                                argv[myoptind][1] == '\0') {
            myoptarg = NULL;
            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        if (strcmp(argv[myoptind], "--") == 0) {
            myoptind++;
            myoptarg = NULL;

            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        next = argv[myoptind];
        next++;                  /* skip - */
        myoptind++;
    }

    c  = *next++;
    /* The C++ strchr can return a different value */
    cp = (char*)strchr(optstring, c);

    if (cp == NULL || c == ':')
        return '?';

    cp++;

    if (*cp == ':') {
        if (*next != '\0') {
            myoptarg = next;
            next     = NULL;
        }
        else if (myoptind < argc) {
            myoptarg = argv[myoptind];
            myoptind++;
        }
        else
            return '?';
    }

    return c;
}


static INLINE WS_NORETURN void err_sys(const char* msg)
{
    printf("server error: %s\n", msg);

#ifndef __GNUC__
    /* scan-build (which pretends to be gnuc) can get confused and think the
     * msg pointer can be null even when hardcoded and then it won't exit,
     * making null pointer checks above the err_sys() call useless.
     * We could just always exit() but some compilers will complain about no
     * possible return, with gcc we know the attribute to handle that with
     * WS_NORETURN. */
    if (msg)
#endif
    {
        exit(EXIT_FAILURE);
    }
}


static INLINE void build_addr(SOCKADDR_IN_T* addr, const char* peer,
                              uint16_t port)
{
    int useLookup = 0;
    (void)useLookup;

    memset(addr, 0, sizeof(SOCKADDR_IN_T));

#ifndef TEST_IPV6
    /* peer could be in human readable form */
    if ( (peer != INADDR_ANY) && isalpha((int)peer[0])) {
        #ifdef CYASSL_MDK_ARM
            int err;
            struct hostent* entry = gethostbyname(peer, &err);
        #else
            struct hostent* entry = gethostbyname(peer);
        #endif

        if (entry) {
            memcpy(&addr->sin_addr.s_addr, entry->h_addr_list[0],
                   entry->h_length);
            useLookup = 1;
        }
        else
            err_sys("no entry for host");
    }
#endif

#ifndef TEST_IPV6
    #if defined(CYASSL_MDK_ARM)
        addr->sin_family = PF_INET;
    #else
        addr->sin_family = AF_INET_V;
    #endif
    addr->sin_port = htons(port);
    if (peer == INADDR_ANY)
        addr->sin_addr.s_addr = INADDR_ANY;
    else {
        if (!useLookup)
            addr->sin_addr.s_addr = inet_addr(peer);
    }
#else
    addr->sin6_family = AF_INET_V;
    addr->sin6_port = htons(port);
    if (peer == INADDR_ANY)
        addr->sin6_addr = in6addr_any;
    else {
        #ifdef HAVE_GETADDRINFO
            struct addrinfo  hints;
            struct addrinfo* answer = NULL;
            int    ret;
            char   strPort[80];

            memset(&hints, 0, sizeof(hints));

            hints.ai_family   = AF_INET_V;
            hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;
            hints.ai_protocol = udp ? IPPROTO_UDP : IPPROTO_TCP;

            SNPRINTF(strPort, sizeof(strPort), "%d", port);
            strPort[79] = '\0';

            ret = getaddrinfo(peer, strPort, &hints, &answer);
            if (ret < 0 || answer == NULL)
                err_sys("getaddrinfo failed");

            memcpy(addr, answer->ai_addr, answer->ai_addrlen);
            freeaddrinfo(answer);
        #else
            printf("no ipv6 getaddrinfo, loopback only tests/examples\n");
            addr->sin6_addr = in6addr_loopback;
        #endif
    }
#endif
}


static INLINE void tcp_socket(SOCKET_T* sockFd)
{
    *sockFd = socket(AF_INET_V, SOCK_STREAM, 0);

#ifdef USE_WINDOWS_API
    if (*sockFd == INVALID_SOCKET)
        err_sys("socket failed\n");
#else
    if (*sockFd < 0)
        err_sys("socket failed\n");
#endif

#ifndef USE_WINDOWS_API
#ifdef SO_NOSIGPIPE
    {
        int       on = 1;
        socklen_t len = sizeof(on);
        int       res = setsockopt(*sockFd, SOL_SOCKET, SO_NOSIGPIPE, &on, len);
        if (res < 0)
            err_sys("setsockopt SO_NOSIGPIPE failed\n");
    }
#elif defined(CYASSL_MDK_ARM)
    /* nothing to define */
#else  /* no S_NOSIGPIPE */
    signal(SIGPIPE, SIG_IGN);
#endif /* S_NOSIGPIPE */

#if defined(TCP_NODELAY)
    {
        int       on = 1;
        socklen_t len = sizeof(on);
        int       res = setsockopt(*sockFd, IPPROTO_TCP, TCP_NODELAY, &on, len);
        if (res < 0)
            err_sys("setsockopt TCP_NODELAY failed\n");
    }
#endif
#endif  /* USE_WINDOWS_API */
}


static INLINE void tcp_bind(SOCKET_T* sockFd, uint16_t port, int useAnyAddr)
{
    SOCKADDR_IN_T addr;

    /* don't use INADDR_ANY by default, firewall may block, make user switch
       on */
    build_addr(&addr, (useAnyAddr ? INADDR_ANY : wolfsshIP), port);
    tcp_socket(sockFd);

#if !defined(USE_WINDOWS_API) && !defined(CYASSL_MDK_ARM)
    {
        int       res, on  = 1;
        socklen_t len = sizeof(on);
        res = setsockopt(*sockFd, SOL_SOCKET, SO_REUSEADDR, &on, len);
        if (res < 0)
            err_sys("setsockopt SO_REUSEADDR failed\n");
    }
#endif

    if (bind(*sockFd, (const struct sockaddr*)&addr, sizeof(addr)) != 0)
        err_sys("tcp bind failed");
}


static uint8_t find_char(const uint8_t* str, const uint8_t* buf, uint32_t bufSz)
{
    const uint8_t* cur;

    while (bufSz) {
        cur = str;
        while (*cur != '\0') {
            if (*cur == *buf)
                return *cur;
            cur++;
        }
        buf++;
        bufSz--;
    }

    return 0;
}


static int dump_stats(thread_ctx_t* ctx)
{
    char stats[1024];
    uint32_t statsSz;
    uint32_t txCount, rxCount, seq, peerSeq;

    wolfSSH_GetStats(ctx->ssh, &txCount, &rxCount, &seq, &peerSeq);

    sprintf(stats,
            "Statistics for Thread #%u:\r\n"
            "  txCount = %u\r\n  rxCount = %u\r\n"
            "  seq = %u\r\n  peerSeq = %u\r\n",
            ctx->id, txCount, rxCount, seq, peerSeq);
    statsSz = (uint32_t)strlen(stats);

    fprintf(stderr, "%s", stats);
    return wolfSSH_stream_send(ctx->ssh, (uint8_t*)stats, statsSz);
}

static void server_worker(WOLFSSH *ssh, int afd)
{
    int r;

    if (wolfSSH_accept(ssh) == WS_SUCCESS) {
        printf("SSH handshake successful.\r\n");
        while(1)
            sleep(1);
        int pid = vfork();
        if (pid == 0) {
            if (afd != STDIN_FILENO) {
                close(STDIN_FILENO);
                dup(afd);
            }
            if (afd != STDOUT_FILENO) {
                close(STDOUT_FILENO);
                dup(afd);
            }
            if (afd != STDERR_FILENO) {
                close(STDERR_FILENO);
                dup(afd);
            }
            execv(FRESH_BIN, fresh_args);
        }
        close(afd);
        wolfSSH_free(ssh);
    }
}


static int load_file(const char* fileName, uint8_t* buf, uint32_t bufSz)
{
    FILE* file;
    uint32_t fileSz;
    uint32_t readSz;
    struct stat st;

    if (fileName == NULL) return 0;


    if (stat(fileName, &st) != 0) {
        return 0;
    }
    fileSz = st.st_size;

    file = fopen(fileName, "rb");
    if (file == NULL) return 0;

    if (fileSz > bufSz) {
        fclose(file);
        return 0;
    }

    readSz = (uint32_t)fread(buf, 1, fileSz, file);
    if (readSz < fileSz) {
        fclose(file);
        return 0;
    }

    fclose(file);

    return fileSz;
}


static inline void c32toa(uint32_t u32, uint8_t* c)
{
    c[0] = (u32 >> 24) & 0xff;
    c[1] = (u32 >> 16) & 0xff;
    c[2] = (u32 >>  8) & 0xff;
    c[3] =  u32 & 0xff;
}


/* Map user names to passwords */
/* Use arrays for username and p. The password or public key can
 * be hashed and the hash stored here. Then I won't need the type. */
typedef struct PwMap {
    uint8_t type;
    uint8_t username[32];
    uint32_t usernameSz;
    uint8_t p[SHA256_DIGEST_SIZE];
    struct PwMap* next;
} PwMap;


typedef struct PwMapList {
    PwMap* head;
} PwMapList;


static PwMap* PwMapNew(PwMapList* list, uint8_t type, const uint8_t* username,
                       uint32_t usernameSz, const uint8_t* p, uint32_t pSz)
{
    PwMap* map;

    map = (PwMap*)malloc(sizeof(PwMap));
    if (map != NULL) {
        Sha256 sha;
        uint8_t flatSz[4];

        map->type = type;
        if (usernameSz >= sizeof(map->username))
            usernameSz = sizeof(map->username) - 1;
        memcpy(map->username, username, usernameSz + 1);
        map->username[usernameSz] = 0;
        map->usernameSz = usernameSz;

        wc_InitSha256(&sha);
        c32toa(pSz, flatSz);
        wc_Sha256Update(&sha, flatSz, sizeof(flatSz));
        wc_Sha256Update(&sha, p, pSz);
        wc_Sha256Final(&sha, map->p);

        map->next = list->head;
        list->head = map;
    }

    return map;
}


static void PwMapListDelete(PwMapList* list)
{
    if (list != NULL) {
        PwMap* head = list->head;

        while (head != NULL) {
            PwMap* cur = head;
            head = head->next;
            memset(cur, 0, sizeof(PwMap));
            free(cur);
        }
    }
}


static const char samplePasswordBuffer[] =
    "jill:upthehill\n"
    "jack:fetchapail\n";

//static const char samplePublicKeyBuffer[] = "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIO6nc16wVleuiKHLB1aYcXrDHrwdKoyvXgqGBokGIDZL dan@holocron\n";

//static const char samplePublicKeyBuffer[] = "ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBK04HI5ez7ahWkAkMo2OeVzZfD4CPi/dUzEt3zzcGUZ6BKZX4i7XyK18qnJ5b8+ZiTYyKG4G6xwt2vGuULNGs8o= gretel@holocron\n";

static const char samplePublicKeyBuffer[] = "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIO6DoT0yWxxpMoCTsxG8ah7rJ2hKlCxK5xKw1wb/wHgf gretel@holocron\n";

unsigned char server_key_ecc_der[] = {
  0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x61, 0x09, 0x99, 0x0b, 0x79,
  0xd2, 0x5f, 0x28, 0x5a, 0x0f, 0x5d, 0x15, 0xcc, 0xa1, 0x56, 0x54, 0xf9,
  0x2b, 0x39, 0x87, 0x21, 0x2d, 0xa7, 0x7d, 0x85, 0x7b, 0xb8, 0x7f, 0x38,
  0xc6, 0x6d, 0xd5, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
  0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x81, 0x13, 0xff,
  0xa4, 0x2b, 0xb7, 0x9c, 0x45, 0x74, 0x7a, 0x83, 0x4c, 0x61, 0xf3, 0x3f,
  0xad, 0x26, 0xcf, 0x22, 0xcd, 0xa9, 0xa3, 0xbc, 0xa5, 0x61, 0xb4, 0x7c,
  0xe6, 0x62, 0xd4, 0xc2, 0xf7, 0x55, 0x43, 0x9a, 0x31, 0xfb, 0x80, 0x11,
  0x20, 0xb5, 0x12, 0x4b, 0x24, 0xf5, 0x78, 0xd7, 0xfd, 0x22, 0xef, 0x46,
  0x35, 0xf0, 0x05, 0x58, 0x6b, 0x5f, 0x63, 0xc8, 0xda, 0x1b, 0xc4, 0xf5,
  0x69
};
unsigned int server_key_ecc_der_len = 121;

static int LoadPasswordBuffer(uint8_t* buf, uint32_t bufSz, PwMapList* list)
{
    char* str = (char*)buf;
    char* delimiter;
    char* username;
    char* password;

    /* Each line of passwd.txt is in the format
     *     username:password\n
     * This function modifies the passed-in buffer. */

    if (list == NULL)
        return -1;

    if (buf == NULL || bufSz == 0)
        return 0;

    while (*str != 0) {
        delimiter = strchr(str, ':');
        username = str;
        *delimiter = 0;
        password = delimiter + 1;
        str = strchr(password, '\n');
        *str = 0;
        str++;
        if (PwMapNew(list, WOLFSSH_USERAUTH_PASSWORD,
                     (uint8_t*)username, (uint32_t)strlen(username),
                     (uint8_t*)password, (uint32_t)strlen(password)) == NULL ) {

            return -1;
        }
    }

    return 0;
}


static int LoadPublicKeyBuffer(uint8_t* buf, uint32_t bufSz, PwMapList* list)
{
    char* str = (char*)buf;
    char* delimiter;
    uint8_t* publicKey64;
    uint32_t publicKey64Sz;
    uint8_t* username;
    uint32_t usernameSz;
    uint8_t  publicKey[300];
    uint32_t publicKeySz;

    /* Each line of passwd.txt is in the format
     *     ssh-rsa AAAB3BASE64ENCODEDPUBLICKEYBLOB username\n
     * This function modifies the passed-in buffer. */
    if (list == NULL)
        return -1;

    if (buf == NULL || bufSz == 0)
        return 0;

    while (*str != 0) {
        /* Skip the public key type. This example will always be ssh-rsa. */
        delimiter = strchr(str, ' ');
        str = delimiter + 1;
        delimiter = strchr(str, ' ');
        publicKey64 = (uint8_t*)str;
        *delimiter = 0;
        publicKey64Sz = (uint32_t)(delimiter - str);
        str = delimiter + 1;
        delimiter = strchr(str, '\n');
        username = (uint8_t*)str;
        *delimiter = 0;
        usernameSz = (uint32_t)(delimiter - str);
        str = delimiter + 1;
        publicKeySz = sizeof(publicKey);

        if (Base64_Decode(publicKey64, publicKey64Sz,
                          publicKey, &publicKeySz) != 0) {

            return -1;
        }

        if (PwMapNew(list, WOLFSSH_USERAUTH_PUBLICKEY,
                     username, usernameSz,
                     publicKey, publicKeySz) == NULL ) {

            return -1;
        }
    }

    return 0;
}


static int wsUserAuth(uint8_t authType,
                      const WS_UserAuthData* authData,
                      void* ctx)
{
    PwMapList* list;
    PwMap* map;
    uint8_t authHash[SHA256_DIGEST_SIZE];

    if (ctx == NULL) {
        fprintf(stderr, "wsUserAuth: ctx not set");
        return WOLFSSH_USERAUTH_FAILURE;
    }

    if (authType != WOLFSSH_USERAUTH_PASSWORD &&
        authType != WOLFSSH_USERAUTH_PUBLICKEY) {

        return WOLFSSH_USERAUTH_FAILURE;
    }

    /* Hash the password or public key with its length. */
    {
        Sha256 sha;
        uint8_t flatSz[4];
        wc_InitSha256(&sha);
        if (authType == WOLFSSH_USERAUTH_PASSWORD) {
            c32toa(authData->sf.password.passwordSz, flatSz);
            wc_Sha256Update(&sha, flatSz, sizeof(flatSz));
            wc_Sha256Update(&sha,
                            authData->sf.password.password,
                            authData->sf.password.passwordSz);
        }
        else if (authType == WOLFSSH_USERAUTH_PUBLICKEY) {
            c32toa(authData->sf.publicKey.publicKeySz, flatSz);
            wc_Sha256Update(&sha, flatSz, sizeof(flatSz));
            wc_Sha256Update(&sha,
                            authData->sf.publicKey.publicKey,
                            authData->sf.publicKey.publicKeySz);
        }
        wc_Sha256Final(&sha, authHash);
    }

    list = (PwMapList*)ctx;
    map = list->head;

    while (map != NULL) {
        if (authData->usernameSz == map->usernameSz &&
            memcmp(authData->username, map->username, map->usernameSz) == 0) {

            if (authData->type == map->type) {
                if (memcmp(map->p, authHash, SHA256_DIGEST_SIZE) == 0) {
                    return WOLFSSH_USERAUTH_SUCCESS;
                }
                else {
                    return (authType == WOLFSSH_USERAUTH_PASSWORD ?
                            WOLFSSH_USERAUTH_INVALID_PASSWORD :
                            WOLFSSH_USERAUTH_INVALID_PUBLICKEY);
                }
            }
            else {
                return WOLFSSH_USERAUTH_INVALID_AUTHTYPE;
            }
        }
        map = map->next;
    }

    return WOLFSSH_USERAUTH_INVALID_USER;
}


static void ShowUsage(void)
{
    printf("echoserver %s\n", LIBWOLFSSH_VERSION_STRING);
    printf("-h          Help, print this usage\n");
    printf("-m          Allow multiple connections\n");
    printf("-e          Use ECC private key\n");
}


int main(int argc, char** argv)
{
    WOLFSSH_CTX* ctx = NULL;
    PwMapList pwMapList;
    SOCKET_T listenFd = 0;
    uint32_t defaultHighwater = EXAMPLE_HIGHWATER_MARK;
    uint32_t threadCount = 0;
    int multipleConnections = 0;
    int useEcc = 0;
    char ch;

    #ifdef DEBUG_WOLFSSH
        wolfSSH_Debugging_ON();
    #endif

    while ((ch = mygetopt(argc, argv, "hme")) != (char)(-1)) {
        switch (ch) {
            case 'h' :
                ShowUsage();
                exit(EXIT_SUCCESS);

            case 'm' :
                multipleConnections = 1;
                break;

            case 'e' :
                useEcc = 1;
                break;

            default:
                ShowUsage();
                exit(MY_EX_USAGE);
        }
    }
    myoptind = 0;      /* reset for test cases */

    if (wolfSSH_Init() != WS_SUCCESS) {
        fprintf(stderr, "Couldn't initialize wolfSSH.\n");
        exit(EXIT_FAILURE);
    }
    printf("WolfSSH initialized\r\n");

    ctx = wolfSSH_CTX_new(WOLFSSH_ENDPOINT_SERVER, NULL);
    if (ctx == NULL) {
        fprintf(stderr, "Couldn't allocate SSH CTX data.\n");
        exit(EXIT_FAILURE);
    }
    printf("SSL context initialized\r\n");

    memset(&pwMapList, 0, sizeof(pwMapList));
    wolfSSH_SetUserAuth(ctx, wsUserAuth);
    wolfSSH_CTX_SetBanner(ctx, echoserverBanner);

    {
        uint32_t bufSz;

        //bufSz = load_file( "/bin/key.der", buf, SCRATCH_BUFFER_SIZE);
        //if (bufSz == 0) {
        //    fprintf(stderr, "Couldn't load key file.\n");
        //    exit(EXIT_FAILURE);
        //}
        if (wolfSSH_CTX_UsePrivateKey_buffer(ctx, server_key_ecc_der, server_key_ecc_der_len , WOLFSSH_FORMAT_ASN1) < 0) {
            fprintf(stderr, "Couldn't use key buffer.\n");
            exit(EXIT_FAILURE);
        }
        printf("Private key loaded\r\n");

        bufSz = (uint32_t)strlen((char*)samplePasswordBuffer);
        memcpy(proc_buf, samplePasswordBuffer, bufSz);
        proc_buf[bufSz] = 0;
        LoadPasswordBuffer(proc_buf, bufSz, &pwMapList);
        printf("Passwords loaded\r\n");

        bufSz = (uint32_t)strlen((char*)samplePublicKeyBuffer);
        memcpy(proc_buf, samplePublicKeyBuffer, bufSz);
        proc_buf[bufSz] = 0;
        LoadPublicKeyBuffer(proc_buf, bufSz, &pwMapList);
        printf("Public keys database loaded\r\n");
    }

    tcp_bind(&listenFd, SERVER_PORT_NUMBER, 1);

    if (listen(listenFd, 5) != 0)
        err_sys("tcp listen failed");

    do {
        SOCKET_T      clientFd = 0;
        SOCKADDR_IN_T clientAddr;
        SOCKLEN_T     clientAddrSz = sizeof(clientAddr);
        THREAD_TYPE   thread;
        WOLFSSH*      ssh;
        thread_ctx_t* threadCtx;

        threadCtx = (thread_ctx_t*)malloc(sizeof(thread_ctx_t));
        if (threadCtx == NULL) {
            fprintf(stderr, "Couldn't allocate thread context data.\n");
            exit(EXIT_FAILURE);
        }
        printf("SSL client context initialized\r\n");

        ssh = wolfSSH_new(ctx);
        if (ssh == NULL) {
            fprintf(stderr, "Couldn't allocate SSH data.\n");
            exit(EXIT_FAILURE);
        }
        printf("SSH client initialized\r\n");
        wolfSSH_SetUserAuthCtx(ssh, &pwMapList);
        /* Use the session object for its own highwater callback ctx */
        if (defaultHighwater > 0) {
            wolfSSH_SetHighwaterCtx(ssh, (void*)ssh);
            wolfSSH_SetHighwater(ssh, defaultHighwater);
        }
        printf("Accepting connections on port 22\r\n");

        clientFd = accept(listenFd, (struct sockaddr*)&clientAddr,
                                                                 &clientAddrSz);
        if (clientFd == -1)
            err_sys("tcp accept failed");

        printf("Accepted TCP connection\r\n");

        wolfSSH_set_fd(ssh, clientFd);

        server_worker(ssh, clientFd);
#if 0

        pthread_create(&thread, 0, server_worker, threadCtx);

        if (multipleConnections)
            pthread_detach(thread);
        else
            pthread_join(thread, NULL);
#endif

    } while (multipleConnections);

    PwMapListDelete(&pwMapList);
    wolfSSH_CTX_free(ctx);
    if (wolfSSH_Cleanup() != WS_SUCCESS) {
        fprintf(stderr, "Couldn't clean up wolfSSH.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int myoptind = 0;
char* myoptarg = NULL;
