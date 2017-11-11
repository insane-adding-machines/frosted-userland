/* Posix filesystem test */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

static int passed = 0;
static int failed = 0;


#define fail_if(cond, ...) if ((cond)) { fprintf(stderr, ##__VA_ARGS__); fprintf(stderr,"(at line: %d)\r\n", __LINE__); failed++; } else {passed++;}
#define fail_unless(cond, ...) if (!(cond)) { fprintf(stderr, ##__VA_ARGS__); fprintf(stderr,"(at line: %d\r\n", __LINE__); failed++; } else {passed++;}

const char content[] =
"Aut dolores sunt ex. Cupiditate deserunt impedit aut. Consequatur deserunt consequuntur illo.\r\n"
"Dolores omnis omnis ipsum necessitatibus delectus adipisci. Dolorem soluta possimus labore asperiores rerum. Qui doloribus voluptatibus commodi sit dolorem quo sapiente. Ea in aut delectus.\r\n"
"Eum voluptates totam temporibus sed. Qui eos quia est eveniet nostrum et. Itaque ipsum dolores repellendus sunt. Qui officiis sed vel. Laudantium aperiam beatae et quas quod. Deleniti consectetur animi sunt vel et aut odio.\r\n"
"Culpa quis explicabo quae quis nobis autem ea. Est nihil expedita quia voluptas esse eum. Velit quidem ipsa aut ut hic. Id non consequuntur quia. Cupiditate ut aspernatur asperiores ad.\r\n"
"Dolor voluptas voluptate deserunt. Magni ab quia nesciunt quis quia ut dolorum atque. Illum provident dolores quia id enim repellendus. Temporibus vel reiciendis quisquam voluptatibus blanditiis. Quisquam neque vero cupiditate quo. Dolor praesentium maiores repellendus.\r\n";

#define TXTSIZE (1024)
static char txtbuf[TXTSIZE];

