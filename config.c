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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "liblists/lists.h"
#include "libconfigfile/configfile.h"
#include "petidomo.h"

static struct PD_Config *  MasterConfig;
List ListConfigs;

/* These variables need to be static/global, so that the addresses as
   used in MasterCF are known at compile time. */

static char *             fqdn = NULL;
static char *             master_password = NULL;
static char *             mta = "/usr/sbin/sendmail";
static char *             mta_options = "-i -f%s";

int
InitPetidomo(void)
{
    char *             basedir = "/usr/local/petidomo";
    int                rc;

    /* Format description of our global config file. */

    struct ConfigFile  MasterCF[] = {
	{ "Hostname", CF_STRING, &fqdn },
	{ "AdminPassword", CF_STRING, &master_password },
	{ "MTA", CF_STRING, &mta },
	{ "MTA_Options", CF_STRING, &mta_options },
	{ NULL, 0, NULL}
    };

    /* Allocate memory for the global config structure. */

    MasterConfig = calloc(sizeof(struct PD_Config), 1);
    if (MasterConfig == NULL) {
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", sizeof(struct PD_Config));
	return -1;
    }

    /* Init the list of read list configs. */

    ListConfigs = InitList(NULL);

    /* chdir() into the base directory. */

    rc = chdir(basedir);
    if (rc != 0) {
	syslog(LOG_ERR, "Failed to change current directory to \"%s\": %m", basedir);
	return -1;
    }

    /* Parse the config file. */

    rc = ReadConfig("etc/petidomo.conf", MasterCF);
    if (rc != 0) {
	syslog(LOG_ERR, "Failed to parse the master config file \"petidomo.conf\"");
	return -1;
    }

    /* Do consistency checks. */

    if (fqdn == NULL) {
	syslog(LOG_ERR, "The master config file \"petidomo.conf\" doesn't set the host name.");
	return -1;
    }
    if (master_password == NULL) {
	syslog(LOG_ERR, "The master config file \"petidomo.conf\" doesn't set the admin password.");
	return -1;
    }
    if (strstr(mta_options, "%s") == NULL) {
	syslog(LOG_ERR, "The argument to MTA_Options in the master config file is invalid.");
	return -1;
    }

    /* Copy the results to the structure. */

    MasterConfig->basedir = basedir;
    MasterConfig->fqdn = fqdn;
    MasterConfig->master_password = master_password;
    MasterConfig->mta = mta;
    MasterConfig->mta_options = mta_options;

    return 0;
}

const struct PD_Config *
getMasterConfig(void)
{
    return MasterConfig;
}

static char *     list_fqdn = NULL;
static char *     admin_password = NULL;
static char *     posting_password = NULL;
static char *     listtype = NULL;
static char *     reply_to = NULL;
static char *     postingfilter = NULL;
static char *     archivepath = NULL;
static bool       allowpubsub = TRUE;
static bool       allowaliensub = TRUE;
static bool       allowmembers = FALSE;
static bool       showonindex = TRUE;

const struct List_Config *
getListConfig(const char * listname)
{
    const struct PD_Config *  MasterConfig;
    struct List_Config *      ListConfig;
    Node                      node;
    int                       rc;
    char                      buffer[4096];

    /* Format description of our global config file. */

    struct ConfigFile ListCF[] = {
	{ "ListType", CF_STRING, &listtype },
	{ "AllowPublicSubscription", CF_YES_NO, &allowpubsub },
	{ "AllowAlienSubscription", CF_YES_NO, &allowaliensub },
	{ "AllowMembersCommand", CF_YES_NO, &allowmembers },
	{ "ShowOnIndex", CF_YES_NO, &showonindex },
	{ "ReplyTo", CF_STRING, &reply_to },
	{ "Hostname", CF_STRING, &list_fqdn },
	{ "AdminPassword", CF_STRING, &admin_password },
	{ "PostingPassword", CF_STRING, &posting_password },
	{ "PostingFilter", CF_STRING, &postingfilter },
	{ "Archive", CF_STRING, &archivepath },
	{ NULL, 0, NULL}
    };

    /* Get the master configuration. */

    MasterConfig = getMasterConfig();

    /* Did we read this config file already? */

    node = FindNodeByKey(ListConfigs, listname);
    if (node != NULL)
      return getNodeData(node);

    /* No? Then read the config file. */

    sprintf(buffer, "lists/%s/config", listname);
    rc = ReadConfig(buffer, ListCF);
    if (rc != 0) {
	syslog(LOG_ERR, "Failed to parse the list \"%s\"'s config file.", listname);
	exit(1);
    }

    /* Do consistency checks. */

    if (listtype == NULL) {
	syslog(LOG_ERR, "List \"%s\" doesn't have a valid type in config file.", listname);
	exit(1);
    }

    /* Set up the list config structure. */

    ListConfig = calloc(sizeof(struct List_Config), 1);
    if (ListConfig == NULL) {
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", sizeof(struct List_Config));
	exit(1);
    }
    if (!strcasecmp(listtype, "open"))
      ListConfig->listtype = LIST_OPEN;
    else if (!strcasecmp(listtype, "closed"))
      ListConfig->listtype = LIST_CLOSED;
    else if (!strcasecmp(listtype, "moderated"))
      ListConfig->listtype = LIST_MODERATED;
    else {
	syslog(LOG_ERR, "List \"%s\" doesn't have a valid type in config file.", listname);
	exit(1);
    }
    ListConfig->allowpubsub = allowpubsub;
    ListConfig->allowaliensub = allowaliensub;
    ListConfig->allowmembers = allowmembers;
    ListConfig->showonindex = showonindex;
    ListConfig->fqdn = (list_fqdn) ? list_fqdn : MasterConfig->fqdn;
    ListConfig->reply_to = reply_to;
    if (reply_to != NULL && strcasecmp(reply_to, "none"))
      CanonizeAddress(&(ListConfig->reply_to), ListConfig->fqdn);
    ListConfig->admin_password = admin_password;
    ListConfig->posting_password = posting_password;
    ListConfig->postingfilter = postingfilter;
    ListConfig->archivepath = archivepath;
    AppendNode(ListConfigs, xstrdup(listname), ListConfig);

    return ListConfig;
}
