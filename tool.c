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

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>

#include "libtext/text.h"
#include "petidomo.h"

bool
isSubscribed(const char * listname, const char * address,
             char ** listfile, char ** subscriber, bool dofuzzy)
    {
    const struct List_Config * ListConfig;
    struct stat    sb;
    char *         list;
    char *         p;
    unsigned int   len;
    bool           rc;

    if (isValidListName(listname))
        ListConfig = getListConfig(listname);
    else
        return FALSE;

    if (stat(ListConfig->address_file, &sb) != 0)
        return FALSE;
    list = loadfile(ListConfig->address_file);
    if (list == NULL)
        return FALSE;

    for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p))
        {
        if (strncasecmp(p, address, len) == 0 &&
            (p == list || p[-1] == '\n') &&
            (isspace((int)p[len]) || p[len] == '\0'))
            {
            break;
            }
        }

    if (*p == '\0' && dofuzzy == TRUE)
        {
        address = buildFuzzyMatchAddress(address);
        if (address != NULL)
            {
            for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p))
                {
                if (text_easy_pattern_match(p, address) == TRUE &&
                    (p == list || p[-1] == '\n'))
                    {
                    break;
                    }
                }
            }
        }


    /* Save the returncode now, because p may be invalid in a few
       moments. */

    rc = ((*p != '\0') ? TRUE : FALSE);

    /* Did the caller want results back? Then give them to him. */

    if (listfile != NULL)
        {
        *listfile = list;
        if (subscriber != NULL)
            *subscriber = (*p != '\0') ? p : NULL;
        }
    else
        free(list);

    /* Return the result. */

    return rc;
    }

char *
buildFuzzyMatchAddress(const char * address)
{
    char *   fuzzyaddress;
    int      rc;

    fuzzyaddress = xmalloc(strlen(address)+16);
    rc = text_transform_text(fuzzyaddress, address, "([^@]+)@[^\\.]+\\.([^\\.]+\\..*)",
                       "\\1@([^\\\\.]+\\\\.)?\\2");
    if (rc == TEXT_REGEX_TRANSFORM_DIDNT_MATCH) {
        rc = text_transform_text(fuzzyaddress, address, "([^@]+)@([^\\.]+\\.[^\\.]+)",
                       "\\1@([^\\\\.]+\\\\.)?\\2");
    }

    switch (rc) {
      case TEXT_REGEX_ERROR:
          syslog(LOG_CRIT, "Internal error in buildFuzzyMatchAddress(): "\
              "Regular expression can't be compiled.");
          break;
      case TEXT_REGEX_TRANSFORM_DIDNT_MATCH:
          break;
      case TEXT_REGEX_OK:
          return fuzzyaddress;
      default:
          syslog(LOG_CRIT, "Internal error: Unexpected returncode in ParseMessageIdLine().");
    }
    free(fuzzyaddress);
    return NULL;
}



bool
isValidListName(const char * listname)
    {
    struct stat   sb;
    char *        buffer;
    const struct PD_Config * MasterConfig = getMasterConfig();

    assert(listname != NULL);

    if ((strchr(listname, '/') != NULL) || (strchr(listname, ':') != NULL))
        return FALSE;

    buffer = text_easy_sprintf("%s/%s/config", MasterConfig->list_dir, listname);
    if (stat(buffer, &sb) != 0)
        {
        free(buffer);
        buffer = text_easy_sprintf("%s/%s/conf", MasterConfig->list_dir, listname);
        if (stat(buffer, &sb) != 0)
            {
            free(buffer);
            buffer = text_easy_sprintf("%s/%s.config", MasterConfig->list_dir, listname);
            if (stat(buffer, &sb) != 0)
                {
                free(buffer);
                buffer = text_easy_sprintf("%s/%s.conf", MasterConfig->list_dir, listname);
                if (stat(buffer, &sb) != 0)
                    {
                    free(buffer);
                    return FALSE;
                    }
                }
            }
        }
    free(buffer);
    return TRUE;
    }
