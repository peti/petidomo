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
#include <sys/types.h>
#include <regex.h>
#include "text.h"

/* Simple front end to regular expressions.

   This is a simple front end to make the usage of regular expressions
   easier. The routine tests whether the pattern provided in 'pattern'
   matches the text in 'buffer' or not. Comparisons are done
   case-insensitive, using the extended regular expression language.

   RETURNS: If the pattern matches, TRUE is returned. Otherwise the
   routine will return FALSE.

   NOTE: The error handling is somewhat relaxed... well, don't use the
   routine if you need error handling.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

bool
text_easy_pattern_match(const char * buffer, /* text buffer */
                 const char * pattern /* regular expression */
                 )
{
    regex_t   preg;
    int       rc;

    /* Sanity checks. */

    assert(buffer != NULL);
    assert(pattern != NULL);
    if (!buffer || !pattern)
      return FALSE;

    /* Compile the regular expression. */

    rc = regcomp(&preg, pattern, REG_EXTENDED | REG_ICASE | REG_NOSUB | REG_NEWLINE);
    if (rc != 0)
        return FALSE;

    /* Match it. */

    rc = regexec(&preg, buffer, 0, NULL, 0);
    regfree(&preg);

    if (rc == 0)
      return TRUE;
    else
      return FALSE;
}
