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

#ifndef __LIB_TEXT_H__
#define __LIB_TEXT_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

/********** Useful defines and declarations **********/

#ifndef __cplusplus
#ifndef __HAVE_DEFINED_BOOL__
#  define __HAVE_DEFINED_BOOL__ 1
typedef int bool;
#endif

#ifndef FALSE
#  define FALSE (0==1)
#endif
#ifndef TRUE
#  define TRUE (1==1)
#endif
#endif
enum {
    TEXT_REGEX_OK = 0,
    TEXT_REGEX_ERROR,
    TEXT_REGEX_TRANSFORM_DIDNT_MATCH
};

/********** Structures **********/


/********** Prototypes **********/

int      text_transform_text(char *, const char *, const char *, const char *);
bool     text_easy_pattern_match(const char * buffer, const char * pattern);
void     text_wordwrap(char * buffer, unsigned int line_len);
char *   text_easy_sprintf(const char * fmt, ...);
char *   text_find_next_line(char *);
char *   text_find_string(char * buffer, char * string);

#endif /* !__LIB_TEXT_H__ */
