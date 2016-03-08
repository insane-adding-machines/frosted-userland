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
#include <string.h>

# define LED0 "/dev/led0"

/* dits - morse short blink */
int dits(int led)
{
    write(led, "1", 1);
    sleep(100);
    write(led, "0", 1);
    sleep(100);
    return 0;
}

/* dahs - morse long blink */
int dahs(int led)
{
    write(led, "1", 1);
    sleep(300);
    write(led, "0", 1);
    sleep(100);
    return 0;
}

/* morse blink word on a led */
int blinkm(int led, char *word) {
	const uint8_t morse[][6] = 	{{1, 2, 0},		// a
					{2, 1, 1, 1, 0},	// b
					{2, 1, 2, 1, 0},	// c
					{2, 1, 1, 0},		// d
					{1, 0},			// e
					{1, 1, 2, 1, 0},	// f
					{2, 2, 1, 0},		// g
					{1, 1, 1, 1, 0},	// h
					{1, 1, 0},		// i
					{1, 2, 2, 2, 0},	// j
					{2, 1, 2, 0},		// k
					{1, 2, 1, 1, 0},	// l
					{2, 2, 0},		// m
					{2, 1, 0},		// n
					{2, 2, 2, 0},		// o
					{1, 2, 2, 1, 0},	// p
					{2, 2, 1, 2, 0},	// q
					{1, 2, 1, 0},		// r
					{1, 1, 1, 0},		// s
					{2, 0},			// t
					{1, 1, 2, 0},		// u
					{1, 1, 1, 2, 0},	// v
					{1, 2, 2, 0},		// w
					{2, 1, 1, 2, 0},	// x
					{2, 1, 2, 2, 0},	// y
					{2, 2, 1, 1, 0},	// z
					{2, 2, 2, 2, 2, 0},	// 0
					{1, 2, 2, 2, 2, 0},	// 1
					{1, 1, 2, 2, 2, 0},	// 2
					{1, 1, 1, 2, 2, 0},	// 3
					{1, 1, 1, 1, 2, 0},	// 4
					{1, 1, 1, 1, 1, 0},	// 5
					{2, 1, 1, 1, 1, 0},	// 6
					{2, 2, 1, 1, 1, 0},	// 7
					{2, 2, 2, 1, 1, 0},	// 8
					{2, 2, 2, 2, 1, 0},	// 9
			};

	int j = 0, dec = 0x42;

	while (*word != '\0') {
		while (1) {
			if (*word == ' ') {
				sleep(200);
				break;
			} else if (*word >= 'a' && *word <= 'z') {
				dec = 'a';
			} else if (*word >= 'A' && *word <= 'Z') {
				dec = 'A';
			} else if (*word >= '0' && *word <= '9') {
				dec = 0x30;
			}
			if (morse[(*word - dec)][j] == 1) {
				dits(led);
				j++;
			} else if (morse[(*word - dec)][j] == 2) {
				dahs(led);
				j++;
			} else if (morse[(*word - dec)][j] == 0) {
				sleep(200);
				j = 0;
				break;
			}
		}
		word++;
	}
	return 0;
}

/* main - open led, handle arguments, if any, close led */
int main(int argc, char *argv[])
{
	int led = 0;
	int k = 1;
	char *noargs = "No arguments!";

	led = open(LED0, O_RDWR, 0);

	if (!led) {
		printf("Error opening led!\r\n");
		exit(1);
	}

	printf("Blinking..");
	fflush(stdout);

	if (argc < 2) {
		blinkm(led, noargs);
		printf("done!\r\n");
		fflush(stdout);
		exit(0);
	}

	do {
		blinkm(led, argv[k]);
		sleep(200);

		k++;
	} while (argc > k);

	printf("done!\r\n");
	fflush(stdout);
	close(led);

	exit(0);
}
