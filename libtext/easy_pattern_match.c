/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1996-99 by Peter Simons <simons@cys.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Peter Simons.
 *
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
