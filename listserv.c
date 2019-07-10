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

#include <config.h>

#include <string.h>
#include <ctype.h>

#include "libtext/text.h"
#include "petidomo.h"

char * g_currLine;              /* pointer to the line currently parsed */

void listserv_main(char * incoming_mail, char * default_list)
{
    const struct List_Config * ListConfig;
    struct Mail *   MailStruct;
    char *          nextLine;
    char *          parameter;
    char            param1[512], param2[512];
    char            keyword[32];
    int             j, i, junklines, rc, found, operator;

    /* Initialize internals. */

    if (default_list != NULL)
      ListConfig = getListConfig(default_list);
    else
      ListConfig = NULL;

    /* Parse the incoming mail. */

    rc = ParseMail(&MailStruct, incoming_mail,
                   (ListConfig != NULL) ? ListConfig->fqdn : NULL);
    if (rc != 0) {
        syslog(LOG_ERR, "Parsing the incoming mail failed.");
        exit(rc);
    }

    /* Do sanity checks. */

    if (MailStruct->Envelope == NULL) {
        syslog(LOG_NOTICE, "Received mail without a valid envelope.");
        return;
    }
    if (MailStruct->From == NULL) {
        syslog(LOG_NOTICE, "Received mail without From: line.");
        return;
    }

    /* Do access control. */

    if (checkACL(MailStruct, NULL, &operator, &parameter, ACL_PRE) != 0) {
        syslog(LOG_ERR, "checkACL() failed with an error.");
        exit(EXIT_FAILURE);
    }
    rc = handleACL(MailStruct, NULL, operator, parameter);
    switch(rc) {
      case -1:
          syslog(LOG_ERR, "handleACL() failed with an error.");
          exit(EXIT_FAILURE);
      case 0:
          break;
      case 1:
          return;
    }
    if (checkACL(MailStruct, NULL, &operator, &parameter, ACL_POST) != 0) {
        syslog(LOG_ERR, "checkACL() failed with an error.");
        exit(EXIT_FAILURE);
    }
    rc = handleACL(MailStruct, NULL, operator, parameter);
    switch(rc) {
      case -1:
          syslog(LOG_ERR, "handleACL() failed with an error.");
          exit(EXIT_FAILURE);
      case 0:
          break;
      case 1:
          return;
    }

    /* Parse the body and call the apropriate routines for each
       command. */

    g_currLine = MailStruct->Body;
    if (*g_currLine == '\0') {
        SendHelp(MailStruct, NULL, NULL, default_list);
        return;
    }
    for (nextLine = text_find_next_line(g_currLine), junklines = 0, found = 0;
         *g_currLine != '\0' && junklines <= 7;
         g_currLine = nextLine, nextLine = text_find_next_line(g_currLine)) {

        /* remove trailing \n */

        if (nextLine[-1] == '\n')
          nextLine[-1] = '\0';

        /* Skip comments, signature and empty lines. */

        if (*g_currLine == '\0' || *g_currLine == '#')
            continue;
        if (!strcmp(g_currLine, "-- "))
            break;

        /* Log contents of current line. */

        syslog(LOG_INFO, "%s: %s",
            ((MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From), g_currLine);

        /* Check whether we have a routine for that command. */

        for (j = 0; !isspace((int)g_currLine[j]) && j < (sizeof(keyword)-1); j++)
          keyword[j] = g_currLine[j];
        keyword[j] = '\0';
        for (i = 0; (&(ParseArray[i]))->keyword != NULL; i++) {
            if (strcasecmp(keyword, (&(ParseArray[i]))->keyword) == 0) { /* hit */
                rc = sscanf(g_currLine, "%*s%511s%511s", param1, param2);
                rc = ((&(ParseArray[i]))->handleCommand)(MailStruct,
                                                         ((rc >= 1) ? param1 : NULL),
                                                         ((rc == 2) ? param2 : NULL),
                                                         default_list);
                if (rc != 0) {
                    syslog(LOG_ERR, "Error occured while handling command.");
                    exit(EXIT_FAILURE);
                }
                found++;
                break;
            }
        }

        if ((&(ParseArray[i]))->keyword == NULL) {

            /* No valid command. */

            junklines++;
        }
    }

    if (junklines > 7)
      syslog(LOG_INFO, "Too many junk lines, ignoring rest of the mail.");

    if (found == 0) {
        syslog(LOG_INFO, "No valid command found, sending help file back to \"%s\".",
            ((MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From));
        Indecipherable(MailStruct, default_list);
    }
}
