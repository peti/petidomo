/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
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

