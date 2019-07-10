%{
/*
 * Copyright (c) 1995-2019 Peter Simons <simons@cryp.to>
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

/* Definitions we need in the parser. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

static int acl_error(const char *);
int acl_lex(void);
static int domatch(int, int, char *);
static int dofilter(const char *);

unsigned int lineno;
int operation, g_rc;
char * g_parameter = NULL;
struct Mail * g_MailStruct;

/* Forward declarations for the scanner generated from acl-scanner.l. */
extern char *acl_text;
extern FILE *acl_in;

#define YYERROR_VERBOSE
%}
%define api.prefix {acl_}
%token TOK_IF TOK_EQUAL TOK_FROM TOK_SUBJECT
%token TOK_ENVELOPE TOK_HEADER TOK_BODY TOK_AND TOK_OR TOK_NOT
%token TOK_THEN TOK_MATCH TOK_STRING TOK_DROP TOK_PASS TOK_APPROVE
%token TOK_REDIRECT TOK_FORWARD TOK_REJECT TOK_REJECTWITH
%token TOK_FILTER
%left TOK_AND
%left TOK_OR
%right TOK_NOT
%%

input:    /* empty */
        | input statmt
;

statmt:   ';'
        | TOK_IF exp TOK_THEN action ';'  { if ($2 == TRUE) {
                                                 operation = $4;
                                                 YYACCEPT;
                                            }
                                          }
;

exp:      qualifier TOK_EQUAL TOK_STRING  {
                                            g_rc = domatch($1, TOK_EQUAL, acl_text);
                                            if (g_rc == -1)
                                               YYABORT;
                                            $$ = g_rc;
                                          }
        | qualifier TOK_MATCH TOK_STRING  {
                                            g_rc = domatch($1, TOK_MATCH, acl_text);
                                            if (g_rc == -1)
                                               YYABORT;
                                            $$ = g_rc;
                                          }
        | TOK_STRING                      {
                                            g_rc = dofilter(acl_text);
                                            if (g_rc == -1)
                                               YYABORT;
                                            $$ = g_rc;
                                          }
        | exp TOK_OR exp                  { $$ = $1 || $3; }
        | exp TOK_AND exp                 { $$ = $1 && $3; }
        | TOK_NOT exp                     { $$ = ! $2; }
        | '(' exp ')'                     { $$ = $2; }
;

qualifier: TOK_FROM                       { $$ = TOK_FROM; }
        | TOK_SUBJECT                     { $$ = TOK_SUBJECT; }
        | TOK_ENVELOPE                    { $$ = TOK_ENVELOPE; }
        | TOK_HEADER                      { $$ = TOK_HEADER; }
        | TOK_BODY                        { $$ = TOK_BODY; }
;

action:   TOK_PASS                        { $$ = ACL_PASS; }
        | TOK_APPROVE                     { $$ = ACL_APPROVE; }
        | TOK_DROP                        { $$ = ACL_DROP; }
        | TOK_REJECT                      { $$ = ACL_REJECT; }
        | TOK_REJECTWITH TOK_STRING       {
                                            $$ = ACL_REJECTWITH;
                                            free(g_parameter);
                                            g_parameter = strdup(acl_text);
                                            if (g_parameter == NULL)
                                              YYABORT;
                                          }
        | TOK_REDIRECT TOK_STRING         {
                                            $$ = ACL_REDIRECT;
                                            free(g_parameter);
                                            g_parameter = strdup(acl_text);
                                            if (g_parameter == NULL)
                                              YYABORT;
                                          }
        | TOK_FORWARD TOK_STRING          {
                                            $$ = ACL_FORWARD;
                                            free(g_parameter);
                                            g_parameter = strdup(acl_text);
                                            if (g_parameter == NULL)
                                              YYABORT;
                                          }
        | TOK_FILTER TOK_STRING           {
                                            $$ = ACL_FILTER;
                                            free(g_parameter);
                                            g_parameter = strdup(acl_text);
                                            if (g_parameter == NULL)
                                              YYABORT;
                                          }
;
%%
/***** internal routines *****/

static int
acl_error(const char * string)
{
    syslog(LOG_ERR, "syntax error in line %u: %s\n", lineno, string);
    return 0;
}


static int
dofilter(const char * filter)
{
    FILE *  fh;
    int     rc;

    fh = popen(filter, "w");
    if (fh == NULL) {
        syslog(LOG_ERR, "failed to open ACL-filter \"%s\": %s", filter, strerror(errno));
        return -1;
    }
    fprintf(fh, "%s\n", g_MailStruct->Header);
    fprintf(fh, "%s", g_MailStruct->Body);
    rc = pclose(fh);

    if (!WIFEXITED(rc))
      return -1;

    rc = WEXITSTATUS(rc);
    switch(rc) {
      case 0:
          return TRUE;
      case 1:
          return FALSE;
      default:
          syslog(LOG_ERR, "acl filter \"%s\" returned unexpected value %d.", filter, rc);
          return -1;
    }
}


