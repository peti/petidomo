/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
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

#include <petidomo.h>
#include "version.h"

void
AppendSignature(FILE * fh)
{
#ifdef COMMERCIAL_VERSION
    const struct PD_Config * MasterConfig = getMasterConfig();
#endif
    struct utsname machine_name;
    struct rusage resource_usage;

#ifdef COMMERCIAL_VERSION
    char whoami[] = VERS " (commercial)";
#else
    char whoami[] = VERS " (non-commercial)";
#endif
    whoami[0] = 'P';		/* cosmetics */

#ifdef COMMERCIAL_VERSION
    if (MasterConfig->show_stats == TRUE) {
#endif

    /* Start with the part of the signature that never fails. */

    fflush(fh);
    fprintf(fh, "\n\n-- \n");
    fprintf(fh, " /*\n");
    fprintf(fh, "  * Listserver software: %s\n", whoami);

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

#ifdef COMMERCIAL_VERSION
    }
#endif
}
