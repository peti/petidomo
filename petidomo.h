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

/********** main.c **********/

extern char g_is_approved;

/********** config.c **********/

struct PD_Config
    {
    char *    fqdn;
    char *    master_password;
    char *    mta;
    char *    mta_options;
    char *    list_dir;
    char *    ack_queue_dir;
    char *    help_file;
    char *    acl_file;
    char *    index_file;
    };

enum list_type_t
    {
    LIST_OPEN,
    LIST_CLOSED,
    LIST_MODERATED,
    LIST_ACKED,
    LIST_ACKED_ONCE
    };

enum subscription_type_t
    {
    SUBSCRIPTION_PUBLIC,
    SUBSCRIPTION_ADMIN,
    SUBSCRIPTION_ACKED
    };

struct List_Config
    {
    unsigned int  listtype;
    unsigned int  subtype;
    int           allowmembers;
    char *        fqdn;
    char *        admin_password;
    char *        posting_password;
    char *        postingfilter;
    char *        archivepath;
    char *        reply_to;
    char *        intro_file;
    char *        desc_file;
    char *        sig_file;
    char *        header_file;
    char *        acl_file;
    char *        list_dir;
    char *        address_file;
    char *        ack_file;
    };

int InitPetidomo(const char* masterconfig_path);
const struct PD_Config *getMasterConfig(void);
const struct List_Config *getListConfig(const char* listname);

/********** rfcparse.c **********/

struct Mail
    {
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

/********** filter.c **********/

int MailFilter(struct Mail *MailStruct, const char *filter);

/********** acl.c **********/

int checkACL(struct Mail *, const char *, int *, char **);
enum
    {
    ACL_DROP,
    ACL_PASS,
    ACL_APPROVE,
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

/********** approve.c **********/

int approve_main(char *incoming_mail);

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

/********** unsubscribe.c **********/

int DeleteAddress(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** hermes.c **********/

int hermes_main(char *incoming_mail, const char *listname);

/********** subscribe.c **********/

int AddAddress(struct Mail *MailStruct, const char *param1, const char *param2, const char *defaultlist);

/********** parsearray.c **********/

struct Parse
    {
    const char *    keyword;
    int             (*handleCommand)(struct Mail *, const char *, const char *, const char *);
    };
extern struct Parse ParseArray[];

/********** generate_cookie.c **********/

char* generate_cookie(const char*);

/********** queue_posting.c **********/

char* queue_posting(const struct Mail* mail, const char* listname);

/********** queue_command.c **********/

char* queue_command(const struct Mail* mail, const char* command);

/********** address_db.c **********/

int is_address_on_list(const char* file, const char* address);
int add_address(const char* file, const char* address);

#endif /* !defined(__PETIDOMO_H__) */
