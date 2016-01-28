#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static uint32_t read_rand(void)
{
	int rngfd;
	rngfd = open("/dev/random", O_RDONLY);
	char buf[10];
	int ret = read(rngfd, buf, 4);
	close(rngfd);
	return *((uint32_t *) buf);
}

int main(int argc, char *argv[])
{
	printf("\r\nRandom number: \t%u\r\n\r\n", read_rand());
	exit(0);
}
