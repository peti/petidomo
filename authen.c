/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <string.h>
#include <ctype.h>

#include <text.h>
#include <petidomo.h>

int
FindBodyPassword(struct Mail * MailStruct)
{
    char *   currLine;

    currLine = MailStruct->Body;
    while(isspace((int)*currLine))
      currLine++;
    if (!strncasecmp(currLine, "Approve", 7) ||
	!strncasecmp(currLine, "Approved", 8) ||
	!strncasecmp(currLine, "Passwd", 6) ||
	!strncasecmp(currLine, "Password", 8)) {
	MailStruct->Body = text_find_next_line(currLine);
	(text_find_next_line(currLine))[-1] = '\0';
	while(!isspace((int)*currLine))
	  currLine++;
	if (ParseApproveLine(currLine) != 0) {
	    syslog(LOG_ERR, "Failed to parse the approve statement in the mail body.");
	    return -1;
	}
	debug((DEBUG_HERMES, 3, "Found password \"%s\" in mail body.", currLine));
	MailStruct->Approve = currLine;
    }
    return 0;
}

bool
isValidAdminPassword(const char * password, const char * listname)
{
    const struct PD_Config *     MasterConfig;
    const struct List_Config *   ListConfig;

    if (password == NULL)
      return FALSE;

    MasterConfig = getMasterConfig();

    if (!strcasecmp(MasterConfig->master_password, password))
      return TRUE;

    if (listname != NULL) {
	ListConfig = getListConfig(listname);

	if (ListConfig->admin_password == NULL)
	  return FALSE;

	debug((DEBUG_AUTHEN, 5, "Comparing provided password '%s' to correct one '%s'.",
	       ListConfig->admin_password, password));

	if (!strcasecmp(ListConfig->admin_password, password))
	  return TRUE;
    }
    return FALSE;
}

bool
isValidPostingPassword(const char * password, const char * listname)
{
    const struct List_Config *   ListConfig;

    if (password == NULL)
      return FALSE;

    if (isValidAdminPassword(password, listname) == TRUE)
      return TRUE;

    if (listname != NULL) {
	ListConfig = getListConfig(listname);

	if (ListConfig->posting_password == NULL)
	  return FALSE;

	debug((DEBUG_AUTHEN, 5, "provided password '%s' to correct one '%s'.",
	       ListConfig->posting_password, password));

	if (!strcasecmp(ListConfig->posting_password, password)) {
	    debug((DEBUG_AUTHEN, 2, "Provided password is correct!"));
	    return TRUE;
	}
	else {
	    debug((DEBUG_AUTHEN, 2, "Provided password is incorrect!"));
	}

    }

    return FALSE;
}
