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
 #include <sys/time.h>
 #include <time.h>

 int main(int argc, char *args[])
 {
	 struct timeval tv;
	 time_t now;
	 long days = 0;

	 gettimeofday(&tv, NULL);
	 now = tv.tv_sec;

	 if (now > 86400) {
		 days = now / 86400;
	 }

	 struct tm *tm = localtime(&now);
	 printf(" %d:%d:%d up %ld days, %02d:%02d, 1 user\r\n", tm->tm_hour, tm->tm_min, tm->tm_sec, days, tm->tm_hour, tm->tm_min);

	 exit(0);
 }
