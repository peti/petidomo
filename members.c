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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#include "libtext/text.h"
#include "petidomo.h"

int
SendSubscriberList(struct Mail * MailStruct,
		   const char * param1,
		   const char * param2,
		   const char * defaultlist)
{
    const struct List_Config * ListConfig;
    FILE *         fh;
    const char *   address = NULL;
    const char *   listname = NULL;
    char           owner[4096];
    char           envelope[4096];
    char *         buffer;
    char *         p;
    int            i;

    /* Try to find out, which parameter is what. */

    if (param1 != NULL) {
	if (isValidListName(param1) == TRUE)
	  listname = param1;
    }

    address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
    if (listname == NULL && defaultlist != NULL)
      listname = defaultlist;

    if (address == NULL || listname == NULL) {
	syslog(LOG_NOTICE, "%s: members-command invalid: No list specified.",
	    MailStruct->From);
	return 0;
    }

    /* Initialize internal stuff. */

    ListConfig = getListConfig(listname);
    sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);
    sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);

    /* Check whether 'members' is allowed for this list. */

    if (isValidAdminPassword(getPassword(), listname) == FALSE &&
	ListConfig->allowmembers == FALSE) {

	syslog(LOG_NOTICE, "MEMBERS command from \"%s\" has been denied.", address);
	fh = vOpenMailer(envelope, address, owner, NULL);
	if (fh != NULL) {
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    listname, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", address);
	    fprintf(fh, "Cc: %s\n", owner);
	    fprintf(fh, "Subject: Petidomo: Request \"members %s\"\n", listname);
	    if (MailStruct->Message_Id != NULL)
	      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    buffer = text_easy_sprintf(
"The MEMBERS command has been disabled for this mailing list, I am " \
"afraid. If there's a certain reason, why you need to know the list " \
"of subscribed addresses, please contact the mailing list administrator " \
"under the address \"%s\" instead.", owner);
	    text_wordwrap(buffer, 75);
	    fprintf(fh, "%s\n", buffer);
	    CloseMailer(fh);
	}
	else

    syslog(LOG_ERR, "Failed to send mail to \"%s\"", address);
	return 0;
    }

    /* Okay, send the address list back. */

    buffer = text_easy_sprintf("lists/%s/list", listname);
    buffer = loadfile(buffer);
    if (buffer == NULL) {
	syslog(LOG_ERR, "Failed to open file \"~petidomo/lists/%s/list\"", listname);
	return -1;
    }

    fh = vOpenMailer(envelope, address, NULL);
    if (fh != NULL) {
	fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		listname, ListConfig->fqdn);
	fprintf(fh, "To: %s\n", address);
	fprintf(fh, "Subject: Petidomo: Request \"members %s\"\n", listname);
	if (MailStruct->Message_Id != NULL)
	  fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	fprintf(fh, "Precedence: junk\n");
	fprintf(fh, "Sender: %s\n", envelope);
	fprintf(fh, "\n");
	fprintf(fh, "Subscribers of list \"%s\":\n", listname);
	fprintf(fh, "=======================");
	fflush(fh);
	for (i = 0; i < strlen(listname); i++) {
	    fputc('=', fh);
	}
	fputc('\n', fh);
	for (p = buffer; *p; p++) {
	    if (isspace((int)*p)) {
		fputc('\n', fh);
		while (*p != '\0' && *p != '\n')
		  p++;
	    }
	    else {
	      fputc(*p, fh);
	    }
	}
	CloseMailer(fh);
    }
    else {
	free(buffer);
	syslog(LOG_ERR, "Failed to send email to \"%s\"!", address);
	return -1;
    }

    free(buffer);
    return 0;
}
