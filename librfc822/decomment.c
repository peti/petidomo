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
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"

static const char *
read_until_next_quote(const char * p)
{
    while (*p) {
	if (*p == '"') {
	    break;
	}
	if (*p == '\\' && p[1] != '\0') {
	    p += 2;
	    continue;
	}
	p++;
    }
    return p;
}

static const char *
read_until_close_bracket(const char * p)
{
    while (*p) {
	if (*p == ')') {
	    break;
	}
	if (*p == '(') {
	    p = read_until_close_bracket(p+1);
	    if (*p == '\0')
	      break;
	    else {
		p++;
		continue;
	    }
	}
	else if (*p == '\\' && p[1] != '\0') {
	    p += 2;
	    continue;
	}
	else if (*p == '"') {
	    p = read_until_next_quote(p+1);
	    if (*p == '\0')
	      break;
	    else {
		p++;
		continue;
	    }
	}
	p++;
    }
    return p;
}

/* remove all comments from an rfc822 address

   This routine takes as argument an address string conforming to the
   RFC822 syntax and removes all comments from it, returning the
   result in the pointer specified as second argument.

   This is very useful if you want to parse the address with a context
   free grammer parser, like yacc, because recognizing the various
   forms of commenting with a BNF grammar is very complicated.

   RETURNS: The routine returns an integer describing whether the
   decommenting process succeeded or not. In case of an error, the
   return code described the reason for failure.

   RFC822_OK: Success.

   RFC822_FATAL_ERROR: A fatal system error occured, like malloc()
   failed. The global errno variable will be set accordingly.

   RFC822_UNCLOSED_COMMENT: A "(comment)"-like expression was not
   closed properly.

   RFC822_UNMATCHED_CLOSE_BRACKET: A close bracket (")") was
   encountered outside the comment context.

   RFC822_UNCLOSED_QUOTE: A quoted `"string"'-like expression was not
   closed properly.

   RFC822_UNCLOSED_ANGEL_BRACKET: An "<address>"-like expression was
   not closed properly.

   RFC822_NESTED_ANGEL_BRACKET: An open angel bracket ("<") was
   encountered inside the "<address>" context.

   RFC822_UNMATCHED_CLOSE_ANGEL_BRACKET: A close angel bracket (">") was
   encountered outside the "<address>" context.

   RFC822_SYNTAX_ERROR: The address does not follow the rfc822 syntax.

   NOTE: The returned by rfc822_decomment() has been allocated with
   malloc(3) and should be freed when not needed anymore.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

int
rfc822_decomment(const char *  source	/* String containing a formatted rfc822 address. */,
		 char **   destination  /* Location where to store the parsed string. */)
{
    char *   buffer,
	 *   address_start,
	 *   p;
    const char * s;
    int      in_quote,
	     angel_bracket_count;


    /* Sanity checks. */

    assert(source != NULL);
    if (!source || *source == '\0') {
	errno = EINVAL;
	return 0;
    }

    /* Allocate buffer. */

    buffer = malloc(strlen(source)+1);
    if (buffer == NULL) {
	errno = ENOMEM;
	return RFC822_FATAL_ERROR;
    }

    /* Let's parse it. */

    address_start = p = buffer;
    s = source;
    in_quote = 0;
    angel_bracket_count = 0;

    while(*s) {

	if (*s == '"') {
	    in_quote = !in_quote;
	}
	else if (!in_quote && *s == '(') {
	    s = read_until_close_bracket(s+1);
	    if (*s == '\0') {
		free(buffer);
		return RFC822_UNCLOSED_COMMENT;
	    }
	    s++;
	    *p++ = ' ';
	    continue;
	}
	else if (!in_quote && *s == ')') {
	    free(buffer);
	    return RFC822_UNMATCHED_CLOSE_BRACKET;
	}
	else if (!in_quote && *s == '<') {
	    if (angel_bracket_count > 0) {
		free(buffer);
		return RFC822_NESTED_ANGEL_BRACKET;
	    }
	    s++;
	    p = address_start;
	    *p = '\0';
	    angel_bracket_count++;
	    continue;
	}
	else if (!in_quote && *s == '>') {
	    if (angel_bracket_count == 0) {
		free(buffer);
		return RFC822_UNMATCHED_CLOSE_ANGEL_BRACKET;
	    }
	    *p = '\0';
	    angel_bracket_count--;
	    break;
	}
	else if (*s == '\\') {
	    if (s[1] != '\0') {
		*p++ = *s++;
	    }
	    else {
		free(buffer);
		return RFC822_SYNTAX_ERROR;
	    }
	}


	*p++ = *s++;
    }
    *p = '\0';
    if (in_quote) {
	free(buffer);
	return RFC822_UNCLOSED_QUOTE;
    }

    if (angel_bracket_count > 0) {
	free(buffer);
	return RFC822_UNCLOSED_ANGEL_BRACKET;
    }


    *destination = buffer;
    return RFC822_OK;
}
