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

#include <stdlib.h>
#include <string.h>
#include "argv.h"

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
argvSetDebugLevel(argv_array_t debug)
    {
#ifdef DEBUG
    extern const char* const ModuleTable[];
    void setDebugLevel(unsigned short, unsigned short);

    char *         ModuleName;
    char *         DebugLevel;
    unsigned int   count, i;

    for (count = 0; count < debug.aa_entry_n; count++)
	{
        ModuleName = strtok(ARGV_ARRAY_ENTRY(debug, char *, count), ",/:");
        DebugLevel = strtok(NULL, ",/:");
        if (ModuleName == NULL || DebugLevel == NULL || atoi(DebugLevel) < 0 || atoi(DebugLevel) > 9)
	    {
            fprintf(stderr, "\"%s\" is not a valid debug-level specification.\n",
                    ARGV_ARRAY_ENTRY(debug, char *, count));
            return -1;
	    }
        for (i = 0; ModuleTable[i] != NULL; i++)
	    {
            if (!strcasecmp(ModuleName, ModuleTable[i]))
		{
                setDebugLevel(i, atoi(DebugLevel));
                break;
		}
	    }
        if (ModuleTable[i] == NULL)
	    {
	    fprintf(stderr, "\"%s\" is not a valid debug-module name.\n", ModuleName);
	    return -1;
	    }
	}
#endif
    return 0;
    }
