/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1999 by Gray Watson <gray.watson@letters.com>.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Gray Watson not be used in advertising
 * or publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

#ifndef __ARGV_LOC_H__
#define __ARGV_LOC_H__

#include "argv.h"			/* to get the types */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * generic constants
 */
/* special consideration for NULL.  some compilers bitch if I redefine it */
#ifndef NULL
#define NULL		0L
#endif

#undef	MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))

#undef NOERROR
#define NOERROR		0

#undef ERROR
#define ERROR		(-1)

#undef STDIN
#define STDIN		0

/*
 * local argv defines
 */

#define ERROR_STREAM_INIT	(FILE *)0x1	/* hack to init stderr FILE* */
#define NO_VALUE		(-1)		/* no mandatory args value */
#define ARRAY_INCR		10		/* increment by 10 every 10 */
#define SCREEN_WIDTH		79		/* width of screen till wrap */
#define BITS_IN_BYTE		8		/* bits in a byte */
#define SPECIAL_CHARS		"e\033^^\"\"''\\\\n\nr\rt\tb\bf\fa\007"
#define DUMP_SPACE_BUF		128		/* space for memory dump */
#define ARG_MALLOC_INCR		20		/* alloc in 10 increments */
#define FILE_LINE_SIZE		1024		/* max size of file lines */

/* internal flags set in the ar_type field */
/* NOTE: other external flags defined in argv.h */
#define ARGV_FLAG_USED		(1 << 12)	/* if arg has been specified */

/* error messages */
#define USAGE_ERROR_NAME	"usage problem"
#define INTERNAL_ERROR_NAME	"internal argv error"

/*
 * settings to vary the output of the argument routines
 */

#define PROGRAM_NAME		256		/* max size of program name */
#define EXIT_CODE		1		/* argv exit code for errors */

/* global env settings */
#define GLOBAL_NAME		"GLOBAL_ARGV"	/* global argv env name */
#define GLOBAL_CLOSE		"close="	/* close arg setting */
#define GLOBAL_ENV		"env="		/* env setting */
#define GLOBAL_ERROR		"error="	/* error setting */
#define GLOBAL_MULTI		"multi="	/* multi setting */
#define GLOBAL_USAGE		"usage="	/* usage setting */
#define GLOBAL_LASTTOG		"lasttog="	/* last-arg toggle */

#define GLOBAL_CLOSE_DISABLE	1		/* disable close args */
#define GLOBAL_CLOSE_ENABLE	2		/* enable close args */

#define GLOBAL_ENV_NONE		3		/* no env processing */
#define GLOBAL_ENV_BEFORE	4		/* process env before args */
#define GLOBAL_ENV_AFTER	5		/* process env after args */

#define GLOBAL_USAGE_SEE	6		/* see messages (special) */
#define GLOBAL_USAGE_SHORT	7		/* print short */
#define GLOBAL_USAGE_SHORTREM	8		/* short + remind */
#define GLOBAL_USAGE_LONG	9		/* long */
#define GLOBAL_USAGE_ALL	10		/* all */

#define GLOBAL_MULTI_REJECT	11		/* reject multiple arg use */
#define GLOBAL_MULTI_ACCEPT	12		/* accept multiple arg use */

#define GLOBAL_LASTTOG_DISABLE	13		/* toggling of last-arg off */
#define GLOBAL_LASTTOG_ENABLE	14		/* toggling of last-arg on */

#define GLOBAL_ERROR_NONE	15		/* print only error */
#define GLOBAL_ERROR_SEE	GLOBAL_USAGE_SEE /* error + see messages */
#define GLOBAL_ERROR_SHORT	GLOBAL_USAGE_SHORT /* error + short */
#define GLOBAL_ERROR_SHORTREM	GLOBAL_USAGE_SHORTREM /* err + short + remind*/
#define GLOBAL_ERROR_LONG	GLOBAL_USAGE_LONG /* err + long */
#define GLOBAL_ERROR_ALL	GLOBAL_USAGE_ALL /* err + all */

/* special argument definitions */
#define LAST_ARG		"--"		/* arg to mark end of args */
#define LONG_PREFIX		"--"		/* prefix for long args */
#define SHORT_PREFIX		"-"		/* prefix for short args */
#define UNKNOWN_ARG		"??"		/* unknown argument output */
#define ARG_EQUALS		'='		/* to assign value to option */
#define NUMBER_ARG_CHARS	"0123456789+-."	/* characters in numbers */

/* how to produce the env var using sprintf and the argv_program variable */
#define ENVIRON_FORMAT		"ARGV_%s"

/* special short-argument strings */
#define USAGE_CHAR_ARG		'?'		/* default short-opt usage */

