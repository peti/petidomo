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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "libtext/text.h"
#include "petidomo.h"

int
GenIndex(struct Mail * MailStruct,
		const char * param1,
		const char * param2,
		const char * defaultlist)
{
    const struct PD_Config * MasterConfig = getMasterConfig();
    const struct List_Config * ListConfig;
    FILE *           fh;
    const char *     address = NULL;
    char             from[4096];
    char             envelope[4096];
    char *           description;
    char *           currLine;
    char *           nextLine;
    char *           buffer;
    DIR *            dirp;
    struct dirent *  entry;
    unsigned int     entry_num;

    address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    /* Initialize internal stuff. */

    MasterConfig = getMasterConfig();
    sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    if (defaultlist != NULL)
      sprintf(from, "%s-request@%s", defaultlist, MasterConfig->fqdn);
    else
      sprintf(from, "petidomo@%s", MasterConfig->fqdn);

    /* Open the mailer. */

    fh = vOpenMailer(envelope, address, NULL);
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to send mail to \"%s\": %m", address);
	return -1;
    }
    fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", from);
    fprintf(fh, "To: %s\n", address);
    fprintf(fh, "Subject: Petidomo: Your request \"index\"\n");
    if (MailStruct->Message_Id != NULL)
      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
    fprintf(fh, "Precedence: junk\n");
    fprintf(fh, "Sender: %s\n", envelope);
    fprintf(fh, "\n");
    fprintf(fh, "Index of available lists:\n");
    fprintf(fh, "=========================\n\n");

    /* Scan the directory. */

    entry_num = 0;
    dirp = opendir("lists");
    if (dirp == NULL) {
	fprintf(fh, \
"An internal error has occured while processing your request. The\n" \
"server administrator has been notified. You don't need to re-submit\n" \
"your request, it will be processed as soon as the problem has been\n" \
"remedied.\n");
        CloseMailer(fh);
	syslog(LOG_ERR, "Failed to read directory \"lists\": %m");
	return -1;
    }
    while((entry = readdir(dirp)) != NULL) {
	if (!strcasecmp(entry->d_name, ".") || !strcasecmp(entry->d_name, ".."))
	  continue;
	if (isValidListName(entry->d_name) == FALSE)
	  continue;

	ListConfig = getListConfig(entry->d_name);
	if (ListConfig->showonindex == FALSE)
	    continue;
	entry_num++;

	/* Print stuff to the mail. */

	fprintf(fh, "%s", entry->d_name);
	{
	    int  i;
	    i = 40 - strlen(entry->d_name);
	    if (i < 1)
	      i = 1;
	    while(i-- > 0)
	      fputc(' ', fh);
	}
	if (ListConfig->allowpubsub == TRUE) {
	    if (ListConfig->listtype == LIST_MODERATED)
	      fprintf(fh, "(moderated mailing list)\n");
	    else
	      fprintf(fh, "(public mailing list)\n");
	}
	else
	  fprintf(fh, "(closed mailing list)\n");

	buffer = text_easy_sprintf("lists/%s/description", entry->d_name);
	description = loadfile(buffer);
	if (description == NULL) {
	    fprintf(fh, "    no description available\n\n");
	    continue;
	}

	for (currLine = description; *currLine != '\0'; currLine = nextLine) {
	    nextLine = text_find_next_line(currLine);
	    if (nextLine[-1] == '\n')
	      nextLine[-1] = '\0';
	    fprintf(fh, "    %s\n", currLine);
	}
	fprintf(fh, "\n");
	free(description);
    }
    closedir(dirp);

    switch (entry_num) {
      case 0:
	  fprintf(fh, "No mailing lists found.\n");
	  break;
      case 1:
	  fprintf(fh, "Found %d mailing list.\n", entry_num);
	  break;
      default:
	  fprintf(fh, "Found %d mailing lists.\n", entry_num);
    }

    CloseMailer(fh);

    return 0;
}
