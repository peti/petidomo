/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
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
