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

%{
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"
#include "address.h"

extern int     yylval;

char *         rfc822_address_buffer;
int            rfc822_address_buffer_pos;

#define YY_INPUT(buf,result,max_size) { \
    buf[0] = rfc822_address_buffer[rfc822_address_buffer_pos++]; \
    result = ((buf[0] != '\0') ? 1 : YY_NULL); \
}
%}
%x quoted escaped escaped2

%option 8bit
%option batch
%option case-insensitive
%option nounput
%option outfile="lex.yy.c"
%option prefix="rfc822_"
%option warn
%option noyywrap

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
