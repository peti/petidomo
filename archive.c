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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libtext/text.h"
#include "petidomo.h"

int
ArchiveMail(const struct Mail * MailStruct, const char * listname)
    {
    const struct List_Config * ListConfig;
    struct stat   sb;
    FILE *        fh;
    char *        filename;
    u_int         counter;
    int           rc;
    struct tm *   timeptr;
    char *        date;
    time_t        clock;

    assert(MailStruct != NULL);
    assert(listname != NULL);

    /* Initialize internals. */

    ListConfig = getListConfig(listname);
    clock = time(NULL);
    timeptr = localtime(&clock);
    date = asctime(timeptr);
    rc = -1;

    /* Sanity checks. */

    if (ListConfig->archivepath == NULL)
	return 0;

    /* Check whether we have a file or a directory. */

    if (stat(ListConfig->archivepath, &sb) == 0 && (sb.st_mode & S_IFDIR) != 0)
	{

	/* Read the "active"-file to see at what article number we
           were. */

	filename = text_easy_sprintf("%s/.active", ListConfig->archivepath);
	fh = fopen(filename, "r");
	if (fh != NULL)
	    {
	    fscanf(fh, "%u", &counter);
	    fclose(fh);
	    }
	else
	    {
	    if (errno != ENOENT)
		syslog(LOG_ERR, "Failed to read file \"%s\": %m", filename);
	    else
		counter = 0;
	    }

	/* Store the article. */

	do
	    {
	    filename = text_easy_sprintf("%s/%u", ListConfig->archivepath, counter);
	    if (stat(filename, &sb) == -1)
		{
		if (errno == ENOENT)
		    {
		    fh = fopen(filename, "a");
		    if (fh != NULL)
			{
			fprintf(fh, "From %s-owner@%s  %s", listname, ListConfig->fqdn, date);
			fprintf(fh, "%s\n", MailStruct->Header);
			fprintf(fh, "%s\n", MailStruct->Body);
			fclose(fh);
			rc = 0;
			}
		    else
			syslog(LOG_ERR, "Failed opening file \"%s\" for writing: %m", filename);
		    break;
		    }
		else
		    {
		    syslog(LOG_ERR, "An error while trying to access the log " \
			   "directory \"%s\": %m", ListConfig->archivepath);
		    break;
		    }
		}
	    }
	while (++counter);	/* until break */

	/* Write the current "active" number back to the file. */

	counter++;
	filename = text_easy_sprintf("%s/.active", ListConfig->archivepath);
	fh = fopen(filename, "w");
	if (fh != NULL)
	    {
	    fprintf(fh, "%u", counter);
	    fclose(fh);
	    }
	else
	    syslog(LOG_ERR, "Failed to write to file \"%s\": %m", filename);
	}
    else
	{

	/* Simply append the article to this file. */

	fh = fopen(ListConfig->archivepath, "a");
	if (fh != NULL)
	    {
	    /* Write an envelope first. */

	    fprintf(fh, "From %s-owner@%s  %s", listname, ListConfig->fqdn, date);
	    fprintf(fh, "%s\n", MailStruct->Header);
	    fprintf(fh, "%s\n", MailStruct->Body);
	    fclose(fh);
	    rc = 0;
	    }
	else
	    syslog(LOG_ERR, "Failed opening file \"%s\" for writing: %m", ListConfig->archivepath);
	}

    return rc;
    }
