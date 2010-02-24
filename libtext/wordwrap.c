/*
 * Copyright (c) 1995-2010 Peter Simons <simons@cryp.to>
 * Copyright (c) 2000-2001 Cable & Wireless GmbH
 * Copyright (c) 1999-2000 CyberSolutions GmbH
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
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
