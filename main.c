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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "libargv/argv.h"
#include "petidomo.h"

#ifndef LOG_PERROR
#  define LOG_PERROR 0
#endif

static char*  listname = NULL;
static char*  mode = NULL;
static char*  masterconfig_path = SYSCONFDIR "/petidomo.conf";
char          g_is_approved = ARGV_FALSE;

int
main(int argc, char * argv[])
    {
    const struct PD_Config * MasterConfig;
    char *        incoming_mail;
    argv_t        args[] =
	{
        {ARGV_MAND, "mode", ARGV_CHAR_P, &mode, "mode", "listserv, deliver, or approve."},
        {ARGV_MAYBE, "listname", ARGV_CHAR_P, &listname, "listname", "Default mailing list."},
        {ARGV_MAYBE, "masterconf", ARGV_CHAR_P, &masterconfig_path, "masterconf", "Path to petidomo.conf."},
        {ARGV_MAYBE, "approved", ARGV_BOOL, &g_is_approved, "approved", "approved flag."},
        {ARGV_LAST}
	};

    /* Init logging routines first of all, so that we can report
       errors. */

    openlog("petidomo", LOG_CONS | LOG_PID | LOG_PERROR, LOG_MAIL);

    /* Parse the command line. */

    argv_help_string = "OpenPetidomo Mailing List Server";
    argv_version_string = "OpenPetidomo";
    argv_process(args, argc, argv);

    /* Init Petidomo's internal stuff. */

    if (InitPetidomo(masterconfig_path) != 0) {
	syslog(LOG_CRIT, "Failed to initialize my internals.");
	exit(1);
    }
    MasterConfig = getMasterConfig();

    /* Load the file from standard input and save it, so that it isn't
       lost in case of an error. */

    incoming_mail = LoadFromDescriptor(STDIN_FILENO);
    if (incoming_mail == NULL) {
	syslog(LOG_ERR, "Failed to read incoming mail from standard input.");
	exit(1);
    }

    /* Now decide what we actually do with the mail. */

    if (strcasecmp("listserv", mode) == 0)
	listserv_main(incoming_mail, listname);
    else if (strcasecmp("deliver", mode) == 0)
	{
	if (listname != NULL)
	    hermes_main(incoming_mail, listname);
	else
	    {
	    syslog(LOG_ERR, "Wrong command line syntax; deliver mode requires a parameter.");
	    exit(1);
	    }
	}
    else if (strcasecmp("approve", mode) == 0)
	{
	approve_main(incoming_mail);
	}
    else
	{
	syslog(LOG_ERR, "I don't know anything about mode \"%s\".", mode);
	exit(1);
	}

    /* Exit gracefully. */

    return 0;
    }
