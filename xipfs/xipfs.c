#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "xipfs.h"
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>


static char *progname;

void usage(void) 
{
    fprintf(stderr, "Usage: %s image file1 file2 ...\n", progname);
    exit(1);
}

static int add_link(int img, char *name)
{
    struct xipfs_fhdr hdr;
    uint8_t b;
    uint8_t pad = 0xFF;
    int count = 0;
    memset(&hdr, 0, sizeof(struct xipfs_fhdr));
    hdr.magic = XIPFS_MAGIC_ICELINK;
    strncpy(hdr.name, basename(name), 55);
    hdr.len = 0;
    if (write(img, &hdr, sizeof(struct xipfs_fhdr)) != sizeof(struct xipfs_fhdr)) {
        perror("write");
        exit(3);
    }
    return 0;
}

static int add_bin(int img, int fd, char *name)
{
    struct xipfs_fhdr hdr;
    struct stat st;
    uint8_t b;
    uint8_t pad = 0xFF;
    int count = 0;
    uint32_t pad_len = 0;
    memset(&hdr, 0, sizeof(struct xipfs_fhdr));
    hdr.magic = XIPFS_MAGIC;
    strncpy(hdr.name, basename(name), 55);
    fstat(fd, &st);
    hdr.len = st.st_size;
    pad_len = hdr.len;
    while ((pad_len % 4) != 0)
        pad_len++;
    
    if (write(img, &hdr, sizeof(struct xipfs_fhdr)) != sizeof(struct xipfs_fhdr)) {
        perror("write");
        exit(3);
    }

    while (1 == read(fd, &b, 1)) {
        write(img, &b, 1);
        count++;
    }
    while (count < pad_len) {
        write(img, &pad, 1);
        count++;
    }
    return count;
}


int main(int argc, char *argv[])
{
    int img;
    int i;
    int fd;
    struct xipfs_fat fat;
    uint32_t size = 0;
    uint32_t count = 0;

    progname = argv[0];
    if (argc < 3)
        usage();

    img = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0660);
    if (img < 0) {
        perror("opening image file");
        exit(2);
    }

    memset(&fat, 0, sizeof(struct xipfs_fat));
    fat.fs_files = argc - 2;
    fat.fs_magic = XIPFS_MAGIC;
    if (write(img, &fat, sizeof(struct xipfs_fat)) != sizeof(struct xipfs_fat)) {
        perror("write");
        exit(3);
    }
    
    for (i = 2; i < argc; i++) {
        struct stat st;
        if (lstat(argv[i], &st) != 0) {
            perror(argv[i]);
            exit(3);
        }
        if (S_ISLNK(st.st_mode)) {
            continue;
        }


        fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            perror(argv[i]);
            exit(3);
        }
        count += add_bin(img, fd, argv[i]);
        close(fd);
    }
    for (i = 2; i < argc; i++) {
        struct stat st;
        if (lstat(argv[i], &st) != 0) {
            perror(argv[i]);
            exit(3);
        }
        if (S_ISLNK(st.st_mode)) {
            count += add_link(img, argv[i]);
        }
    }
    fat.fs_size = count;
    lseek(img, 0, SEEK_SET);
    if (write(img, &fat, sizeof(struct xipfs_fat)) != sizeof(struct xipfs_fat)) {
        perror("write");
        exit(3);
    }
    close(img);
    exit(0);
}
