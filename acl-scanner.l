%{
/*
 * Copyright (c) 1995-2013 Peter Simons <simons@cryp.to>
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

#include "config.h"
#include <ctype.h>
#define YY_NO_INPUT
#include "acl-parser.h"
%}

%option 8bit
%option batch
%option case-insensitive
%option nodefault
%option nounput
%option outfile="lex.yy.c"
%option prefix="acl_"
%option warn
%option noyywrap

%%
^[ \t]*#.*$	/* ignore comments */
[ \t]		/* ignore whitespace */
\n		yylineno++;

if		return TOK_IF;
=		return TOK_EQUAL;
==		return TOK_EQUAL;
from		return TOK_FROM;
subject		return TOK_SUBJECT;
envelope	return TOK_ENVELOPE;
header		return TOK_HEADER;
body		return TOK_BODY;
and		return TOK_AND;
or		return TOK_OR;
not		return TOK_NOT;
then		return TOK_THEN;
match(es)?	return TOK_MATCH;
\"[^\"]*\"	{
			yytext[yyleng-1] = '\0';
			yytext++;
			yyleng -= 2;
			return TOK_STRING;
		}
drop		return TOK_DROP;
pass		return TOK_PASS;
approve		return TOK_APPROVE;
redirect	return TOK_REDIRECT;
forward		return TOK_FORWARD;
reject		return TOK_REJECT;
rejectwith	return TOK_REJECTWITH;
filter		return TOK_FILTER;

.		{ acl_lval = acl_text[0]; return acl_lval; } /* literal */
%%

void acl_reset_lexer()
{
  BEGIN(INITIAL);
}
