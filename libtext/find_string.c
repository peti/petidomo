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

#include <string.h>
#include "text.h"

/* A simple text search.

   This function performs a simple, case-independent text search in
   the indicated buffer.

   RETURNS: FindString() returns a pointer to the first occurance of
   "string" in "buffer", or NULL, if the string was not found.

   NOTE: This routine will perform poorly on larger text buffers.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

char *
text_find_string(char * buffer,	/* text buffer */
	   char * string	/* string to find */
)

{
    int         buffer_len;
    int         string_len;
    char *      result;

    /* Sanity checks. */

    assert(buffer != NULL);
    assert(string != NULL);
    if (!buffer || !string)
      return NULL;

    buffer_len = strlen(buffer);
    string_len = strlen(string);

    if (buffer_len == 0 || buffer_len < string_len)
      return NULL;
    if (string_len == 0)
      return buffer;

    /* Now look for the string. */

    for(result = NULL; buffer_len >= string_len; buffer++, buffer_len--) {
	if (!strncasecmp(buffer, string, string_len)) {
	    result = buffer;
	    break;
	}
    }
    return result;
}
