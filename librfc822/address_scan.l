/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

%{
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"
#include "address_scan.h"
#define yylval rfc822_lval

extern int     yylval;

char *         rfc822_address_buffer;
int            rfc822_address_buffer_pos;

#define YY_INPUT(buf,result,max_size) { \
    buf[0] = rfc822_address_buffer[rfc822_address_buffer_pos++]; \
    result = ((buf[0] != '\0') ? 1 : YY_NULL); \
}
%}
%x quoted escaped escaped2
%%
([^@\.\(\) \t<>\":,\\]*\\)/(.[@\.:])	{ BEGIN(escaped2); yymore(); }
([^@\.\(\) \t<>\":,\\]*\\)/(.)		{ BEGIN(escaped2); yymore(); }
([^@\.\(\) \t<>\":,\\]*\\)/(.[^@\.:])	{ BEGIN(escaped); yymore(); }
([^@\.\(\) \t<>\":,\\]*\\)		{ return TOK_ILLEGAL; }
[^@\.\(\) \t<>\":,\\]+		        { return TOK_ATOM; }

[ \t]+				/* eat up whitespace */
[@\.:]				{ yylval = yytext[0]; return yylval; }
[<>\(\),:]			{ return TOK_ILLEGAL; }

<escaped>.			{ BEGIN(INITIAL); yymore(); }
<escaped2>.			{ BEGIN(INITIAL); return TOK_ATOM; }

\"				{ BEGIN(quoted); yymore(); }
<quoted>[^\"\\]+		{ yymore(); }
<quoted>\\\"			{ yymore(); }
<quoted>\"			{ BEGIN(INITIAL); return TOK_ATOM; }
<quoted><<EOF>>			{ BEGIN(INITIAL); return TOK_ILLEGAL; }
%%
/* Internal routines. */

int
yywrap(void)
{
    return 1;
}
