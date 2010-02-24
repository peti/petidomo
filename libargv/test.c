/*
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

#include <progname.h>
#include "argv.h"

int
main(int argc, char ** argv)
{
   /* These variables should be static because the ANSI C standard
      doesn't allow to reference to local variables at compile time,
      as would need it when filling out the args[] array. */

    static char *         filename = "/tmp/defaultfile";
    static char           do_something = ARGV_TRUE;
    static argv_array_t   parameters;

    argv_t args[] = {
     { 's', "something", ARGV_BOOL_ARG, &do_something, NULL, "Do something?"},
     { 0, "filename", ARGV_CHAR_P, &filename, "logfile", "Path of the logfile."},
     { ARGV_MAND, 0L, ARGV_CHAR_P | ARGV_FLAG_ARRAY, &parameters, "parameters", "Whatever..."},
     {ARGV_LAST}
    };
    unsigned int i;

    set_program_name(argv[0]);

    /* Parse the command line. */

    argv_help_string = "You need help with a test program?";
    argv_version_string = "libargv test programm";
    argv_process(args, argc, argv);

    /* Print results. */

    printf("\n");
    printf("logfile     : %s\n", filename);
    printf("do_something: %s\n", (do_something == ARGV_TRUE) ? "yes" : "no");
    printf("parameters  : ");
    for (i = 0; i < ARGV_ARRAY_COUNT(parameters); i++) {
        if (i+1 < ARGV_ARRAY_COUNT(parameters))
          printf("'%s', ", ARGV_ARRAY_ENTRY(parameters, char *, i));
        else
          printf("'%s'", ARGV_ARRAY_ENTRY(parameters, char *, i));
    }
    printf("\n");

    /* Exit gracefully. */

    argv_cleanup(args);
    return 0;
}
