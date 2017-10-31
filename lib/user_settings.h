#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

/* Configuration */

#define WOLFSSL_GENERAL_ALIGNMENT 4
//#define WOLFSSL_FROSTED 

/* ARM  */
#define RSA_LOW_MEM
#define NO_OLD_RNGNAME  
#define SMALL_SESSION_CACHE
#define WOLFSSL_SMALL_STACK
#define TFM_ARM
//#define NO_INLINE
#define BLOCKING
		
/* Cipher features */
//#define USE_FAST_MATH
#define HAVE_CHACHA 
#define HAVE_POLY1305 
#define HAVE_ECC 
#define HAVE_CURVE25519
#define CURVED25519_SMALL
#define HAVE_ONE_TIME_AUTH
#define HAVE_TLS_EXTENSIONS
#define WOLFSSL_DH_CONST
#define RSA_DECODE_EXTRA
#define HAVE_ECC_SIGN 
#define HAVE_ALL_CURVES 
#define HAVE_SUPPORTED_CURVES
#define HAVE_ED25519
#define HAVE_POLY1305
#define HAVE_SHA512
#define WOLFSSL_SHA512
#define HAVE_AESGCM
// #define ALT_ECC_SIZE // depends on FAST_MATH

/* Robustness */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* Remove Features */
#define NO_WRITEV
#define NO_MD4
#define NO_RABBIT
#define NO_HC128
#define NO_DSA
#define NO_PWDBASED
#define NO_PSK
#define NO_64BIT
#define NO_DES3
#define NO_RC4

#define RNG uint32_t
#define CUSTOM_RAND_GENERATE_BLOCK posixRNG
static inline int posixRNG(uint8_t *output, uint32_t sz)
{
    int fd = open("/dev/urandom", O_RDONLY);
    int r;
    if (fd < 0) 
        return -errno;

    r = read(fd, output, sz);
    if (r != sz)
       return 0;

    close(fd);
    return 0; 
}

