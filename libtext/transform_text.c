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

#include <sys/types.h>
#include <regex.h>
#include "text.h"

#ifndef MAX_TRANSFORM_ELEMENTS
#  define MAX_TRANSFORM_ELEMENTS 10
#endif

/* Do text-transformations using regular expressions.

   TransformText() is an easy interface to the regular expression
   routines included in most Unix kernels. It allows you to use text
   manipulation and replacing operations with a grace similar to
   sed(1) and perl(1).

   The regular expression language is described in the re_format(2)
   man file in great detail.

   RETURNS: TransformText() will return one of the following codes,
   indicating the success or failure of the transformation:

   TEXT_REGEX_OK: Success.

   TEXT_REGEX_ERROR: This error occurs if TransformText() failed to
   compile the given regular expression or if the regular expression
   didn't specify any submatches -- what is syntactically correct, but
   useless for this routine.

   TEXT_REGEX_TRANSFORM_DIDNT_MATCH: This returncode indicates that
   the provided regular expression did not match the text buffer.

   EXAMPLE:

   The following call will remove all whitespace at the begining and
   the end of the string contained in 'buf' and place the result back
   in the same variable:

       TransformText(buf, buf, "^[\t ]*(.*)[\t ]*$", "\\\\1");

   This practice is safe in this case, because the result string is
   guaranteed to be of equal length of shorter than the original. If
   this is not the case you must use a seperate target buffer or you
   will mess your string and buffers up badly.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

int
text_transform_text(char *          dst_buffer,   /* Where to save the resulting string. */
		    const char *    src_buffer,   /* Text to transform. */
		    const char *    regex,        /* Regex to describe what matches. */
		    const char *    rule)         /* How the result should look. */
{
    regex_t       preg;
    regmatch_t    pmatch[MAX_TRANSFORM_ELEMENTS];
    char          error_msg[256];
    int           rc;
    unsigned int  i, j;
    const char *  src_p;
    char *        dst_p;

    /* Compile the regular expression. */

    rc = regcomp(&preg, regex, REG_EXTENDED | REG_ICASE);
    if (rc != 0) {
	regfree(&preg);
	return TEXT_REGEX_ERROR;
    }
    if (preg.re_nsub <= 0) {
	regfree(&preg);
	return TEXT_REGEX_ERROR;
    }

    /* Build the matching array. */

    rc = regexec(&preg, src_buffer, MAX_TRANSFORM_ELEMENTS, pmatch, 0);
    if (rc != 0) {
	if (rc == REG_NOMATCH) {
	    regfree(&preg);
	    return TEXT_REGEX_TRANSFORM_DIDNT_MATCH;
	}
	else {
	    regerror(rc, &preg, error_msg, (size_t) sizeof(error_msg));
	    regfree(&preg);
	    return TEXT_REGEX_ERROR;
	}
    }

    /* Do the transformation. */

    src_p = rule;
    dst_p = dst_buffer;
    do {
	switch (*src_p) {
	  case '\\':		/* Handle backslash squences. */
	      src_p++;
	      switch (*src_p) {
		case '0': case '1': case '2':
		case '3': case '4': case '5':
		case '6': case '7': case '8':
		case '9':	/* Substitute appropriate match. */
		    i = *src_p - '0';
		    for (j = pmatch[i].rm_so; j < pmatch[i].rm_eo; j++)
		      *dst_p++ = src_buffer[j];
		    src_p++;
		    break;
		case '\\':	/* Copy bashslash verbatim. */
		    *dst_p++ = *src_p++;
		    break;
		default:	/* Copy verbatim and warn about unknown sequence. */
		    *dst_p++ = *src_p++;
	      }
	      break;
	  default:
	      *dst_p++ = *src_p++;
	}
    } while (*src_p != '\0');
    *dst_p = '\0';		/* Terminate string. */

    regfree(&preg);

    return TEXT_REGEX_OK;
}
