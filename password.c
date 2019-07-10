/*
 * Copyright (c) 1995-2019 Peter Simons <simons@cryp.to>
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

#include <ctype.h>
#include <string.h>
#include "petidomo.h"

static const char * s_password = NULL;
extern char *       g_currLine;

int
setPassword(struct Mail * MailStruct,
                const char * param1,
                const char * param2,
                const char * defaultlist)
{
    char *         p;
    char *         q;

    /* Find the beginning of the parameter. */

    p = g_currLine;
    while(*p && !isspace((int)*p))
      p++;
    while(*p && isspace((int)*p))
      p++;

    /* If the rest is empty, there ain't no fucking password. */

    if (*p == '\0' || strlen(p) == 0)
      return 0;

    /* Cut trailing blanks. */

    q = p + strlen(p);
    while(isspace((int)q[-1]))
      q--;
    *q = '\0';

    /* Okay, check for quotes and that's it then. */

    if (*p == '\"' && q[-1] == '\"') {
        p++;
        q[-1] = '\0';
    }

    /* Store the result. */

    s_password = p;

    return 0;
}

const char *
getPassword(void)
{
    return s_password;
}
