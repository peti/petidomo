/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <text.h>
#include <petidomo.h>

int
ArchiveMail(const struct Mail * MailStruct, const char * listname)
{
    const struct List_Config * ListConfig;
    struct stat   sb;
    FILE *        fh;
    char *        filename;
    char *        path;
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

    /* Construct the path to the log file or directory. */

    if (*(ListConfig->archivepath) == '/')
      path = xstrdup(ListConfig->archivepath);
    else {
	path = text_easy_sprintf("lists/%s/%s", listname, ListConfig->archivepath);
	path = xstrdup(path);
    }
    debug((DEBUG_ARCHIVE, 2, "Our archive path is \"%s\".", path));

    /* Check whether we have a file or a directory. */

    if (stat(path, &sb) == 0 && (sb.st_mode & S_IFDIR) != 0) {

	/* Store the article to the current number into the directory. */

	debug((DEBUG_ARCHIVE, 3, "\"%s\" is a directory.", path));

	/* Read the "active"-file to see at what article number we
           were. */

	filename = text_easy_sprintf("%s/.active", path);
	fh = fopen(filename, "r");
	if (fh != NULL) {
	    fscanf(fh, "%u", &counter);
	    debug((DEBUG_ARCHIVE, 5, ".active file contained '%u'.", counter));
	    fclose(fh);
	}
	else {
	    if (errno != ENOENT)
	      syslog(LOG_ERR, "Failed to read file \"%s\": %m", filename);
	    else
	      debug((DEBUG_ARCHIVE, 1, "File \".active\" did not exist."));
	    counter = 0;
	}

	/* Store the article. */

	do {
	    filename = text_easy_sprintf("%s/%u", path, counter);
	    debug((DEBUG_ARCHIVE, 4, "Testing whether file \"%s\" exists already.",
		   filename));
	    if (stat(filename, &sb) == -1) {
		debug((DEBUG_ARCHIVE, 7, "Nope, it doesn't."));
		if (errno == ENOENT) {
		    debug((DEBUG_ARCHIVE, 1, "Writing mail to file \"%s\".", filename));
		    fh = fopen(filename, "a");
		    if (fh != NULL) {
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
		else {
		    syslog(LOG_ERR, "An error while trying to access the log " \
			     "directory \"%s\": %m", path);
		    break;
		}
	    }
	    else
	      debug((DEBUG_ARCHIVE, 7, "Yep, it does."));
	} while (++counter);	/* until break */

	/* Write the current "active" number back to the file. */

	counter++;
	filename = text_easy_sprintf("%s/.active", path);
	fh = fopen(filename, "w");
	if (fh != NULL) {
	    fprintf(fh, "%u", counter);
	    debug((DEBUG_ARCHIVE, 5, "Wrote '%u' to .active file.", counter));
	    fclose(fh);
	}
	else
	  syslog(LOG_ERR, "Failed to write to file \"%s\": %m", filename);
    }
    else {

	/* Simply append the article to this file. */

	debug((DEBUG_ARCHIVE, 1, "Appending mail to logfile \"%s\".", path));
	fh = fopen(path, "a");
	if (fh != NULL) {
	    /* Write an envelope first. */

	    fprintf(fh, "From %s-owner@%s  %s", listname, ListConfig->fqdn, date);
	    fprintf(fh, "%s\n", MailStruct->Header);
	    fprintf(fh, "%s\n", MailStruct->Body);
	    fclose(fh);
	    rc = 0;
	}
	else
	  syslog(LOG_ERR, "Failed opening file \"%s\" for writing: %m", path);
    }

    free(path);
    return rc;
}
