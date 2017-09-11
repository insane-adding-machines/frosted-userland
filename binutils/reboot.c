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
 *      Authors: danielinux
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <unistd.h>

#ifdef APP_REBOOT_STANDALONE
int main(int argc, char *args[])
#else
int icebox_reboot(int argc, char *args[])
#endif
{
    fprintf(stderr, "Rebooting frosted NOW!\r\n");
    fflush(stderr);
    usleep(500000);
    reboot();
    exit(0); /* Never reached */
}