/* special long-argument strings */
#define DISPLAY_ARG		"argv-display"	/* display arg variable vals */
#define FILE_ARG		"argv-file"	/* read args from file */
#define HELP_ARG		"help"		/* default help option */
#define USAGE_ARG		"usage"		/* default usage option */
#define USAGE_SHORT_ARG		"usage-short"	/* default usage-short opt */
#define USAGE_LONG_ARG		"usage-long"	/* default usage-long opt */
#define USAGE_ALL_ARG		"usage-all"	/* default usage-all opt */
#define VERSION_ARG		"version"	/* default version option */

/* spacing on line for usage messages */
#define SHORT_COLUMN		2	/* spaces to indent for short-args */
#define LONG_COLUMN		20	/* column for long options */
#define COMMENT_COLUMN		45	/* column for comments */

/* some in-line "labels" for comments */
#define USAGE_LABEL		"Usage: "	/* usage message start */
#define VERSION_LABEL		"= "		/* default:no version prefix */
#define LONG_LABEL		"or "		/* put before long-option */
#define COMMENT_LABEL		""		/* put before comments */
#define ARRAY_LABEL		" array"	/* put after displayed type */
#define BOOL_ARG_LABEL		"yes|no"	/* label for bool-arg arg */

/* some sizeof defines */
#define SHORT_PREFIX_LENGTH	(sizeof(SHORT_PREFIX) - 1)
#define LONG_PREFIX_LENGTH	(sizeof(LONG_PREFIX) - 1)

#define USAGE_LABEL_LENGTH	(sizeof(USAGE_LABEL) - 1)
#define COMMENT_LABEL_LENGTH	(sizeof(COMMENT_LABEL) - 1)
#define LONG_LABEL_LENGTH	(sizeof(LONG_LABEL) - 1)
#define UNKNOWN_ARG_LENGTH	(sizeof(UNKNOWN_ARG) - 1)
#define BOOL_ARG_LENGTH		(sizeof(BOOL_ARG_LABEL) - 1)

#define HAS_ARG(type)	(! (ARGV_TYPE(type) == ARGV_BOOL \
			    || ARGV_TYPE(type) == ARGV_BOOL_NEG \
			    || ARGV_TYPE(type) == ARGV_INCR))

/******************************** argv types *********************************/

/* strcture defining argv types */
typedef struct {
  unsigned int	at_value;		/* value of the type */
  char		*at_name;		/* name of the type */
  unsigned int	at_size;		/* size of type */
  char		*at_desc;		/* description of the type */
} argv_type_t;

static	argv_type_t	argv_types[] = {
  { ARGV_BOOL,		"flag",			sizeof(char),
      "if option used, set variable to true" },
  { ARGV_BOOL_NEG,	"negative flag",	sizeof(int),
      "if option used, set variable to false" },
  { ARGV_BOOL_ARG,	"flag with arg",	sizeof(char),
      "like flag but you specify with yes/no argument" },
  { ARGV_CHAR,		"character",		sizeof(char),
      "single character" },
  { ARGV_CHAR_P,	"string",		sizeof(char *),
      "multiple characters terminated with a '\\0'" },
  { ARGV_SHORT,		"short integer",	sizeof(short),
      "decimal short-sized integer value" },
  { ARGV_U_SHORT,	"unsigned short integer", sizeof(unsigned short),
      "decimal unsigned short-sized integer value" },
  { ARGV_INT,		"integer",		sizeof(int),
      "decimal integer value" },
  { ARGV_U_INT,		"unsigned integer",	sizeof(unsigned int),
      "decimal unsigned integer value" },
  { ARGV_LONG,		"long integer",		sizeof(long),
      "decimal long-sized integer value" },
  { ARGV_U_LONG,	"unsigned long",	sizeof(unsigned long),
      "decimal unsigned long-sized integer value" },
  { ARGV_FLOAT,		"floating point",	sizeof(float),
      "real number with decimal point" },
  { ARGV_DOUBLE,	"double floating point", sizeof(double),
      "double precision real number with decimal point" },
  { ARGV_BIN,		"binary",		sizeof(int),
      "base 2 value with digits of 0 or 1" },
  { ARGV_OCT,		"octal",		sizeof(int),
      "base 8 value with digits from 0-7" },
  { ARGV_HEX,		"hexadecimal",		sizeof(int),
      "base 16 value with digits from 0-9, A-F" },
  { ARGV_INCR,		"increment",		sizeof(int),
      "increment variable each time option used" },
  { ARGV_SIZE,		"long size",		sizeof(long),
      "size as long int + [bkmg] b=byte,k=kilo,m=meg,g=gig" },
  { ARGV_U_SIZE,	"unsigned long size",	sizeof(unsigned long),
      "size as unsigned long int + [bkmg] b=byte,k=kilo,m=meg,g=gig" },
  { 0 }
};

#ifdef __cplusplus
}
#endif

#endif /* ! __ARGV_LOC_H__ */