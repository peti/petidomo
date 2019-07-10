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
#include "text.h"

/* Find the next text line.

   This routine scans the text provided buffer until it encounters
   either a linefeed (\\n) or a null byte (\\0). If it finds a line
   feed, it returns a pointer to the *next* line. If it finds a null
   byte, a pointer to that null byte is returned.

   Using this routine you can easily scan a text buffer line for line,
   just use a look like this:

     while((p = text_find_next_line(p)) != '\\0') ...

   AUTHOR: Peter Simons <simons@rhein.de>

 */

char *
text_find_next_line(char * buffer /* string pointer */)
{
    assert(buffer != NULL);

    while (*buffer != '\n' && *buffer != '\0')
      buffer++;
    if (*buffer == '\n')
      buffer++;
    return buffer;
}
