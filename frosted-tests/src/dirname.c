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

#include <libgen.h>
#include "test.h"

struct dirname_t {
	char path[64];
	char txt[64];
};

static struct dirname_t tests[] = {{ "/dev/test", "/dev"},
							{ "/sys/blah", "/sys"},
							{ "/", "/"},
							{ "/sys", "/"},
							{ "/sys/", "/"},
							{ "/sys/b", "/sys"},
							{ "/sys//////blah", "/sys"},
							{ "///////sys/blah", "///////sys"},
							{ "/sys/b/lah", "/sys/b"},
							{ "/usr/lib", "/usr"},
							{ "/usr/", "/"},
							{ "usr", "."},
							{ ".", "."},
							{ "..", "."},
							{ "", "."},
};

int dirname_tests(void)
{
 	char *path;
 	int i;

 	for (i = 0; i < 9; i++) {
 		path = dirname(tests[i].path);
 		test_str(path, tests[i].txt);
 	}

	return 0;
}

