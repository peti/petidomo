/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

   This file is part of OpenPetidomo.

   OpenPetidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   OpenPetidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
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
