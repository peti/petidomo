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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

static char * s_crash_filename = NULL;

static void
cb_crashmail(void)
{
    const struct PD_Config *  MasterConfig;
    FILE *                    fh;

    if (s_crash_filename != NULL) {
	MasterConfig = getMasterConfig();
	{
	    char *    envelope;

	    envelope = text_easy_sprintf("petidomo-manager@%s", MasterConfig->fqdn);
	    if (envelope == NULL) {
		syslog(LOG_CRIT, "Failed to allocate required internal buffer: %m");
		exit(1);
	    }
	    fh = vOpenMailer(envelope, "petidomo-manager", NULL);
	    free(envelope);
	}
	if (fh != NULL) {
	    fprintf(fh, "From: petidomo-manager (Petidomo Mailing List Server)\n");
	    fprintf(fh, "To: petidomo-manager\n");
	    fprintf(fh, "Subject: Petidomo failed -- mail has been rescued\n");
	    fprintf(fh, "\n");
	    fprintf(fh, "Due to a configuration error, Petidomo was not able to process the\n");
	    fprintf(fh, "incoming mail properly. The mail has been rescued into the following\n");
	    fprintf(fh, "file: %s/%s\n", MasterConfig->basedir, s_crash_filename);
	    fprintf(fh, "\n");
	    fprintf(fh, "Please take a look at the logfile to find out what went wrong and fix\n");
	    fprintf(fh, "the problem. Then you can pipe the mail into Petidomo again, to\n");
	    fprintf(fh, "finish processing the request.");
	    CloseMailer(fh);
	}
    }
}

void
RescueMail(const char * mail)
{
    struct flock  lock;
    char *        buffer;
    int           counter;
    int           fd;

    for (buffer = xmalloc(64), counter = 0; ; counter++) {
	sprintf(buffer, "crash/mail%04d", counter);
	debug((DEBUG_MAIN, 8, "Trying rescue file \"%s\".", buffer));
	fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd == -1) {
	    if (errno == EEXIST)
	      continue;
	    else {
		syslog(LOG_CRIT, "Tried to write file \"%s\": %m", buffer);
		exit(1);
	    }
	}
	else
	  break;
    }
    debug((DEBUG_MAIN, 8, "Saving read file to \"%s\" in case something goes wrong.",
	   buffer));

    lock.l_start  = 0;
    lock.l_len    = 0;
    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, &lock);

    if ((write(fd, mail, strlen(mail)) == -1)) {
        syslog(LOG_ERR, "Error occured while writing to file \"%s\": %m", buffer);
        close(fd);
	exit(1);
    }
    close(fd);

    s_crash_filename = buffer;
    atexit(cb_crashmail);
}

void
RemoveRescueMail()
{
    if (s_crash_filename) {
	debug((DEBUG_MAIN, 8, "Removing crash rescue file \"%s\".", s_crash_filename));
	remove(s_crash_filename);
	s_crash_filename = NULL;
    }
}
