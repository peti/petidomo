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
