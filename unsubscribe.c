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

#include <string.h>
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

int
DeleteAddress(struct Mail * MailStruct,
	      const char * param1,
	      const char * param2,
	      const char * defaultlist)
    {
    const struct PD_Config   * MasterConfig;
    const struct List_Config * ListConfig;
    FILE *         fh;
    const char *   address = NULL;
    const char *   listname = NULL;
    char           owner[4096];
    char           envelope[4096];
    char *         buffer;
    char *         originator;
    char *         p;
    char *         list;

    /* Initialize internal stuff from master config file. */

    MasterConfig = getMasterConfig();
    sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    /* Try to find out, which parameter is what. */

    if (param1 != NULL)
	{
	if (isValidListName(param1) == TRUE)
	    listname = param1;
	else if (isRFC822Address(param1) == TRUE)
	    address = param1;

        if (param2 != NULL)
            {
            if (listname == NULL && isValidListName(param2) == TRUE)
                listname = param2;
            else if (address == NULL && isRFC822Address(param2) == TRUE)
                address = param2;
            }
	}

    if (address == NULL)
	address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
    assert(address != NULL);

    if (listname == NULL)
	{
	if (defaultlist != NULL)
	    listname = defaultlist;
	else
	    {
	    syslog(LOG_INFO, "%s: unsubscribe-command invalid: No list specified.", MailStruct->From);
	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh != NULL)
		{
		fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n", MasterConfig->fqdn);
		fprintf(fh, "To: %s\n", originator);
		fprintf(fh, "Subject: Petidomo: Your request \"unsubscribe %s\"\n", address);
		if (MailStruct->Message_Id != NULL)
		    fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf("You tried to unsubscribe the address \"%s\" from a mailing list. "   \
					   "Unfortunately, your request could not be processed, because "    \
					   "you didn't specify a valid mailing list name from which the "      \
					   "address should be unsubscribed. You may use the command INDEX " \
					   "to receive an overview over the available mailing lists. Also, " \
					   "use the command HELP to verify that you got the command syntax " \
					   "right.", address);
		text_wordwrap(buffer, 70);
		fprintf(fh, "%s\n", buffer);
		CloseMailer(fh);
		}
	    else
		syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
	    return 0;
	    }
	}

    /* Initialize internal stuff again from list config. */

    ListConfig = getListConfig(listname);
    sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);
    sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);

    /* Check whether the request is authorized at all. */

    if (isValidAdminPassword(getPassword(), listname) == FALSE)
	{
	/* No valid password, check further. */

	if (ListConfig->subtype == SUBSCRIPTION_ADMIN)
	    {
	    /* Access was unauthorized, notify the originator. */

	    syslog(LOG_INFO, "%s: Attempt to unsubscribe \"%s\" from list \"%s\" rejected due to lack of " \
		   "a correct admin password.", MailStruct->From, address, listname);

	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh != NULL)
		{
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", originator);
		fprintf(fh, "Subject: Petidomo: Your request \"unsubscribe %s %s\"\n",
			address, listname);
		if (MailStruct->Message_Id != NULL)
		    fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf("The mailing list \"%s\" is a closed forum and only the maintainer may " \
					   "unsubscribe addresses. Your request has been forwarded to the " \
					   "appropriate person, so please don't send any further mail. You will " \
					   "be notified as soon as possible.", listname);
		text_wordwrap(buffer, 70);
		fprintf(fh, "%s\n", buffer);
                CloseMailer(fh);
		}
	    else
		syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);

	    /* Notify the owner. */

	    fh = vOpenMailer(envelope, owner, NULL);
	    if (fh != NULL)
		{
		fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
			listname, ListConfig->fqdn);
		fprintf(fh, "To: %s\n", owner);
		fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Unauthorized request from \"%s\"\n", listname, ListConfig->fqdn, originator);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		buffer = text_easy_sprintf("\"%s\" tried to unsubscribe the address \"%s\" from the \"%s\" mailing list, " \
					   "but couldn't provide the correct password. To unsubscribe him, send the " \
					   "following commands to the server:", originator, address, listname);
		text_wordwrap(buffer, 70);
		fprintf(fh, "%s\n\n", buffer);
		fprintf(fh, "password <AdminPassword>\n");
		fprintf(fh, "unsubscribe %s %s\n", address, listname);
                CloseMailer(fh);
		}
	    else
		{
		syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
		return -1;
		}
	    return 0;
	    }
	}

    /* Okay, remove the address from the list. */

    if (isSubscribed(listname, address, &list, &p, FALSE) == FALSE)
	{
       syslog(LOG_INFO, "%s: Attempt to unsubscribe \"%s\" from list \"%s\" rejected, because the " \
              "address is not on the list.", MailStruct->From, address, listname);

	/* Notify the originator, that the address is not subscribed at
	   all. */

	fh = vOpenMailer(envelope, originator, NULL);
	if (fh != NULL)
	    {
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    listname, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", originator);
	    fprintf(fh, "Subject: Petidomo: Your request \"unsubscribe %s %s\"\n",
		    address, listname);
	    if (MailStruct->Message_Id != NULL)
		fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    fprintf(fh, "The address is not subscribed to this list.\n");
	    CloseMailer(fh);
	    }
	else
	    {
	    syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.",
		   originator);
	    return -1;
	    }
	}
    else
	{
	if (isValidAdminPassword(getPassword(), listname) == FALSE &&
	    ListConfig->subtype == SUBSCRIPTION_ACKED && !g_is_approved)
	    {
	    /* Require confirmation. */

	    char* command;
	    char* cookie;

	    syslog(LOG_INFO, "%s: Attempt to unsubscribe \"%s\" from list \"%s\" deferred, because the " \
		   "request must be acknowledged first.", MailStruct->From, address, listname);

	    command = text_easy_sprintf("unsubscribe %s %s", address, listname);
	    cookie  = queue_command(MailStruct, command);

	    /* Send request for confirmation to the user. */

	    fh = vOpenMailer(envelope, address, NULL);
	    if (fh != NULL)
		{
		fprintf(fh, "From: petidomo-approve@%s (Petidomo Mailing List Server)\n", ListConfig->fqdn);
		fprintf(fh, "To: %s\n", address);
		fprintf(fh, "Subject: Petidomo: CONFIRM %s@%s: Request from \"%s\"\n", listname, ListConfig->fqdn, originator);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", envelope);
		fprintf(fh, "\n");
		if (strcasecmp(address, originator) == 0)
		    buffer = text_easy_sprintf("You requested that the address \"%s\" should be unsubscribed from " \
					       "the mailing list \"%s\". This will not happen unless you confirm the " \
					       "request by replying to this mail and citing the string",
					       originator, address, listname);
		else
		    buffer = text_easy_sprintf("Per request from \"%s\", the address \"%s\" should be unsubscribed from " \
					       "the mailing list \"%s\". This will not happen unless you confirm the " \
					       "request by replying to this mail and citing the string",
					       originator, address, listname);
		text_wordwrap(buffer, 70);
		fprintf(fh, "%s\n", buffer);
		fprintf(fh, "\n    %s\n\n", cookie);
		fprintf(fh, "in your reply.\n");
		CloseMailer(fh);
		}
	    else
		{
		syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
		return -1;
		}

            /* If the request for confirmation has been sent to an
               address different to that of the originator, notify him
               what happened. */

	    if (strcasecmp(address, originator) == 0)
		{
		fh = vOpenMailer(envelope, originator, NULL);
		if (fh != NULL)
		    {
		    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n", listname, ListConfig->fqdn);
		    fprintf(fh, "To: %s\n", originator);
		    fprintf(fh, "Subject: Petidomo: Your request \"unsubscribe %s %s\"\n", address, listname);
		    if (MailStruct->Message_Id != NULL)
			fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
		    fprintf(fh, "Precedence: junk\n");
		    fprintf(fh, "Sender: %s\n", envelope);
		    fprintf(fh, "\n");
		    fprintf(fh, "Unsubscribing the address will need confirmation. Such a\n");
		    fprintf(fh, "request has been sent to the address already, so don't move!\n");
		    CloseMailer(fh);
		    }
		else
		    {
		    syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
		    return -1;
		    }
		}

	    return 0;
	    }

	syslog(LOG_INFO, "%s: Okay; unsubscribing address \"%s\" from list \"%s\".", MailStruct->From, address, listname);

	fh = fopen(ListConfig->address_file, "w");
	if (fh == NULL)
	    {
	    syslog(LOG_ERR, "Failed to open file \"%s\" for writing: %s", ListConfig->address_file, strerror(errno));
	    return -1;
	    }
	*p++ = '\0';
	fprintf(fh, "%s", list);
	p = text_find_next_line(p); /* skip address in question */
	fprintf(fh, "%s", p);
	fclose(fh);

	/* Send success notification to the originator, and the
	   unsubscribed address. */

	if (!strcasecmp(address, originator) == TRUE)
	    fh = vOpenMailer(envelope, address, owner, NULL);
	else
	    fh = vOpenMailer(envelope, address, originator, owner, NULL);
	if (fh != NULL)
	    {
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    listname, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", address);
	    if (!strcasecmp(address, originator) == TRUE)
		fprintf(fh, "Cc: %s\n", owner);
	    else
		fprintf(fh, "Cc: %s, %s\n", originator, owner);
	    fprintf(fh, "Subject: Petidomo: Request \"unsubscribe %s %s\"\n", address, listname);
	    if (MailStruct->Message_Id != NULL)
		fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    if (!strcasecmp(address, originator) == TRUE)
		{
		buffer = text_easy_sprintf(
					   "Per your request, the address \"%s\" has been unsubscribed from the " \
					   "\"%s\" mailing list.\n\n", address, listname);
		}
	    else
		{
		buffer = text_easy_sprintf(
					   "Per request from \"%s\", the address \"%s\" has been unsubscribed from the " \
					   "\"%s\" mailing list.\n\n", originator, address, listname);
		}
	    text_wordwrap(buffer, 70);
	    fprintf(fh, "%s", buffer);
	    CloseMailer(fh);
	    }
	else
	    {
	    syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
	    return -1;
	    }
	}
    free(list);
    return 0;
    }
