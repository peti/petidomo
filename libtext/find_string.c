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
