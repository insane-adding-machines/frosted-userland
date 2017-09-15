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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef APP_SLEEP_MODULE
int main(int argc, char *args[])
#else
int icebox_sleep(int argc, char *args[])
#endif
{
	if ((argc < 2) || (argc > 2)) {
		fprintf(stderr, "Usage: sleep TIME\r\n");
	}
	long sec = strtol(args[1], NULL, 10);
	sleep(sec);
	exit(0);
}
