/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1996-99 by Peter Simons <simons@cys.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Peter Simons.
 *
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
