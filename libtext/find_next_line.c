/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996,97 by CyberSolutions GmbH.
 *      All rights reserved.
 */

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
