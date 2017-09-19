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
#include "test.h"
#include "basename.h"
#include "chdir.h"
#include "dirname.h"
#include "mem.h"
#include "pthreads.h"

int main(int argc, char *args[])
{
	int ret;

//	ret = basename_tests();
//	if (ret) {
//		printf("basename tests failed!\n");
//		exit(ret);
//	} else {
//		printf("basename tests succeeded!\n");
//	}

	ret = chdir_tests();
	if (ret) {
		printf("chdir tests failed!\n");
		exit(ret);
	} else {
		printf("chdir tests succeeded!\n");
	}


	ret = dirname_tests();
	if (ret) {
		printf("dirname tests failed!\n");
		exit(ret);
	} else {
		printf("dirname tests succeeded!\n");
	}

	ret = mem_tests();
	if (ret) {
		printf("memory tests failed!\n");
		exit(ret);
	} else {
		printf("memory tests succeeded!\n");
	}

	//ret = pthreads_tests();
	//if (ret) {
	//	printf("pthreads tests failed!\n");
	//	exit(ret);
	//} else {
	//	printf("pthreads tests succeeded!\n");
	//}

	exit(0);
}
