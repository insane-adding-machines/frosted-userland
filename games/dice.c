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
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

static uint32_t read_rand(void)
{
	int rngfd;
	rngfd = open("/dev/random", O_RDONLY);
	char buf[10];
	int ret = read(rngfd, buf, 1);
	close(rngfd);
	return *((uint32_t *) buf);
}

/*
 * Rolls a dice for each of two players.
 *
 * Returns:		1 if player 1 wins.
 *			-1 if player 2 wins.
 *			0 if it is a tie.
 */
int roll(void)
{
	uint8_t player1 = read_rand() % 6;
	uint8_t player2 = read_rand() % 6;
	if (player1 > player2) {
		return 1;
	} else if (player1 < player2) {
		return -1;
	}
	return 0;
}

int main(int argc, char *args[])
{
 	char c;
 	int noes = 2;
 	int yes = 0;
 	int user = 0, mcu = 0;

 	printf("\r\nRoll the dice - try out your luck!\r\n\r\n\r\n");
 	while (1) {
 		if (!yes) {
 			printf("Roll?  (Y/n) ");
 			read(0, &c, 1);
 		} else {
 			yes = 0;
 			c = '\n';
 		}
 		printf("\r");
		if (tolower(c) != 'n') {
			int ret = roll();

			if (ret > 0) {
				user++;
				printf("User won!");
			} else if (ret < 0) {
				mcu++;
				printf("Mcu won!");
			} else {
				printf("It's a tie!");
			}

			printf(" \tstats:  user: \t%d  -  mcu: \t%d\r\n\r\n", user, mcu);

		} else {
			if (noes > 0) {
				printf("Naw, that's really a yes ain't it??\r\n");
				noes--;
				yes++;
			} else {
				break;
			}
		}
 	}

 	printf("\r\n\r\nRoll the dice is finished! Final score:\r\n\r\n User: \t%d\r\n Mcu: \t%d\r\n\r\nVICTORY FOR ", user, mcu);
 	if (user > mcu) {
 		printf("USER!!!!\r\n");
 	} else if (user < mcu) {
 		printf("MCU!!!!\r\n");
 	} else {
 		printf("NO ONE - it's a TIE!!!!\r\n");
 	}

 	printf("\r\nThank you, come again!\r\n\r\n");
 	exit(0);
}
