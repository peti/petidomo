/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

   This file is part of Petidomo.

   Petidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Petidomo is distributed in the hope that it will be useful, but
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
#include "libtext/text.h"
#include "petidomo.h"

#ifndef LOG_PERROR
#  define LOG_PERROR 0
#endif

static char*  listname = NULL;
static char*  mode = NULL;
static char*  masterconfig_path = SYSCONFDIR "/petidomo.conf";
char          g_is_approved = ARGV_FALSE;
const char* who_am_i;

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

    /* Store our full path and program name in who_am_I, so that
       queue_posting() and queue_command() know where to find the
       Petidomo binary. */

    if (argv[0][0] == '/')
	{
	who_am_i = argv[0];
	}
    else
	{
	char buf[4096];
	if (getcwd(buf, sizeof(buf)) == NULL)
	    {
	    syslog(LOG_CRIT, "Failed to get the path to my current working directory.");
	    exit(1);
	    }
	who_am_i = text_easy_sprintf("%s/%s", buf, argv[0]);
	}

    /* Set our real user id equal to the effective user id to avoid
       confusion in case we're started as a setuid binary. */

    setreuid(geteuid(), geteuid());

    /* Parse the command line. */

    argv_help_string = "Petidomo Mailing List Server";
    argv_version_string = "Petidomo";
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

    /* Log a few helpful facts about this Petidomo instance. */

    syslog(LOG_DEBUG, "Petidomo starting up; mode=%s, listname=%s, approved=%s, ruid=%d, euid=%d, gid=%d, egid=%s",
	   mode, listname, (g_is_approved) ? "true" : "false", getuid(), geteuid(), getgid(), getegid());

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
