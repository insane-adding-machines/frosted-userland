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
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef APP_YES_MODULE
int main(int argc, char *args[])
#else
int icebox_yes(int argc, char *args[])
#endif
{
	while (1 < 2) {
		if (argc > 1) {
			int i = 1;
			while (args[i]) {
				printf("%s ", args[i]);
				i++;
			}
			printf("\r\n");
		} else {
			printf("y\r\n");
		}
	}
	exit(0);
}
