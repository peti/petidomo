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
#include <sys/stat.h>

#include "libtext/text.h"
#include "liblists/lists.h"
#include "libconfigfile/configfile.h"
#include "petidomo.h"

static struct PD_Config *  MasterConfig;
List ListConfigs;

/* These variables need to be static/global, so that the addresses as
   used in MasterCF are known at compile time. */

static char*  fqdn            = NULL;
static char*  master_password = NULL;
static char*  mta             = NULL;
static char*  mta_options     = "-i -f%s";
static char*  help_file       = DATADIR "/petidomo.conf";
static char*  acl_file        = SYSCONFDIR "/petidomo.acl";
static char*  index_file      = LOCALSTATEDIR "/index";
static char*  list_dir        = LOCALSTATEDIR;

int InitPetidomo(const char* masterconfig_path)
    {
    int    rc;

    /* Format description of our global config file. */

    struct ConfigFile  MasterCF[] =
	{
	{ "Hostname", CF_STRING, &fqdn },
	{ "AdminPassword", CF_STRING, &master_password },
	{ "MTA", CF_STRING, &mta },
	{ "MTA_Options", CF_STRING, &mta_options },
	{ "Help_File", CF_STRING, &help_file },
	{ "Acl_File", CF_STRING, &acl_file },
	{ "Index_File", CF_STRING, &index_file },
	{ "List_Directory", CF_STRING, &list_dir },
	{ NULL, 0, NULL}
	};

    /* Allocate memory for the global config structure. */

    MasterConfig = calloc(sizeof(struct PD_Config), 1);
    if (MasterConfig == NULL)
	{
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", sizeof(struct PD_Config));
	return -1;
	}

    /* Init the list of read list configs. */

    ListConfigs = InitList(NULL);

    /* Parse the config file. */

    rc = ReadConfig(masterconfig_path, MasterCF);
    if (rc != 0)
	{
	syslog(LOG_ERR, "Failed to parse the master config file.");
	return -1;
	}

    /* Do consistency checks. */

    if (fqdn == NULL)
	{
	syslog(LOG_ERR, "The master config file \"%s\" doesn't set the host name.", masterconfig_path);
	return -1;
	}
    if (mta == NULL)
	{
	syslog(LOG_ERR, "The master config file \"%s\" doesn't set the MTA.", masterconfig_path);
	return -1;
	}
    if (master_password == NULL)
	{
	syslog(LOG_ERR, "The master config file \"%s\" doesn't set the admin password.", masterconfig_path);
	return -1;
	}
    if (strstr(mta_options, "%s") == NULL)
	{
	syslog(LOG_ERR, "The argument to MTA_Options in the master config file is invalid.");
	return -1;
	}

    /* Copy the results to the structure. */

    MasterConfig->fqdn = fqdn;
    MasterConfig->master_password = master_password;
    MasterConfig->mta = mta;
    MasterConfig->mta_options = mta_options;
    MasterConfig->help_file = help_file;
    MasterConfig->acl_file = acl_file;
    MasterConfig->index_file = index_file;
    MasterConfig->list_dir = list_dir;

    return 0;
    }

const struct PD_Config* getMasterConfig(void)
    {
    return MasterConfig;
    }


static char*     list_fqdn;
static char*     admin_password;
static char*     posting_password;
static char*     listtype;
static char*     reply_to;
static char*     postingfilter;
static char*     archivepath;
static bool      allowpubsub;
static bool      allowaliensub;
static bool      allowmembers;
static char*     intro_file;
static char*     sig_file;
static char*     desc_file;
static char*     header_file;
static char*     list_acl_file;
static char*     address_file;
static char*     ack_file;

