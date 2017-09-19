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

#include <unistd.h>
#include "test.h"

static char tests[6][64] = { "/dev/",
						"/",
						"/dev",
						"/",
						"dev",
						"/",
//						"/nonsense/",
};

int chdir_tests(void)
{
 	int ret;
 	int i;
 	char buf[64];

 	for (i = 0; i < 6; i++) {
 		ret = chdir(tests[i]);
 		snprintf(buf, sizeof (buf), "chdir test #%d", (i + 1));
 		fail_if((ret < 0), buf);
 		//test_str(path, tests[i].txt);
 	}

	return 0;
}

