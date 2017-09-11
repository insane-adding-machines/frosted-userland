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
 *      Authors: brabo
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>

#define FILE_SIZE   1024

/* ELF */
#define EI_NIDENT (16)
#define EI_CLASS    4
#define EI_DATA    5
#define EI_VERSION    6

#define EI_OSABI    7       /* OS ABI identification */
#define ELFOSABI_NONE       0   /* UNIX System V ABI */
#define ELFOSABI_SYSV       0   /* Alias.  */
#define ELFOSABI_HPUX       1   /* HP-UX */
#define ELFOSABI_NETBSD     2   /* NetBSD.  */
#define ELFOSABI_GNU        3   /* Object uses GNU ELF extensions.  */
#define ELFOSABI_LINUX      ELFOSABI_GNU /* Compatibility alias.  */
#define ELFOSABI_SOLARIS    6   /* Sun Solaris.  */
#define ELFOSABI_AIX        7   /* IBM AIX.  */
#define ELFOSABI_IRIX       8   /* SGI Irix.  */
#define ELFOSABI_FREEBSD    9   /* FreeBSD.  */
#define ELFOSABI_TRU64      10  /* Compaq TRU64 UNIX.  */
#define ELFOSABI_MODESTO    11  /* Novell Modesto.  */
#define ELFOSABI_OPENBSD    12  /* OpenBSD.  */
#define ELFOSABI_ARM_AEABI  64  /* ARM EABI */
#define ELFOSABI_ARM        97  /* ARM */
#define ELFOSABI_STANDALONE 255 /* Standalone (embedded) application */

#define EI_ABIVERSION   8       /* ABI version */

#define EI_PAD      9       /* Byte index of padding bytes */

#define ET_NONE     0       /* No file type */
#define ET_REL      1       /* Relocatable file */
#define ET_EXEC     2       /* Executable file */
#define ET_DYN      3       /* Shared object file */
#define ET_CORE     4       /* Core file */
#define ET_NUM      5       /* Number of defined types */
#define ET_LOOS     0xfe00      /* OS-specific range start */
#define ET_HIOS     0xfeff      /* OS-specific range end */
#define ET_LOPROC   0xff00      /* Processor-specific range start */
#define ET_HIPROC   0xffff      /* Processor-specific range end */

#define EM_NONE      0  /* No machine */
#define EM_M32       1  /* AT&T WE 32100 */
#define EM_SPARC     2  /* SUN SPARC */
#define EM_386       3  /* Intel 80386 */
#define EM_ARM      40  /* ARM */
#define ELF_MAGIC 0x464C457F

