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
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *args[])
{
    volatile uint32_t p, o, x;
    printf("Starting infinite cpu-consuming loop...\n");
	while (1 < 2) {
        x = o;
        o = p;
        p = getpid();
        x += o + p;
	}
	exit(0);
}
