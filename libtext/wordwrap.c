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

#include <ctype.h>
#include "text.h"

/* Wrap a text buffer at a certain column.

   This routine wraps all lines in a text buffer that are longer than
   the specified limit. Paragraphs will remain untouched. Paragraphs
   are separated by an empty line -- a line containing only a line
   feed (\\n) that is.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

void
text_wordwrap(char *         buffer    /* Pointer to text buffer. */,
	      unsigned int   line_len  /* Maximum line length. */)
{
    char *         p;
    char *         q;
    char *         lastbreak;
    unsigned int   linepos;

    /* First remove all single occurances of a '\n' and crunch
       multiple blanks to one. */

    for (p = q = buffer; *p != '\0'; ) {
	if (*p == '\n') {
	    if (p[1] == '\n') {
		*q++ = *p++;
		*q++ = *p++;
	    }
	    else {
		*q++ = ' ';
		p++;
	    }
	} else if (isspace((int)*p)) {
	    if (isspace((int)q[-1]))
	      p++;
	    else
	      *q++ = *p++;
	}
	else
	  *q++ = *p++;
    }

    /* Wrap the text. */

    for (p = buffer, lastbreak = NULL, linepos = 1; *p != '\0'; p++, linepos++) {
	if (*p == '\n') {
	    linepos = 0;
	    lastbreak = NULL;
	}
	else if (isspace((int)*p)) {
	    if (linepos <= line_len) {
		lastbreak = p;
	    }
	    else {
		/* Wrap it. */
		if (lastbreak) {
		    *lastbreak = '\n';
		    p = lastbreak;
		    linepos = 0;
		    lastbreak = NULL;
		}
		else {
		    /* Word is too long. <shrug> */
		    *p = '\n';
		    linepos = 0;
		    lastbreak = NULL;
		}
	    }
	}
    }

    if (linepos >= line_len) {
	/* Wrap it one last time. */
	if (lastbreak) {
	    *lastbreak = '\n';
	    p = lastbreak;
	    linepos = 0;
	    lastbreak = NULL;
	}
    }
}
