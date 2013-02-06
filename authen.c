/*
 * Copyright (c) 1995-2013 Peter Simons <simons@cryp.to>
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

#include <string.h>
#include <ctype.h>

#include "libtext/text.h"
#include "petidomo.h"

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

    if (listname != NULL)
        {
        ListConfig = getListConfig(listname);

        if (ListConfig->posting_password == NULL)
            return FALSE;

        if (!strcasecmp(ListConfig->posting_password, password))
            return TRUE;
        }

    return FALSE;
    }
