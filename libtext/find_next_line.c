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
