
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
 *      Authors: Daniele Lacamera, Maxime Vincent
 *
 */
#ifndef INCLUDED_FROSTED_BINUTILS
#define INCLUDED_FROSTED_BINUTILS
#include "frosted_api.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <ioctl.h>

#ifndef STDIN_FILENO
#   define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#   define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#   define STDERR_FILENO 2
#endif

#define BUFSIZE 256
#define MAXFILES 13

#define ARCH	"Unknown"
struct binutils {
    char name[32];
    int (*exe)(void **args);
};

struct lsm303dlhc_ctrl_reg
{
    uint8_t reg;
    uint8_t data;
}; 

struct l3gd20_ctrl_reg
{
    uint8_t reg;
    uint8_t data;
}; 

#endif
