/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1995 by Gray Watson <gray.watson@letters.com>
 */

#ifndef __LIB_ARGV_H__
#define __LIB_ARGV_H__ 1

#include <stdio.h>

/********** Prototypes **********/

#define ARGV_PNT	void *

typedef struct {
  char		ar_short_arg;		/* the char of the arg, 'd' if '-d' */
  char		*ar_long_arg;		/* long version of arg, 'delete' */
  short		ar_type;		/* type of option, see values below */
  ARGV_PNT	ar_variable;		/* address of variable that is arg */
  char		*ar_var_label;		/* label for variable descriptions */
  char		*ar_comment;		/* comment for usage message */
} argv_t;

typedef struct {
  int		aa_entryn;		/* number of elements in aa_entrees */
  ARGV_PNT	aa_entries;		/* entry list specified */
} argv_array_t;

#define ARGV_ARRAY_COUNT(array)		((array).aa_entryn)
#define ARGV_ARRAY_ENTRY(array, type, which)	\
	(((type *)(array).aa_entries)[which])
#define ARGV_LAST	((char)255)
#define ARGV_MAND	((char)254)
#define ARGV_MAYBE	((char)253)
#define ARGV_OR		((char)252)
#define ARGV_ONE_OF	((char)251)
#define ARGV_XOR	((char)251)
#define ARGV_BOOL	1		/* boolean type, sets to ARGV_TRUE */
#define ARGV_BOOL_NEG	2		/* like bool but sets to ARGV_FALSE */
#define ARGV_BOOL_ARG	3		/* like bool but takes a yes/no arg */
#define ARGV_CHAR	4		/* single character */
#define ARGV_CHARP	5		/* same as STRING */
#define ARGV_STRING	5		/* character string */
#define ARGV_FLOAT	6		/* floating pointer number */
#define ARGV_SHORT	7		/* integer number */
#define ARGV_INT	8		/* integer number */
#define ARGV_U_INT	9		/* unsigned integer number */
#define ARGV_LONG	10		/* long integer number */
#define ARGV_U_LONG	11		/* unsinged long integer number */
#define ARGV_BIN	12		/* binary number (0s and 1s) */
#define ARGV_OCT	13		/* octal number, (base 8) */
#define ARGV_HEX	14		/* hexadecimal number, (base 16) */
#define ARGV_INCR	15		/* int arg which gets ++ each time */
#define ARGV_TYPE(t)	((t) & 0x3F)	/* strip off all but the var type */
#define ARGV_ARRAY	(1 << 14)	/* OR with type to indicate array */
#define ARGV_USAGE_SHORT	1	/* print short usage messages */
#define ARGV_USAGE_LONG		2	/* print long-format usage messages */
#define ARGV_USAGE_DEFAULT	3	/* default usage messages */
#define ARGV_FALSE		0
#define ARGV_TRUE		1

#ifdef __cplusplus
extern "C" {
#endif

extern	char	argv_program[/* PROGRAM_NAME + 1 */];
extern	char	**argv_argv;
extern	int	argv_argc;
extern	char	*argv_help_string;
extern	char	*argv_version_string;
extern	char 	argv_interactive;
extern	FILE 	*argv_error_stream;
extern	int	argv_process(argv_t *args, const int argc, char **argv);
extern	int	argv_web_process_string(argv_t *args, const char *arg0,
					const char *string,
					const char *delim);
extern	int	argv_web_process(argv_t *args, const char *arg0);
extern	int	argv_usage(const argv_t *args, const int which);
extern	int	argv_was_used(const argv_t *args, const char arg);
extern	void	argv_cleanup(const argv_t *args);
extern	int	argv_copy_args(char *buf, const int max_size);

#ifdef __cplusplus
}
#endif

#endif /* !__LIB_ARGV_H__ */
