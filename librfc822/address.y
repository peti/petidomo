/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

%{
    /* Definitions we need in the parser. */

#include <errno.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "../libmpools/mpools.h"
#define yytext rfc822_text
#define yyley rfc822_lex

    /* Variables in our lexer. */

extern char *   rfc822_address_buffer;
extern int      rfc822_address_buffer_pos;
extern char *   yytext;

    /* Our static/global variables. */

static int     no_memory_flag;
static char *  poolname,
            *  result_hostpart,
            *  result_localpart,
	    *  result_address;

    /* Prototypes for internal functions. */

static int     yyparse(void);
static int     yyerror(char *);
static char *  pool_strdup(char *);
static char *  pool_strjoin(char *, char *);
int            yylex(void);

    /* These macros call the routines we define later in a fail safe
       way. */

#define str_dup(target,a) { \
    target = (YYSTYPE) pool_strdup((char *) a); \
    if (target == NULL) { \
        no_memory_flag++; \
        YYABORT; \
    } \
}

#define str_join(target,a,b) { \
    target = (YYSTYPE) pool_strjoin((char *) a, (char *) b); \
    if (target == NULL) { \
        no_memory_flag++; \
        YYABORT; \
    } \
}

%}
%token TOK_ATOM TOK_ILLEGAL
%left ':'
%left '@'
%%

address:   local			{
					  result_localpart = (char *)$1;
					  str_dup($$,$1);
					  result_address = (char *)$$;
					}
	 | local at domain		{
					  result_localpart = (char *)$1;
					  result_hostpart = (char *)$3;
					  str_join($$,$1,$2); str_join($$,$$,$3);
					  result_address = (char *)$$;
					}
	 | at domain colon sourceroutings local {
					  result_hostpart = (char *)$2;
					  str_join($$,$4,$5);
					  result_localpart = (char *)$$;
					  str_join($$,$3,$$); str_join($$,$2,$$);
					  str_join($$,$1,$$);
					  result_address = (char *)$$;
					}
	 | at domain colon sourceroutings local at domain {
					  result_hostpart = (char *)$2;
					  str_join($$,$4,$5); str_join($$,$$,$6);
					  str_join($$,$$,$7);
					  result_localpart = (char *)$$;
					  str_join($$,$3,$$); str_join($$,$2,$$);
					  str_join($$,$1,$$);
					  result_address = (char *)$$;
					}
;

sourceroutings:	  /* empty */		{ $$ = (YYSTYPE) NULL; }
		| at domain colon sourceroutings {
					  str_join($$,$1,$2); str_join($$,$$,$3);
					  str_join($$,$$,$4);
					}

local:   atom				{ $$ = $1; }
       | atom local			{ str_join($$,$1,$2); }
       | dot				{ $$ = $1; }
       | dot local			{ str_join($$,$1,$2); }
;

domain:   atom				{ $$ = $1; }
	| atom dot domain		{ str_join($$,$2,$3); str_join($$,$1,$$); }
;

atom:	  TOK_ATOM			{ str_dup($$,yytext); }
dot:	  '.'				{ $$ = (YYSTYPE) "."; }
at:	  '@'				{ $$ = (YYSTYPE) "@"; }
colon:	  ':'				{ $$ = (YYSTYPE) ":"; }

%%
/***** internal routines *****/

static int
yyerror(char * string)
{
    return 0;
}

/* Do the same as strdup(3) but use the memory pool to allocate the
   memory, so that we don't lose memory. */

static char *
pool_strdup(char * string)
{
    char *   p;

    if (string == NULL)
      return NULL;

    p = mp_malloc(poolname, strlen(string) + 1);
    if (p == NULL)
      return NULL;

    strcpy(p, string);
    return p;
}

/* Allocate a new buffer (using the memory pool) and join the strings
   'a' and 'b' into it. */

static char *
pool_strjoin(char * a, char * b)
{
    char *   p;
    int      length = 0;

    if (a != NULL)
      length += strlen(a);
    if (b != NULL)
      length += strlen(b);
    if (length == 0)
      return NULL;

    p = mp_malloc(poolname, length + 2);
    if (p == NULL)
      return NULL;
    else
      *p = '\0';

    if (a)
      strcpy(p, a);
    if (b)
      strcpy(&(p[strlen(p)]), b);

    return p;
}

/****** public routines ******/

#include "parse_address.c"
