/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1996-99 by CyberSolutions GmbH, Germany.
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
 *      This product includes software developed by CyberSolutions GmbH.
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

#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/resource.h>
#ifndef RUSAGE_SELF
#   define RUSAGE_SELF       0
#endif
#ifndef RUSAGE_CHILDREN
#   define RUSAGE_CHILDREN  -1
#endif

#include "petidomo.h"

void
AppendSignature(FILE * fh)
{
    const struct PD_Config * MasterConfig = getMasterConfig();
    struct utsname machine_name;
    struct rusage resource_usage;

    if (MasterConfig->show_stats == TRUE) {

	/* Start with the part of the signature that never fails. */

	fflush(fh);
	fprintf(fh, "\n\n-- \n");
	fprintf(fh, " /*\n");
	fprintf(fh, "  * Listserver software: OpenPetidomo\n");

	/* Determine what machine we are. */

	if (uname(&machine_name) == 0) {
	    fprintf(fh, "  * Server hardware    : %s-%s\n",
		    machine_name.sysname,
		    machine_name.machine);
	}

	/* Determine our resource usage. */

	getrusage(RUSAGE_SELF, &resource_usage);

	fprintf(fh, "  * Utilized cpu time  : %ld.%ld seconds\n",
		resource_usage.ru_utime.tv_sec + resource_usage.ru_stime.tv_sec,
		resource_usage.ru_utime.tv_usec + resource_usage.ru_stime.tv_usec);
	fprintf(fh, "  * Utilized memory    : %ld KByte\n",
		(resource_usage.ru_idrss > 0) ? resource_usage.ru_idrss :
		(long int)sbrk(0) / 1024);

	/* Close signature. */

	fprintf(fh, "  */\n");
	fflush(fh);

    }
}
