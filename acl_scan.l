/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

%{
#include <ctype.h>
#include "acl_scan.h"
%}
%%
^[ \t]*#.*$	/* ignore comments */
[ \t]		/* ignore whitespace */
\n		lineno++;

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
redirect	return TOK_REDIRECT;
forward		return TOK_FORWARD;
reject		return TOK_REJECT;
rejectwith	return TOK_REJECTWITH;
filter		return TOK_FILTER;

.		{ yylval = yytext[0]; return yylval; } /* literal */
%%
