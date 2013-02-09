/*
 * Copyright (c) 1995-2013 Peter Simons <simons@cryp.to>
 * Copyright (c) 2000-2001 Cable & Wireless GmbH
 * Copyright (c) 1999-2000 CyberSolutions GmbH
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "libtext/text.h"
#include "petidomo.h"

/*
 * rc ==  0: Address is not on list.
 * rc == -1: Error occured while reading the file.
 * rc ==  1: Address is on list.
 */
int is_address_on_list(const char* file, const char* address)
    {
    char *         list;
    char *         p;
    unsigned int   len;
    int            rc;

    list = loadfile(file);
    if (list == NULL)
        {
        if (errno == ENOENT)
            return 0;
        else
            return -1;
        }

    for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p))
        {
        if (strncasecmp(p, address, len) == 0 &&
            (p == list || p[-1] == '\n') &&
            (isspace((int)p[len]) || p[len] == '\0'))
            {
            break;
            }
        }

    rc = ((*p != '\0') ? 1 : 0);
    free(list);
    return rc;
    }

int add_address(const char* file, const char* address)
    {
    FILE* fh;
    int rc = is_address_on_list(file, address);
    if (rc != 0)
        return rc;

    fh = fopen(file, "a");
    if (fh == NULL)
        {
        syslog(LOG_ERR, "Failed to open file \"%s\" for writing: %s", file, strerror(errno));
        return -1;
        }
    fprintf(fh, "%s\n", address);
    fclose(fh);
    return 0;
    }
