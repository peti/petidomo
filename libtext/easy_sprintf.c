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