static int
domatch(int qualifier, int oper, char * string)
{
    char *   left;

    switch(qualifier) {
      case TOK_FROM:
          left = g_MailStruct->From;
          break;
      case TOK_SUBJECT:
          left = g_MailStruct->Subject;
          break;
      case TOK_ENVELOPE:
          left = g_MailStruct->Envelope;
          break;
      case TOK_HEADER:
          left = g_MailStruct->Header;
          break;
      case TOK_BODY:
          left = g_MailStruct->Body;
          break;
      default:
          syslog(LOG_CRIT, "Internal error in the ACL parser. Unknown qualifier %d.",
              qualifier);
          return -1;
    }

    switch(oper) {
      case TOK_EQUAL:
          if (left != NULL && strcasecmp(left, string) == 0) {
              return TRUE;
          }
          else {
              return FALSE;
          }
      case TOK_MATCH:
          if (left != NULL && text_easy_pattern_match(left, string) == TRUE) {
              return TRUE;
          }
          else {
              return FALSE;
          }
      default:
          syslog(LOG_CRIT, "Internal error in the ACL parser. Unknown operator %d.", oper);
          return -1;
    }
}


/****** public routines ******/

int checkACL(struct Mail *   MailStruct,
             const char *    listname,
             int *           operation_ptr,
             char **         parameter_ptr,
             acl_type_t      type)
{
    extern void acl_reset_lexer();

    const struct PD_Config * MasterConfig;
    const struct List_Config * ListConfig;
    int     rc;

    assert(MailStruct != NULL);
    assert(operation_ptr != NULL);
    assert(parameter_ptr != NULL);

    MasterConfig = getMasterConfig();
    g_MailStruct = MailStruct;
    g_parameter = NULL;

    /* Set up the lex scanner. */

    acl_reset_lexer();
    lineno = 1; operation = ACL_NONE;

    /* First check the mail against the master acl file. */

    acl_in = fopen((type == ACL_PRE ? MasterConfig->acl_file_pre : MasterConfig->acl_file_post), "r");
    if (acl_in == NULL) {
        switch(errno) {
          case ENOENT:
              /* no master acl file */
              syslog(LOG_WARNING, "You have no global acl file (%s). This is probably not a good idea.",
                     (type == ACL_PRE ? MasterConfig->acl_file_pre : MasterConfig->acl_file_post));
              goto check_local_acl_file;
          default:
              syslog(LOG_ERR, "Couldn't open \"%s\" acl file: %s",
                     (type == ACL_PRE ? MasterConfig->acl_file_pre : MasterConfig->acl_file_post), strerror(errno));
              return -1;
        }
    }

    /* Parse the acl file. */

    rc = yyparse();
    if (acl_in != NULL) {
        fclose(acl_in);
        acl_in = NULL;
    }
    if (rc != 0) {
        syslog(LOG_ERR, "Parsing \"%s\" file returned with an error.",
               (type == ACL_PRE ? MasterConfig->acl_file_pre : MasterConfig->acl_file_post));
        return -1;
    }

    /* If we had a hit, return now. */

    if (operation != ACL_NONE)
      goto finished;



check_local_acl_file:

    /* Do we have a local acl file to test? */

    if (listname == NULL)
      goto finished;

    /* Set up the lex scanner. */

    acl_reset_lexer();
    lineno = 1; operation = ACL_NONE;

    ListConfig = getListConfig(listname);
    acl_in = fopen((type == ACL_PRE ? ListConfig->acl_file_pre : ListConfig->acl_file_post), "r");
    if (acl_in == NULL) {
        switch(errno) {
          case ENOENT:
              /* no list acl file */
	      goto finished;
          default:
              syslog(LOG_ERR, "Couldn't open acl file \"%s\": %s",
                     (type == ACL_PRE ? ListConfig->acl_file_pre : ListConfig->acl_file_post),
                     strerror(errno));
              return -1;
        }
    }

    rc = yyparse();
    fclose(acl_in);
    acl_in = NULL;
    if (rc != 0) {
        syslog(LOG_ERR, "Parsing \"%s\" file returned with an error.",
               (type == ACL_PRE ? ListConfig->acl_file_pre : ListConfig->acl_file_post));
        return -1;
    }

    /* Return to the caller. */
finished:
    *operation_ptr = operation;
    *parameter_ptr = g_parameter;
    return 0;
}
