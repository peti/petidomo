/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <text.h>
#include <petidomo.h>

int
SendHelp(struct Mail * MailStruct,
		const char * param1,
		const char * param2,
		const char * defaultlist)
{
    const struct PD_Config * MasterConfig;
    const struct List_Config * ListConfig = NULL;
    FILE *   fh;
    char *   originator;
    char *   p;
    char     envelope[1024];
    char *   buffer;

    /* Find out who is who and what to send. */

    MasterConfig = getMasterConfig();
    if (defaultlist != NULL) {
	ListConfig = getListConfig(defaultlist);
	sprintf(envelope, "%s-owner@%s", defaultlist, ListConfig->fqdn);
    }
    else
      sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
    if (param1 != NULL) {
	if (isValidListName(param1) == TRUE) {

	    /* Send list's description back. */

	    debug((DEBUG_COMMAND, 5, "Sending \"%s\" list \"%s\" description.",
		   originator, param1));
	    ListConfig = getListConfig(param1);
	    sprintf(envelope, "%s-owner@%s", param1, MasterConfig->fqdn);
	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh == NULL) {
		syslog(LOG_ERR, "Failed to send mail to \"%s\" regarding this request.",
		    originator);
		return -1;
	    }
	    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		    param1, ListConfig->fqdn);
	    fprintf(fh, "To: %s\n", originator);
	    fprintf(fh, "Subject: Your request \"help %s\"\n", param1);
	    if (MailStruct->Message_Id != NULL)
	      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    fprintf(fh, "Description of list \"%s\":\n\n", param1);
	    buffer = text_easy_sprintf("lists/%s/description", param1);
	    p = loadfile(buffer);
	    if (p != NULL) {
		fprintf(fh, "%s\n", p);
		free(p);
	    }
	    else {
		syslog(LOG_NOTICE, "List \"%s\" doesn't have a description.", param1);
		fprintf(fh, "No description available.\n");
	    }
	    AppendSignature(fh);
	    CloseMailer(fh);
	}
	else {

	    /* List does not exist, I am afraid. */

	    debug((DEBUG_COMMAND, 5, "Received HELP command for non-existing list \"%s\".",
		   param1));
	    fh = vOpenMailer(envelope, originator, NULL);
	    if (fh == NULL) {
		syslog(LOG_ERR, "Failed to send mail to \"%s\" regarding this request.",
		    originator);
		return -1;
	    }
	    if (defaultlist != NULL)
	      fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		      defaultlist, ListConfig->fqdn);
	    else
	      fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n",
		      MasterConfig->fqdn);
	    fprintf(fh, "To: %s\n", originator);
	    fprintf(fh, "Subject: Your request \"help %s\"\n", param1);
	    if (MailStruct->Message_Id != NULL)
	      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	    fprintf(fh, "Precedence: junk\n");
	    fprintf(fh, "Sender: %s\n", envelope);
	    fprintf(fh, "\n");
	    buffer = text_easy_sprintf(
"There is no mailing list \"%s\" on this machine, I am afraid. Please check " \
"whether you spelled the name of the list correctly, or whether you have been " \
"sending this request to the wrong address.\n\nYou can receive a list of all " \
"mailing lists available here by sending the command \"INDEX\" to the " \
"mailing list server.", param1);
	    text_wordwrap(buffer, 75);
	    fprintf(fh, "%s\n", buffer);
	    AppendSignature(fh);
	    CloseMailer(fh);
	}
    }
    else {

	/* Send help text to the originator. */

	debug((DEBUG_COMMAND, 5, "Sending helpfile to \"%s\".", originator));
	fh = vOpenMailer(envelope, originator, NULL);
	if (fh == NULL) {
	    syslog(LOG_ERR, "Failed to send mail to \"%s\" regarding this request.",
		originator);
	    return -1;
	}
	if (defaultlist != NULL)
	  fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
		  defaultlist, ListConfig->fqdn);
	else
	  fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n",
		  MasterConfig->fqdn);
	fprintf(fh, "To: %s\n", originator);
	fprintf(fh, "Subject: Your request \"help\"\n");
	if (MailStruct->Message_Id != NULL)
	  fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
	fprintf(fh, "Precedence: junk\n");
	fprintf(fh, "Sender: %s\n", envelope);
	fprintf(fh, "\n");
	p = loadfile("etc/help");
	if (p != NULL) {
	    fprintf(fh, "%s\n", p);
	    free(p);
	}
	else {
	    syslog(LOG_ERR, "There is no help file for Petidomo!");
	    fprintf(fh, "No help text available.\n");
	}
	AppendSignature(fh);
	CloseMailer(fh);
    }

    return 0;
}

int
Indecipherable(struct Mail * MailStruct, const char * defaultlist)
{
    const struct PD_Config * MasterConfig;
    const struct List_Config * ListConfig = NULL;
    FILE *   fh;
    char *   replyto;
    char *   p;
    char     envelope[1024];

    /* Find out who is who and what to send. */

    MasterConfig = getMasterConfig();
    if (defaultlist != NULL) {
	ListConfig = getListConfig(defaultlist);
	sprintf(envelope, "%s-owner@%s", defaultlist, ListConfig->fqdn);
    }
    else
      sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    replyto = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    /* Send the help file out. */

    debug((DEBUG_COMMAND, 5, "Sending helpfile to \"%s\".", replyto));
    fh = vOpenMailer(envelope, replyto, NULL);
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to send mail to \"%s\" regarding this request.", replyto);
	return -1;
    }
    if (defaultlist != NULL)
      fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
	      defaultlist, ListConfig->fqdn);
    else
      fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n",
	      MasterConfig->fqdn);
    fprintf(fh, "To: %s\n", replyto);
    fprintf(fh, "Subject: Your request \"indecipherable\"\n");
    if (MailStruct->Message_Id != NULL)
      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
    fprintf(fh, "Precedence: junk\n");
    fprintf(fh, "Sender: %s\n", envelope);
    fprintf(fh, "\n");
    p = loadfile("etc/help");
    if (p != NULL) {
	fprintf(fh, "%s\n", p);
	free(p);
    }
    else {
	syslog(LOG_ERR, "There is no help file for Petidomo!");
	fprintf(fh, "No help text available.\n");
    }
    AppendSignature(fh);
    CloseMailer(fh);
    return 0;
}
