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
