#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include "xipfs.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

static int xipfs_ls(const uint8_t *blob)
{
    const struct xipfs_fat *fat = (const struct xipfs_fat *)blob;
    const struct xipfs_fhdr *f;
    int i, offset;
    if (!fat || fat->fs_magic != XIPFS_MAGIC)
        return -1;

    offset = sizeof(struct xipfs_fat);
    for (i = 0; i < fat->fs_files; i++) {
        f = (const struct xipfs_fhdr *) (blob + offset);
        if (f->magic == XIPFS_MAGIC) {
            printf("%s (%d bytes)\n", f->name, f->len);
            offset += f->len + sizeof(struct xipfs_fhdr);
        } 
        if (f->magic == XIPFS_MAGIC_ICELINK) {
            printf("%s (link to icebox)\n", f->name);
            offset += sizeof(struct xipfs_fhdr);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int img;
    int i;
    int r;
    struct xipfs_fat fat;
    uint32_t size = 0;
    uint32_t count = 0;
    struct stat st;
    uint8_t *blob;


    if (argc < 2) {
        fprintf(stderr, "please specify a xipfs filesystem file\n");
        exit(1);
    }

    if (stat(argv[1], &st) < 0) {
        perror("stat");
        exit(1);
    }
    img = open(argv[1], O_RDONLY, 0660);
    if (img < 0) {
        perror("opening image file");
        exit(2);
    }
    blob = malloc(st.st_size);
    if (!blob) {
        perror("malloc");
        exit(3);
    }
    r = read(img, blob, st.st_size);
    if (r != st.st_size) {
        perror("malloc");
        exit(4);
    }
    printf ("Content of %s:\n", argv[1]);
    return xipfs_ls(blob);
}
