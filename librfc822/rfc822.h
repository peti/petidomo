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

#ifndef __LIB_RFC822_H__
#define __LIB_RFC822_H__ 1

struct rfc822_address_sep_state {
    char *        address_line;
    unsigned int  group_nest;
};

enum {
    RFC822_FATAL_ERROR = -1,
    RFC822_OK,
    RFC822_UNCLOSED_COMMENT,
    RFC822_UNMATCHED_CLOSE_BRACKET,
    RFC822_UNCLOSED_QUOTE,
    RFC822_UNCLOSED_ANGEL_BRACKET,
    RFC822_NESTED_ANGEL_BRACKET,
    RFC822_UNMATCHED_CLOSE_ANGEL_BRACKET,
    RFC822_SYNTAX_ERROR
};

int     rfc822_decomment(const char *, char **);
int     rfc822_parse_address(const char *, char **, char **, char **);
char *  rfc822_address_sep(struct rfc822_address_sep_state *);

#endif /* !__LIB_RFC822_H__ */

