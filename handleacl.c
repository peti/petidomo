/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <stdlib.h>

#include <petidomo.h>

/* Returncodes have the following meaning: '-1' == Error, '0' ==
   Proceed, '1' == Mail has been taken care of. */

int
handleACL(struct Mail * MailStruct, const char * listname, int operation, char * parameter)
{
    const struct PD_Config *     MasterConfig;
    const struct List_Config *   ListConfig;
    FILE *          fh;
    char *          buffer;
    char            envelope[1024];
    char            owner[1024];
    int             rc;

    assert(MailStruct != NULL);

    MasterConfig = getMasterConfig();
    if (listname != NULL) {
	ListConfig = getListConfig(listname);
	sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);
	sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);
    }
    else {
	sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
	sprintf(owner, "petidomo-manager@%s", MasterConfig->fqdn);
    }

    /* Check for authorization. */

    switch(operation) {
      case ACL_NONE:
	  debug((DEBUG_ACL, 4, "No ACL statement matched the mail."));
	  break;
      case ACL_PASS:
	  debug((DEBUG_ACL, 4, "Mail passed access control."));
	  break;
      case ACL_DROP:
	  syslog(LOG_INFO, "Mail is dropped due to access control.");
	  return 1;
      case ACL_REJECTWITH:
	  assert(parameter != NULL);
      case ACL_REJECT:
	  syslog(LOG_INFO, "Mail is rejected due to access control.");
	  fh = vOpenMailer(envelope, owner, (MailStruct->Reply_To) ?
			   (MailStruct->Reply_To) : (MailStruct->From), NULL);
	  if (fh == NULL) {
	      syslog(LOG_ERR, "Failed to open mailer for redirection.");
	      return -1;
	  }
	  fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", owner);
	  fprintf(fh, "To: %s\n", (MailStruct->Reply_To) ?
		  (MailStruct->Reply_To) : (MailStruct->From));
	  fprintf(fh, "Cc: %s\n", owner);
	  if (listname != NULL)
	    fprintf(fh, "Subject: Your posting to list \"%s\" was rejected\n", listname);
	  else
	    fprintf(fh, "Subject: Your petidomo request was rejected\n");
	  fprintf(fh, "Precedence: junk\n");
	  fprintf(fh, "Sender: %s\n", owner);
	  fprintf(fh, "\n");
	  if (operation == ACL_REJECTWITH && (buffer = loadfile(parameter)) != NULL) {
	      fprintf(fh, "%s\n", buffer);
	      free(buffer);
	  }
	  else
	    fprintf(fh, "Your article was rejected by the access control rules:\n\n");
	  fprintf(fh, "%s\n", MailStruct->Header);
	  fprintf(fh, "%s", MailStruct->Body);
	  CloseMailer(fh);
	  return 1;
      case ACL_REDIRECT:
	  assert(parameter != NULL);
	  syslog(LOG_INFO, "Mail is redirected to \"%s\" due to access control.", parameter);
	  fh = vOpenMailer(MailStruct->Envelope, parameter, NULL);
	  if (fh == NULL) {
	      syslog(LOG_ERR, "Failed to open mailer for redirection.");
	      return -1;
	  }
	  fprintf(fh, "%s\n", MailStruct->Header);
	  fprintf(fh, "%s", MailStruct->Body);
	  CloseMailer(fh);
	  return 1;
      case ACL_FORWARD:
	  assert(parameter != NULL);
	  syslog(LOG_INFO, "Mail is forwarded to \"%s\" due to access control.", parameter);
	  fh = vOpenMailer(envelope, parameter, NULL);
	  if (fh == NULL) {
	      syslog(LOG_ERR, "Failed to open mailer for redirection.");
	      return -1;
	  }
	  fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", owner);
	  fprintf(fh, "To: %s\n", parameter);
	  if (listname != NULL)
	    fprintf(fh, "Subject: Disallowed posting from \"%s\" to list \"%s\"\n",
		    MailStruct->From, listname);
	  else
	    fprintf(fh, "Subject: Disallowed petidomo request from \"%s\"\n",
		    MailStruct->From);
	  fprintf(fh, "Precedence: junk\n");
	  fprintf(fh, "Sender: %s\n", owner);
	  fprintf(fh, "\n");
	  fprintf(fh, "The following article was forwarded to you, due to the\n" \
		  "access control rules:\n\n");
	  fprintf(fh, "%s\n", MailStruct->Header);
	  fprintf(fh, "%s", MailStruct->Body);
	  CloseMailer(fh);
	  return 1;
      case ACL_FILTER:
	  assert(parameter != NULL);
	  syslog(LOG_INFO, "Mail is filtered through \"%s\" due to access control.",
	      parameter);
	  rc = MailFilter(MailStruct, parameter);
	  debug((DEBUG_ACL, 3, "Mail filter \"%s\" returned %d.", parameter, rc));
	  if (rc != 0) {
	      syslog(LOG_ERR, "Mail filter \"%s\" returned error code %d.", parameter, rc);
	      return -1;
	  }
	  break;
      default:
	  syslog(LOG_CRIT, "Internal error: Unexpected return code %d from checkACL()",
	      operation);
	  return -1;
    }
    if (parameter != NULL)
      free(parameter);

#ifdef DEBUG
    if (listname != NULL) {
	debug((DEBUG_ACL, 3, "\"%s\" is authorized to post to \"%s\".",
	       MailStruct->From, listname));
    }
    else {
	debug((DEBUG_ACL, 3, "Request from \"%s\" is okay, says ACL", MailStruct->From));
    }
#endif

    return 0;
}
