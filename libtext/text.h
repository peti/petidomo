/*
 * Copyright (c) 1995-2019 Peter Simons <simons@cryp.to>
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

#ifndef __LIB_TEXT_H__
#define __LIB_TEXT_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

/********** Useful defines and declarations **********/

#ifndef __cplusplus
#ifndef PETIDOMO_HAS_DEFINED_BOOL
#  define PETIDOMO_HAS_DEFINED_BOOL 1
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

#ifdef __cplusplus
}   /* end extern "C" */
#endif

#endif /* !__LIB_TEXT_H__ */
