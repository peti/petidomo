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

#ifdef __cplusplus
}   // end extern "C"
#endif

#endif /* !__LIB_TEXT_H__ */
