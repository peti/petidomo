/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996,97 by CyberSolutions GmbH.
 *      All rights reserved.
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
