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

#ifndef APP_UPTIME_MODULE
int main(int argc, char *args[])
#else
int icebox_uptime(int argc, char *args[])
#endif
 {
	 struct timespec ts;
	 time_t now, hours, mins;
	 long days = 0;

	 clock_gettime(CLOCK_MONOTONIC, &ts);
	 now = ts.tv_sec;

	 if (now > 86400) {
		 days = now / 86400;
	 }

	 struct tm *tm = localtime(&now);
	 hours = tm->tm_hour;
	 mins = tm->tm_min;

	 clock_gettime(CLOCK_REALTIME, &ts);
	 tm = localtime(&ts.tv_sec);
	 printf(" %02d:%02d:%02d up %ld days, %02d:%02d, 1 user\r\n", tm->tm_hour, tm->tm_min, tm->tm_sec, days, hours, mins);

	 exit(0);
 }