int main(int argc, char *argv[])
{
    char path[256];
    int dirnamelen;
    int ret, err, fd;
    int content_len = strlen(content);
    int off = content_len - 200;
    struct stat st;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s target_path\r\n", argv[0]);
        exit(1);
    }
    dirnamelen = strlen(argv[1]);
    strcpy(path, argv[1]);

    /* Opening non-existing file */
    printf("Opening non-existing file\r\n");
    strcpy(path, argv[1]);
    strcat(path, "/__non-existent");
    ret = open(path, O_RDONLY);
    err = errno;
    fail_unless(ret == -1, "open(nonexistent): Failed! open returned %d", ret);
    fail_unless(err == ENOENT, "open(nonexistent): Unexpected errno (%d)", err);

    /* Creating a new file */
    printf("Creating a new file\r\n");
    strcpy(path, argv[1]);
    strcat(path, "/test.txt");
    unlink(path);
    ret = open(path, O_RDWR|O_CREAT|O_EXCL, 0600);
    fail_if(ret < 0, "create new file: Failed! open returned %d", ret);

    /* Write/lseek/read */
    printf("...Write\r\n");
    fd = ret;
    ret = write(fd, content, content_len);
    err = errno;
    fail_if(ret != content_len,  "writing to file: Failed! write returned %d, errno=%d", ret, err);

    printf("...Lseek\r\n");
    ret = lseek(fd, 0, SEEK_SET);
    fail_if(ret != 0, "lseek returns wrong offset %d", ret);

    printf("...Read\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != content_len, "read %d bytes, expected %d", ret, content_len);

    printf("...Read EOF\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
    close(fd);

    /* re-open, read */
    printf("Re-opening the file\r\n");
    ret = open(path, O_RDONLY);
    fail_if(ret < 0, "open existing file: Failed! open returned %d", ret);
    fd = ret;

    printf("...Read\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != content_len, "read %d bytes, expected %d", ret, content_len);
    txtbuf[ret] = 0;

    printf("...Read EOF\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
    printf("...Compare\r\n");
    fail_if(strcmp(txtbuf, content), "Corrupted content of the stored file");
    close(fd);
    
    /* stat */
    printf("Stat + filesize verification\r\n");
    ret = stat(path, &st);
    err = errno;
    fail_if(ret != 0, "stat failed with errno %d", err);
    fail_if(st.st_size != content_len, "wrong size: expected %d got %d", content_len, st.st_size);

    /* truncate to off */
    printf("Truncate/re-open\r\n");
    ret = truncate(path, off);
    ret = open(path, O_RDONLY);
    fail_if(ret < 0, "open truncated file: Failed! open returned %d", ret);
    fd = ret;

    printf("...Read\r\n");
    ret = read(fd, txtbuf, off);
    fail_if(ret != off, "read %d bytes, expected %d", ret, off);
    txtbuf[ret] = 0;

    printf("...Read EOF\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
    printf("...Compare\r\n");
    fail_if(strncmp(txtbuf, content, off), "Corrupted content of the stored file");
    close(fd);

    /* stat */
    printf("Stat + filesize verification\r\n");
    ret = stat(path, &st);
    err = errno;
    fail_if(ret != 0, "stat failed with errno %d", err);
    fail_if(st.st_size != off, "wrong size: expected %d got %d", off, st.st_size);

    /* truncate to zero */
    printf("Truncate/re-open\r\n");
    ret = truncate(path, 0);
    ret = open(path, O_RDWR|O_EXCL);
    fail_if(ret < 0, "open truncated file: Failed! open returned %d", ret);
    fd = ret;

    printf("...Read EOF\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
    close(fd);

    /* stat */
    printf("Stat + filesize verification\r\n");
    ret = stat(path, &st);
    err = errno;
    fail_if(ret != 0, "stat failed with errno %d", err);
    fail_if(st.st_size != 0, "wrong size: expected %d got %d", 0, st.st_size);

    /* append */
    printf("Open in APPEND mode\r\n");
    ret = open(path, O_RDWR|O_EXCL|O_APPEND);
    fail_if(ret < 0, "open for append: Failed! open returned %d", ret);
    fd = ret;
    
    printf("...Write\r\n");
    ret = write(fd, content, off);
    err = errno;
    fail_if(ret != off,  "writing to truncated file: Failed! write returned %d, errno=%d", ret, err);
    close(fd);

    printf("Open in APPEND mode (2nd part)\r\n");
    ret = open(path, O_RDWR|O_EXCL|O_APPEND);
    fail_if(ret < 0, "open for append: Failed! open returned %d", ret);
    fd = ret;
    printf("...Write\r\n");
    ret = write(fd, content + off, content_len - off);
    err = errno;
    fail_if(ret != (content_len - off),  "writing to truncated file: Failed! write returned %d, errno=%d", ret, err);
    close(fd);
    
    /* stat */
    printf("Stat + filesize verification\r\n");
    ret = stat(path, &st);
    err = errno;
    fail_if(ret != 0, "stat failed with errno %d", err);
    fail_if(st.st_size != content_len, "wrong size: expected %d got %d", content_len, st.st_size);

    /* re-open, read */
    printf("Re-opening the file\r\n");
    ret = open(path, O_RDONLY);
    fail_if(ret < 0, "open existing file: Failed! open returned %d", ret);
    fd = ret;

    printf("...Read\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != content_len, "read %d bytes, expected %d", ret, content_len);
    txtbuf[ret] = 0;

    printf("...Read EOF\r\n");
    ret = read(fd, txtbuf, TXTSIZE);
    fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
    printf("...Compare\r\n");
    fail_if(strcmp(txtbuf, content), "Corrupted content of the stored file");
    close(fd);
    
    /* re-open, lseek to last 200B, read */
    printf("Re-opening the file\r\n");
    ret = open(path, O_RDONLY);
    fail_if(ret < 0, "open existing file: Failed! open returned %d", ret);
    fd = ret;

    printf("...Lseek to -200\r\n");
    ret = lseek(fd, -200, SEEK_END);
    fail_if(ret != off, "lseek returns wrong offset %d", ret);

    if (ret == off) {
        printf("...Read\r\n");
        ret = read(fd, txtbuf, 200);
        fail_if(ret != 200, "read %d bytes, expected %d", ret, 200);
        txtbuf[ret] = 0;

        printf("...Read EOF\r\n");
        ret = read(fd, txtbuf, TXTSIZE);
        fail_if(ret != 0, "read %d bytes, expected EOF", ret, content_len);
        printf("...Compare\r\n");
        fail_if(strcmp(txtbuf, content + off), "Corrupted content of the stored file");
    }
    close(fd);

    /* delete */
    printf("Delete file\r\n");
    fail_if(unlink(path) != 0, "unlink failed with errno %d", errno);

    /* open deleted file */
    printf("Try to reopen\r\n");
    ret = open(path, O_RDONLY);
    err = errno;
    fail_unless(ret == -1, "open(deleted): Failed! open returned %d", ret);
    fail_unless(err == ENOENT, "open(deleted): Unexpected errno (%d)", err);


    fprintf(stderr, "--\r\n\r\nTest passed: %d failed: %d\r\n", passed, failed);

    if (failed == 0)
        fprintf(stderr, "All tests completed successfully.\r\n");

    exit(0);
}



