/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996,97 CyberSolutions GmbH.
 *      All rights reserved.
 *
 *      - Set debug-level for various modules from the command line.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <argv.h>

#ifdef DEBUG

extern const char * const ModuleTable[];

/* This routine is a convenient way to enable or disable various
   debugging modules according to the wishes of the user. It takes a
   result array from argv_process(3), parses the contents and set the
   debug level of the specified modules accordingly.

   The provided array must contain zero or more options describing how
   the debug level should be set. Each string must be of the form
   'module,level', 'module/level' or 'module:level', for example:
   'rfcparse,5'. argvSetDebugLevel() will then set the debug level of
   the module 'rfcparse' to '5'.

   In order to be able to map the name of a debug module to the
   according internal module number, argvSetDebugLevel() expects an
   array string pointers of the name 'ModuleTable', which lists all
   available module names in the order in which they have been
   assigned an id number. This array will typically be provided by the
   main() routine of the caller.

   RETURNS: In case of an error, -1 is returned. A return code of 0
   indicates success.

   REQUIRES: extern const char * const ModuleTable[]
*/

int
argvSetDebugLevel(argv_array_t debug /* parameter array as returned by the argv routines */
		  )
{
    char *         ModuleName;
    char *         DebugLevel;
    unsigned int   count, i;

    for (count = 0; count < debug.aa_entryn; count++) {
        ModuleName = strtok(ARGV_ARRAY_ENTRY(debug, char *, count), ",/:");
        DebugLevel = strtok(NULL, ",/:");
        if (ModuleName == NULL || DebugLevel == NULL
            || atoi(DebugLevel) < 0 || atoi(DebugLevel) > 9) {
            fprintf(stderr, "\"%s\" is not a valid debug-level specification.\n",
                    ARGV_ARRAY_ENTRY(debug, char *, count));
            return -1;
        }
        for (i = 0; ModuleTable[i] != NULL; i++) {
            if (!strcasecmp(ModuleName, ModuleTable[i])) {
                setDebugLevel(i, atoi(DebugLevel));
                break;
            }
        }
        if (ModuleTable[i] == NULL) {
            fprintf(stderr, "\"%s\" is not a valid debug-module name.\n", ModuleName);
            return -1;
        }
    }
    return 0;
}
#endif
