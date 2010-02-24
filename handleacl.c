/*
 * Copyright (c) 1995-2010 Peter Simons <simons@cryp.to>
 * Copyright (c) 2000-2001 Cable & Wireless GmbH
 * Copyright (c) 1999-2000 CyberSolutions GmbH
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "petidomo.h"

/* Returncodes have the following meaning: '-1' == Error, '0' ==
   Proceed, '1' == Mail has been taken care of. */

int
handleACL(struct Mail * MailStruct, const char * listname, int operation, char * parameter)
{
    const struct PD_Config *     MasterConfig;
    const struct List_Config *   ListConfig = NULL;
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
	  break;
      case ACL_PASS:
	  break;
      case ACL_APPROVE:
	  MailStruct->Approve = MasterConfig->master_password;
	  break;
      case ACL_DROP:
	  return 1;
      case ACL_REJECTWITH:
	  assert(parameter != NULL);
      case ACL_REJECT:
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
	    fprintf(fh, "Subject: Petidomo: BOUNCE %s@%s: Rejected due to ACL\n", listname, ListConfig->fqdn);
	  else
	    fprintf(fh, "Subject: Petidomo: BOUNCE: Rejected due to ACL\n");
	  fprintf(fh, "Precedence: junk\n");
	  fprintf(fh, "Sender: %s\n", owner);
	  fprintf(fh, "\n");
	  if (operation == ACL_REJECTWITH && (buffer = loadfile(parameter)) != NULL) {
	      fprintf(fh, "%s\n", buffer);
	      free(buffer);
	  }
	  else {
              if (listname != NULL)
                  fprintf(fh, "The following posting was rejected by Petidomo, due to\n"
                              "the access control list (ACL) rules for list `%s@%s'.\n", listname, ListConfig->fqdn);
	  else
                  fprintf(fh, "The following posting was rejected by Petidomo, due to\n" \
                              "the global access control list (ACL) rules.\n\n");
          }
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
	    fprintf(fh, "Subject: Petidomo: BOUNCE %s@%s: Forwarded due to ACL\n", listname, ListConfig->fqdn);
	  else
	    fprintf(fh, "Subject: Petidomo: BOUNCE: Forwarded due to ACL\n");
	  fprintf(fh, "Precedence: junk\n");
	  fprintf(fh, "Sender: %s\n", owner);
	  fprintf(fh, "\n");
	  if (listname != NULL)
	      fprintf(fh, "The following posting was forwarded to you by Petidomo, due to\n"
		          "the access control list (ACL) rules for list `%s@%s'.\n", listname, ListConfig->fqdn);
          else
	      fprintf(fh, "The following posting was forwarded to you by Petidomo, due to\n" \
		          "the global access control list (ACL) rules.\n");
          fprintf(fh, "If you approve this posting, pipe this mail through `petidomo-approve'.\n"
                      "If you do not approve this posting, just do nothing.\n\n");
	  fprintf(fh, "%s\n", MailStruct->Header);
	  fprintf(fh, "%s", MailStruct->Body);
	  CloseMailer(fh);
	  return 1;
      case ACL_FILTER:
	  assert(parameter != NULL);
	  syslog(LOG_INFO, "Mail is filtered through \"%s\" due to access control.",
	      parameter);
	  rc = MailFilter(MailStruct, parameter);
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

    return 0;
}
