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
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "test.h"

size_t sizes[] = { 1, 64, 128, 256, 512, 1024, 99999999999 };

int mem_tests(void)
{
 	void *p, *retp;
 	int i;
 	char buf[64];

 	for (i = 0; i < 6; i++) {
 		p = malloc(sizes[i] * sizeof (char));
 		snprintf(buf, sizeof (buf), "malloc test #%d", (i + 1));
 		err_fail_if((p == NULL), buf);
 	}

 	srand(time(NULL));

// 	for (i = 0; i < 100000000; (i += 1024)) {
//		size_t size = i;//(rand() % 1024) + 1;
// 		printf("%d: malloc %d\n", i,  size);
// 		usleep(1000);
// 		p = malloc(size * sizeof (char));
// 		snprintf(buf, sizeof (buf), "malloc test #%d with size %d", (i + 1), size);
// 		if(p == NULL) break;
// 		free(p);
// 	}

 	int max = 102400000;
 	for (i = 0; i < 10000; i++) {
 		size_t size = (rand() % max) + 1;
 		printf("%d: malloc %d\n", i,  size);
 		//usleep(1000);
 		//sleep(1);
 		p = malloc(size * sizeof (char));
 		snprintf(buf, sizeof (buf), "malloc test #%d with size %d", (i + 1), size);
 		err_fail_if((p == NULL), buf);
 		printf("%d: memset %d\n", i,  size);
 		//usleep(1000);
 		//sleep(1);
 		retp = memset(p, 0x42, size);
 		snprintf(buf, sizeof (buf), "memset test #%d with size %d", (i + 1), size);
 		fail_if((p != retp), buf);
 		free(p);
 	}


	return 0;
}