const struct List_Config* getListConfig(const char * listname)
    {
    const struct PD_Config *  MasterConfig;
    struct List_Config *      ListConfig;
    Node                      node;
    int                       rc;
    char *                    buffer;
    struct stat               sb;
    char*                     list_dir;

    /* Format description of our global config file. */

    struct ConfigFile ListCF[] =
	{
	{ "ListType", CF_STRING, &listtype },
	{ "AllowPublicSubscription", CF_YES_NO, &allowpubsub },
	{ "AllowAlienSubscription", CF_YES_NO, &allowaliensub },
	{ "AllowMembersCommand", CF_YES_NO, &allowmembers },
	{ "ReplyTo", CF_STRING, &reply_to },
	{ "Hostname", CF_STRING, &list_fqdn },
	{ "AdminPassword", CF_STRING, &admin_password },
	{ "PostingPassword", CF_STRING, &posting_password },
	{ "PostingFilter", CF_STRING, &postingfilter },
	{ "Archive", CF_STRING, &archivepath },
	{ "IntroductionFile", CF_STRING, &intro_file },
	{ "SignatureFile", CF_STRING, &sig_file },
	{ "DescriptionFile", CF_STRING, &desc_file },
	{ "HeaderFile", CF_STRING, &header_file },
	{ "ListACLFile", CF_STRING, &list_acl_file },
	{ "AddressFile", CF_STRING, &address_file },
	{ "AcknowledgementFile", CF_STRING, &ack_file },
	{ NULL, 0, NULL}
	};

    /* Set the defaults. */

    list_fqdn        = NULL;
    admin_password   = NULL;
    posting_password = NULL;
    listtype         = NULL;
    reply_to         = NULL;
    postingfilter    = NULL;
    archivepath      = NULL;
    allowpubsub      = TRUE;
    allowaliensub    = TRUE;
    allowmembers     = FALSE;
    intro_file       = "introduction";
    sig_file         = "signature";
    desc_file        = "description";
    header_file      = "header";
    list_acl_file    = "acl";
    address_file     = "list";
    ack_file         = "acks";

    /* Get the master configuration. */

    MasterConfig = getMasterConfig();

    /* Did we read this config file already? */

    node = FindNodeByKey(ListConfigs, listname);
    if (node != NULL)
	return getNodeData(node);

    /* No? Then read the config file. */

    buffer = text_easy_sprintf("%s/%s/config", MasterConfig->list_dir, listname);
    list_dir = text_easy_sprintf("%s/%s", MasterConfig->list_dir, listname);
    if (stat(buffer, &sb) != 0)
	{
	free(buffer);
	buffer = text_easy_sprintf("%s/%s.config", MasterConfig->list_dir, listname);
	list_dir = MasterConfig->list_dir;
	if (stat(buffer, &sb) != 0)
	    {
	    syslog(LOG_ERR, "Can't find a config file for list \"%s\".", listname);
	    exit(1);
	    }
	}
    rc = ReadConfig(buffer, ListCF);
    if (rc != 0)
	{
	syslog(LOG_ERR, "Failed to parse the list \"%s\"'s config file.", listname);
	exit(1);
	}

    /* Do consistency checks. */

    if (listtype == NULL)
	{
	syslog(LOG_ERR, "List \"%s\" doesn't have a valid type in config file.", listname);
	exit(1);
	}

    /* Set up the list config structure. */

    ListConfig = calloc(sizeof(struct List_Config), 1);
    if (ListConfig == NULL)
	{
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", sizeof(struct List_Config));
	exit(1);
	}
    if (!strcasecmp(listtype, "open"))
	ListConfig->listtype = LIST_OPEN;
    else if (!strcasecmp(listtype, "closed"))
	ListConfig->listtype = LIST_CLOSED;
    else if (!strcasecmp(listtype, "moderated"))
	ListConfig->listtype = LIST_MODERATED;
    else
	{
	syslog(LOG_ERR, "List \"%s\" doesn't have a valid type in config file.", listname);
	exit(1);
	}
    ListConfig->allowpubsub = allowpubsub;
    ListConfig->allowaliensub = allowaliensub;
    ListConfig->allowmembers = allowmembers;
    ListConfig->fqdn = (list_fqdn) ? list_fqdn : MasterConfig->fqdn;
    ListConfig->reply_to = reply_to;
    if (reply_to != NULL && strcasecmp(reply_to, "none"))
	CanonizeAddress(&(ListConfig->reply_to), ListConfig->fqdn);
    ListConfig->admin_password = admin_password;
    ListConfig->posting_password = posting_password;
    ListConfig->postingfilter = postingfilter;

    ListConfig->list_dir      = list_dir;

#define EXPAND(dst, src)                  \
    if (src == NULL || src[0] == '/')     \
	ListConfig->dst = src;            \
    else                                  \
	ListConfig->dst = text_easy_sprintf("%s/%s", ListConfig->list_dir, src);

    EXPAND(archivepath, archivepath);
    EXPAND(intro_file, intro_file);
    EXPAND(desc_file, desc_file);
    EXPAND(sig_file, sig_file);
    EXPAND(header_file, header_file);
    EXPAND(acl_file, list_acl_file);
    EXPAND(address_file, address_file);
    EXPAND(ack_file, ack_file);

    AppendNode(ListConfigs, xstrdup(listname), ListConfig);

    return ListConfig;
    }
