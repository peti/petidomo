/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include "argv.h"

/* These variables should be global/static because the ANSI C standard
   doesn't allow to reference to local variables at compile time, as
   we do when filling out the args[] array. */

static char *         filename = "/tmp/defaultfile";
static char           do_something = ARGV_TRUE;
static argv_array_t   parameters;

int
main(int argc, char ** argv)
{
    argv_t            args[] = {
	{ 's', "something", ARGV_BOOL_ARG, &do_something, NULL,
	  "Do something?"},
	{ 0, "filename", ARGV_CHARP, &filename, "logfile",
	  "Path of the logfile."},
	{ ARGV_MAND, 0L, ARGV_CHARP | ARGV_ARRAY, &parameters, "parameters",
	  "Whatever..."},
	{ARGV_LAST}
    };
    unsigned int   i;

    /* Parse the command line. */

    argv_help_string = "BigBrother Internet Surveillance Daemon";
    argv_version_string = "libargv test programm";
    argv_process(args, argc, argv);

    /* Print results. */

    printf("\n");
    printf("logfile     : %s\n", filename);
    printf("do_something: %s\n", (do_something == ARGV_TRUE) ? "yes" : "no");
    printf("parameters  : ");
    for (i = 0; i < parameters.aa_entryn; i++) {
	if (i+1 < parameters.aa_entryn)
	  printf("'%s', ", ARGV_ARRAY_ENTRY(parameters, char *, i));
	else
	  printf("'%s'", ARGV_ARRAY_ENTRY(parameters, char *, i));
    }
    printf("\n");

    /* Exit gracefully. */

    argv_cleanup(args);
    return 0;
}
