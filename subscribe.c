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
#include "libtext/text.h"
#include "petidomo.h"

int
AddAddress(struct Mail * MailStruct,
		const char * param1,
		const char * param2,
		const char * defaultlist)
{
    const struct List_Config * ListConfig;
    const struct PD_Config   * MasterConfig;
    FILE *         fh;
    const char *   address = NULL;
    const char *   listname = NULL;
    char           owner[4096];
    char           envelope[4096];
    char *         buffer;
    char *         originator;
    char *         p;

    debug((DEBUG_COMMAND, 3, "AddAddress(\"%s\", \"%s\") with default list \"%s\".",
	   param1, param2, defaultlist));

    /* Try to find out, which parameter is what. */

    if (param1 != NULL) {
	if (isValidListName(param1) == TRUE)
	  listname = param1;
	else if (isRFC822Address(param1) == TRUE)
	  address = param1;

	if (param2 != NULL) {
	    if (isValidListName(param2) == TRUE)
	      listname = param2;
	    else if (isRFC822Address(param2) == TRUE)
	      address = param2;
	}
    }

    if (address == NULL)
      address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
    if (listname == NULL && defaultlist != NULL)
      listname = defaultlist;

    if (address == NULL || listname == NULL) {
	syslog(LOG_NOTICE, "%s: subscribe-command invalid: No list specified.",
	    MailStruct->From);
	return 0;
    }

    /* Initialize internal stuff. */
    MasterConfig = getMasterConfig();
    ListConfig = getListConfig(listname);
    sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);
    sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);
    originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    debug((DEBUG_COMMAND, 1, "Subscribing \"%s\" to list \"%s\".", address, listname));

    /* Check whether the request is authorized at all. */

    if (isValidAdminPassword(getPassword(), listname) == FALSE) {

	/* No valid password, check further. */

	debug((DEBUG_COMMAND, 5, "The mail didn't contain any admin password. " \
	       "Checking for authorization..."));

	if (ListConfig->allowpubsub == FALSE) {

	    /* Access was unauthorized, notify the originator. */

	    syslog(LOG_INFO, "\"%s\" tried to subscribe \"%s\" to list \"%s\", but couldn't " \
		"provide the correct password.", originator, address, listname);

	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh != NULL) {
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", originator);
		fprintf(fh, "Subject: Your request \"subscribe %s %s\"\n", address, listname);
		if (MailStruct->Message_Id != NULL)
		  fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf(
"The mailing list \"%s\" is a closed forum and only the maintainer may " \
"subscribe addresses. Your request has been forwarded to the " \
"appropriate person, so please don't send any further mail. You will " \
"be notified as soon as possible.", listname);
		text_wordwrap(buffer, 75);
		fprintf(fh, "%s\n", buffer);
		AppendSignature(fh);
                CloseMailer(fh);
	    }
	    else
	      syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.",
		  originator);

	    /* Notify the owner. */

	    fh = vOpenMailer(envelope, owner, NULL);
	    if (fh != NULL) {
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", owner);
		fprintf(fh, "Subject: Unauthorized request from \"%s\"\n", originator);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf(
"\"%s\" tried to subscribe the address \"%s\" to the \"%s\" mailing list, " \
"but couldn't provide the correct password. To subscribe him, send the " \
"following commands to the server:", originator, address, listname);
		text_wordwrap(buffer, 75);
		fprintf(fh, "%s\n\n", buffer);
		fprintf(fh, "password <AdminPassword>\n");
		fprintf(fh, "subscribe %s %s\n", address, listname);
		AppendSignature(fh);
                CloseMailer(fh);
	    }
	    else {
		syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
		return -1;
	    }
	    return 0;
	}

	if (ListConfig->allowaliensub == FALSE &&
	    (MailStruct->From == NULL || !strcasecmp(address, MailStruct->From) == FALSE) &&
	    (MailStruct->Reply_To == NULL || !strcasecmp(address, MailStruct->Reply_To) == FALSE)) {

	    /* Trying to subscribe something different than himself. */

	    syslog(LOG_INFO, "\"%s\" tried to subscribe \"%s\" to list \"%s\", but the list " \
		"type doesn't allow this.", originator, address, listname);

	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh != NULL) {
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", originator);
		fprintf(fh, "Subject: Your request \"subscribe %s %s\"\n", address, listname);
		if (MailStruct->Message_Id != NULL)
		  fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf(
"The mailing list \"%s\" does not allow to automatically subscribe or unsubscribe an " \
"address not equal to the one, you are mailing from. Your request has been forwarded " \
"to the list administrator, so please don't send any futher mail. You will be notified " \
"as soon as possible.", listname);
		text_wordwrap(buffer, 75);
		fprintf(fh, "%s\n", buffer);
		AppendSignature(fh);
                CloseMailer(fh);
	    }
	    else
	      syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.",
		  originator);

	    /* Notify the owner. */

	    fh = vOpenMailer(envelope, owner, NULL);
	    if (fh != NULL) {
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", owner);
		fprintf(fh, "Subject: Unauthorized request from \"%s\"\n", originator);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf(
"\"%s\" tried to subscribe the address \"%s\" to the \"%s\" mailing list. " \
"The list type does not allow subscribing addresses not equal to the From: " \
"address, though, so the request has been denied. To subscribe this person " \
"manually, send the following commands to the server:", originator, address, listname);
		text_wordwrap(buffer, 75);
		fprintf(fh, "%s\n\n", buffer);
		fprintf(fh, "password <AdminPassword>\n");
		fprintf(fh, "subscribe %s %s\n", address, listname);
		AppendSignature(fh);
                CloseMailer(fh);
	    }
	    else {
		syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
		return -1;
	    }
	    return 0;
	}
    }

    /* Check whether the address is subscribed already. */

    if (isSubscribed(listname, address, NULL, NULL, FALSE) == TRUE) {
	debug((DEBUG_COMMAND, 2, "\"%s\" is already subscribed to list \"%s\".",
	       address, listname));

	/* Notify the originator, that the address is already a
           member. */

	fh = vOpenMailer(envelope, originator, NULL);
	if (fh != NULL) {
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    listname, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", originator);
	    fprintf(fh, "Subject: Your request \"subscribe %s %s\"\n",
		    address, listname);
	    if (MailStruct->Message_Id != NULL)
	      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    fprintf(fh, "The address is subscribed to this list already.\n");
	    AppendSignature(fh);
	    CloseMailer(fh);
	}
	else {
	    syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
	    return -1;
	}
	return 0;
    }

    /* Okay, add the address to the list. */

    buffer = text_easy_sprintf("lists/%s/list", listname);
    fh = fopen(buffer, "a");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to open file \"%s\" for writing: %m", buffer);
	return -1;
    }
    fprintf(fh, "%s\n", address);
    fclose(fh);

    /* Send success notification to the originator, the new
       subscriber, and the owner. */

    if (!strcasecmp(address, originator) == TRUE)
      fh = vOpenMailer(envelope, address, owner, NULL);
    else
      fh = vOpenMailer(envelope, address, originator, owner, NULL);
    if (fh != NULL) {
	fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		listname, ListConfig->fqdn);
	fprintf(fh, "To: %s\n", address);
	if (!strcasecmp(address, originator) == TRUE)
	  fprintf(fh, "Cc: %s\n", owner);
	else
	  fprintf(fh, "Cc: %s, %s\n", originator, owner);
	fprintf(fh, "Subject: Request \"subscribe %s %s\"\n", address, listname);
	if (MailStruct->Message_Id != NULL)
	  fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	fprintf(fh, "Precedence: junk\n");
	fprintf(fh, "Sender: %s\n", envelope);
	fprintf(fh, "\n");
	if (!strcasecmp(address, originator) == TRUE) {
	    buffer = text_easy_sprintf(
"Per your request, the address \"%s\" has been subscribed to the " \
"\"%s\" mailing list. If you want to unsubscribe later, you can " \
"do so by sending the following command to \"%s-request@%s\":",
		     address, listname, listname, ListConfig->fqdn);
	}
	else {
	    buffer = text_easy_sprintf(
"Per request from \"%s\", the address \"%s\" has been subscribed to the " \
"\"%s\" mailing list. If you want to unsubscribe later, you can " \
"do so by sending the following command to \"%s-request@%s\":",
                     originator, address, listname, listname, ListConfig->fqdn);
	}
	text_wordwrap(buffer, 75);
	fprintf(fh, "%s\n\n", buffer);
	fprintf(fh, "unsubscribe %s\n\n", address);
	fprintf(fh, "Please save a copy of this mail, to make sure you remember how " \
		"to\nunsubscribe!\n");
	AppendSignature(fh);
	CloseMailer(fh);
    }
    else {
	syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
	return -1;
    }

    /* Send introduction text to the new member. */

    buffer = text_easy_sprintf("lists/%s/introduction", listname);
    p = loadfile(buffer);
    if (p != NULL) {
	fh = vOpenMailer(envelope, address, NULL);
	if (fh != NULL) {
	    debug((DEBUG_COMMAND, 5, "Sending \"%s\" as welcome mail to the new " \
		   "subscriber.", buffer));
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    listname, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", address);
	    fprintf(fh, "Subject: Welcome to the \"%s\" mailing list!\n", listname);
	    if (MailStruct->Message_Id != NULL)
	      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    fprintf(fh, "%s\n", p);
	    CloseMailer(fh);
	    free(p);
	}
	else {
	    free(p);
	    syslog(LOG_ERR, "Failed to send introduction mail to \"%s\"!", address);
	    return -1;
	}
    }
    return 0;
}

