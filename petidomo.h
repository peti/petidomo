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

#ifndef __PETIDOMO_H__
#define __PETIDOMO_H__ 1


/********** Useful defines and declarations **********/

#ifndef __HAVE_DEFINED_BOOL__
#    define __HAVE_DEFINED_BOOL__ 1
     typedef int bool;
#endif

#ifndef DEBUG_DMALLOC
#    define xmalloc(size)    malloc(size)
#    define xstrdup(string)  strdup(string)
#endif

#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE 1

/********** Includes all modules need **********/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <syslog.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "libargv/argv.h"
#include "debug.h"

/********** config.c **********/

struct PD_Config {
    char *    basedir;
    char *    fqdn;
    char *    master_password;
    char *    mta;
    char *    mta_options;
    int       detach;
    int       show_stats;
};

enum {
    LIST_OPEN,
    LIST_CLOSED,
    LIST_MODERATED
};

struct List_Config {
    unsigned int  listtype;
    int           allowpubsub;
    int           allowaliensub;
    int           allowmembers;
    int           showonindex;
    char *        fqdn;
    char *        admin_password;
    char *        posting_password;
    char *        postingfilter;
    char *        archivepath;
    char *        reply_to;
};

int InitPetidomo(void);
const struct PD_Config *getMasterConfig(void  );
const struct List_Config *getListConfig(const char *listname);

/********** rfcparse.c **********/

struct Mail {
    char *     Header;
    char *     Body;
    char *     Envelope;
    char *     From;
    char *     Subject;
    char *     Reply_To;
    char *     Message_Id;
    char *     Approve;
    char *     ListSignature;
};

void RemoveCarrigeReturns(char *buffer);
int isRFC822Address(const char *buffer);
int ParseAddressLine(char *buffer);
int ParseReplyToLine(char *buffer);
int ParseFromLine(char *buffer);
int ParseMessageIdLine(char *buffer);
int ParseApproveLine(char *buffer);
void CanonizeAddress(char **buffer, const char *fqdn);
int ParseMail(struct Mail **result, char *incoming_mail, const char *fqdn);

/********** io.c **********/

char *LoadFromDescriptor(int fd);
char *loadfile(const char *filename);
int savefile(const char *filename, const char *buffer);

/********** archive.c **********/

int ArchiveMail(const struct Mail *MailStruct, const char *listname);

/********** authen.c **********/

int FindBodyPassword(struct Mail *MailStruct);
int isValidAdminPassword(const char *password, const char *listname);
int isValidPostingPassword(const char *password, const char *listname);

/********** exit.c **********/

void RescueMail(const char *mail);
void RemoveRescueMail(void);

/********** filter.c **********/

int MailFilter(struct Mail *MailStruct, const char *filter);

/********** acl.c **********/

int checkACL(struct Mail *, const char *, int *, char **);

enum {
    ACL_DROP,
    ACL_PASS,
    ACL_REDIRECT,
    ACL_FORWARD,
    ACL_REJECT,
    ACL_REJECTWITH,
    ACL_FILTER,
    ACL_NONE
};

/********** handleacl.c **********/

int handleACL(struct Mail *MailStruct, const char *listname, int operation, char *parameter);

/********** help.c **********/

int SendHelp(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);
int Indecipherable(struct Mail *MailStruct, const char *defaultlist);

/********** index.c **********/

int GenIndex(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** io.c **********/

char *LoadFromDescriptor(int fd);
char *loadfile(const char *filename);
int savefile(const char *filename, const char *buffer);

/********** listserv.c **********/

int listserv_main(char *incoming_mail, char *default_list);

/********** mailer.c **********/

FILE *OpenMailer(const char *envelope, const char *recipients[]);
FILE *vOpenMailer(const char *envelope, ...  );
int CloseMailer(FILE * fh);
int ListMail(const char *envelope, const char *listname, const struct Mail *MailStruct);

/********** members.c **********/

int SendSubscriberList(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** password.c **********/

int setPassword(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);
const char *getPassword(void  );

/********** tool.c **********/

char *buildFuzzyMatchAddress(const char *);
int isValidListName(const char *);
bool isSubscribed(const char *, const char *, char **, char **, bool);

/********** signature.c **********/

void AppendSignature(FILE * fh);

/********** unsubscribe.c **********/

int DeleteAddress(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** argvSetDebugLevel.c **********/

int argvSetDebugLevel(argv_array_t debug);

/********** hermes.c **********/

int hermes_main(char *incoming_mail, const char *listname);

/********** subscribe.c **********/

int AddAddress(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** parsearray.c **********/

struct Parse {
    const char *    keyword;
    int             (*handleCommand)(struct Mail *, const char *, const char *, const
 char *);
};
extern struct Parse ParseArray[];

#endif /* !defined(__PETIDOMO_H__) */