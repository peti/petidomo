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

#include <stdarg.h>
#include <string.h>
#include "text.h"

#if !defined(HAVE_VSNPRINTF) && defined(HAVE___VSNPRINTF)
#  define vsnprintf __vsnprintf
#endif

/* Use sprintf() without worrying about buffer size.

   One problem when dealing a lot with texts and strings is the buffer
   handling. Usually a programmer will frequently have to format
   several strings into a new one using sprintf(3), without knowing
   how long the resulting string will become.

   text_easy_sprintf() will solve this problem. The syntax is very much
   like sprintf(), except for that the target buffer is allocated by
   the routine and returned to the caller.

   RETURNS: A buffer holding the formatted string or NULL if the
   memory allocation failed.

   NOTE: The returned buffer has been allocated with malloc(3) and
   should be freed when not needed anymore.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

char *
text_easy_sprintf(const char * fmt  /* Format string in printf(3) syntax. */,
		  ... /* Parameter list. */)
{
    char *    buffer,
	 *    result;
    size_t    buffer_size;
    va_list   ap;
    int       rc;

    /* Sanity checks. */

    assert(fmt != NULL);
    if (fmt == NULL)
      return NULL;

    /* Do the work. */

    va_start(ap, fmt);
    for (buffer_size = 0, result = NULL; TRUE; free(buffer)) { /* forever */

	/* Allocate the internal buffer. */

	buffer_size += 8 * 1024;
	buffer = malloc(buffer_size);
	if (buffer == NULL)
	  goto leave;

	/* Format the string into it. */

	rc = vsnprintf(buffer, buffer_size, fmt, ap);

	/* Success? */

	if (rc < buffer_size) {
	    /* Yes. */

	    result = strdup(buffer);
	    free(buffer);
	    goto leave;
	}

	/* No. */
    }

leave:
    va_end(ap);
    return result;
}