struct elf32_ehdr {
  uint8_t  e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

/* bFLT */
#define BFLT_GOTPIC 2
#define BFLT_MAGIC 0x544C4662

struct flat_hdr {
    char magic[4];
    uint32_t rev;
    uint32_t entry;
    uint32_t data_start;
    uint32_t data_end;
    uint32_t bss_end;
    uint32_t stack_size;
    uint32_t reloc_start;
    uint32_t reloc_count;
    uint32_t flags;
    uint32_t filler[6];
};

int is_elf(uint8_t *b, int len)
{
    struct elf32_ehdr *e = (struct elf32_ehdr *)b;
    if (*(uint32_t *)e->e_ident == ELF_MAGIC) {
        printf("ELF ");
        if (e->e_ident[EI_CLASS] == 1)
            printf("32-bit ");
        else if (e->e_ident[EI_CLASS] == 2)
            printf("64-bit ");

        if (e->e_ident[EI_DATA] == 1)
            printf("LSB ");
        else if (e->e_ident[EI_DATA] == 2)
            printf("MSB ");

        switch (e->e_type) {
            case ET_NONE:
                break;
            case ET_REL:
                printf("relocatable, ");
                break;
            case ET_EXEC:
                printf("executable, ");
                break;
            default:
                break;
        }

        switch (e->e_machine) {
            case EM_386:
                printf("Intel 80386, ");
                break;
            case EM_ARM:
                printf("ARM, ");
                break;
            default:
                break;
        }

        if (e->e_flags & 0x05000000)
            printf("EABI5 ");

        printf("version %d ", e->e_ident[EI_VERSION]);

        switch (e->e_ident[EI_OSABI]) {
            case ELFOSABI_ARM_AEABI:
                printf("EABI, ");
                break;
            case ELFOSABI_SYSV:
                printf("(SYSV)");
                break;
            default:
                break;
        }

        return 0;
    }

    return -1;
}

int is_bflt(uint8_t *b, int nb)
{
    struct flat_hdr *f = (struct flat_hdr *)b;
    if (*(uint32_t *)f->magic == BFLT_MAGIC) {
        printf("BFLT executable - version %d ", ntohl(f->rev));
        if (f->flags & ntohl(BFLT_GOTPIC))
            printf("gotpic");
        return 0;
    }

    return -1;
}

void is_ascii(uint8_t *b, int nb)
{
    int i;
    for (i = 0; i < nb; i++) {
        if (b[i] == 0) {
            printf("data");
            return;
        }
    }

    printf("ASCII text");
}

int is_script(uint8_t *b, int nb)
{
    if (!strncmp(b, "#!", 2)) {
        if ((strstr(b, "/sh")) || (strstr(b, " sh"))) {
            printf("POSIX shell script, ");
            return 0;
        } else if ((strstr(b, "/fresh")) || (strstr(b, " fresh"))) {
            printf("Fresh shell script, ");
            return 0;
        } else if ((strstr(b, "/bash")) || (strstr(b, " bash"))) {
            printf("Bash shell script, ");
            return 0;
        } else if ((strstr(b, "/python")) || (strstr(b, " python"))) {
            printf("Python script, ");
            return 0;
        }
    }

    return -1;
}

int is_exec(uint8_t *b, int nb)
{
    if (!is_elf(b, nb))
        return 0;

    if (!is_bflt(b, nb))
        return 0;

    if (!is_script(b, nb))
        return 1;

    return -1;
}

void ex_regfile(char *name)
{
    int fd = open(name, O_RDONLY);
    if (fd < 0) {
        perror("Open failed");
        return;
    }

    uint8_t buf[FILE_SIZE];
    memset(buf, 0, (FILE_SIZE));

    int nb = read(fd, buf, FILE_SIZE);

    if (nb < 0) {
        perror("Read failed");
        goto end;
    }

    if (!is_exec(buf, nb))
        goto end;

    is_ascii(buf, nb);

end:
    close(fd);
}

void ex_ext(char *name)
{
    int len = strlen(name);
    char *ext = name + len;
    while ((*ext != '.') && (ext != name))
        ext--;
    if (ext != name) {
        ext++;
        if (!strncmp(ext, "c", 1))
            printf("C source, ");
    }
}

#ifdef APP_FILE_STANDALONE
int main(int argc, char *argv[])
#else
int icebox_file(int argc, char *argv[])
#endif
{
    if (argc < 2)
        exit(-EINVAL);

    int i;
    for (i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);

        struct stat s;
        lstat(argv[i], &s);

        if (S_ISREG(s.st_mode)) {
            ex_ext(argv[i]);
            ex_regfile(argv[i]);

        } else if (S_ISDIR(s.st_mode)) {
            printf("directory");

        } else if (S_ISCHR(s.st_mode)) {
            printf("character special");

        } else if (S_ISBLK(s.st_mode)) {
            printf("block special");

        } else if (S_ISFIFO(s.st_mode)) {
            printf("fifo (named pipe)");

        } else if (S_ISLNK(s.st_mode)) {
            printf("symbolic link");
            char buf[PATH_MAX];
            memset(buf, 0, PATH_MAX);
            readlink(argv[1], buf, PATH_MAX);
            printf(" to %s", buf);
        } else if (S_ISSOCK(s.st_mode)) {
            printf("socket");
        }

        printf("\n");
    }

    // if you're happy and you know it,
    exit(0);
}
