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

#include "petidomo.h"

#ifndef LOG_PERROR
#  define LOG_PERROR 0
#endif

MODULE_TABLE			/* defined in debug.h */
static char *        listname = NULL;
#ifdef DEBUG
static argv_array_t  debug;
#endif

int
main(int argc, char * argv[])
    {
    const struct PD_Config * MasterConfig;
    char *        incoming_mail;
    char *        programname;
    argv_t        args[] = {
#ifdef DEBUG
        {'d', "debug", ARGV_CHAR_P | ARGV_FLAG_ARRAY , &debug, "debug",
         "Set debug level per module."},
#endif
        {ARGV_MAYBE, 0L, ARGV_CHAR_P, &listname, "listname", "Default mailing list."},
        {ARGV_LAST}
    };
    int           fd;

    /* Determine the name we have been called under. */

    programname = strrchr(argv[0], (int) '/');
    if (programname == NULL)
      programname = argv[0];
    else
      programname++;

    /* Init logging routines first of all, so that we can report
       errors. */

    openlog(programname, LOG_CONS | LOG_PID | LOG_PERROR, LOG_MAIL);

    /* Set our umask. */

    umask(S_IRWXO);		/* We don't care for "others". */

    /* Switch real and effective uid/gid to 'petidomo'. */

#ifdef HAVE_SETREUID
    setreuid(geteuid(), geteuid());
#endif
#ifdef HAVE_SETREGID
    setregid(getegid(), getegid());
#endif

    /* Parse the command line. */

    argv_help_string = "OpenPetidomo Mailing List Server";
    argv_version_string = "OpenPetidomo";
    argv_process(args, argc, argv);

    /* Set debug level according to the wishes of the user. */

#ifdef DEBUG
    if (argvSetDebugLevel(debug) != 0)
      exit(1);
#endif

    /* Init Petidomo's internal stuff. */

    if (InitPetidomo() != 0) {
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
    RescueMail(incoming_mail);

    /* Detach ourselves, if the configurator wished it so. */

    if (MasterConfig->detach == TRUE) {
	debug((DEBUG_MAIN, 3, "Detaching from control terminal and running asyncronously."));
        switch (fork()) {
	  case -1:
	      syslog(LOG_CRIT, "Can't fork(): %m");
	      exit(1);
	  case 0:
	      setsid();
	      fd = open("/dev/null", O_RDWR, 0);
	      if (fd != -1) {
		  dup2(fd, STDIN_FILENO);
		  dup2(fd, STDOUT_FILENO);
		  dup2(fd, STDERR_FILENO);
		  if (fd > 2)
		    close (fd);
	      }
	      break;
	  default:
	      _exit(0);
        }
    }

    /* Now decide what we actually do with the mail. */

    if (strcasecmp("listserv", programname) == 0)
	listserv_main(incoming_mail, listname);
    else if (strcasecmp("hermes", programname) == 0)
	{
	if (listname != NULL)
	    hermes_main(incoming_mail, listname);
	else
	    {
	    syslog(LOG_ERR, "Wrong command line syntax. \"hermes\" requires a parameter.");
	    exit(1);
	    }
	}
    else if (strcasecmp("petidomo", programname) == 0)
	{
	/* do nothing */
	}
    else
	{
	syslog(LOG_ERR, "I have been called under an unknown name \"%s\".", programname);
	exit(1);
	}

    /* Exit gracefully. */

    RemoveRescueMail();
    return 0;
    }
