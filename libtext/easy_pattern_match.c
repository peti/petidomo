/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

   This file is part of OpenPetidomo.

   OpenPetidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   OpenPetidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with OpenPetidomo; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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
