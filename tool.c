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
    struct stat    sb;
    char *         buffer;
    char *         list;
    char *         p;
    unsigned int   len;
    bool            rc;

    buffer = text_easy_sprintf("lists/%s/list", listname);
    if (stat(buffer, &sb) != 0)
      return FALSE;
    list = loadfile(buffer);
    if (list == NULL)
      return FALSE;

    for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p)) {
	if (strncasecmp(p, address, len) == 0 &&
	    (p == list || p[-1] == '\n') &&
	    (isspace((int)p[len]) || p[len] == '\0')) {
	    break;
	}
    }

    if (*p == '\0' && dofuzzy == TRUE) {
	address = buildFuzzyMatchAddress(address);
	if (address != NULL) {
	    for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p)) {
		if (text_easy_pattern_match(p, address) == TRUE &&
		    (p == list || p[-1] == '\n')) {
		    break;
		}
	    }
	}
    }


    /* Save the returncode now, because p may be invalid in a few
       moments. */

    rc = ((*p != '\0') ? TRUE : FALSE);

    /* Did the caller want results back? Then give them to him. */

    if (listfile != NULL) {
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

    assert(listname != NULL);

    if ((strchr(listname, '/') != NULL) || (strchr(listname, ':') != NULL))
	return FALSE;

    buffer = text_easy_sprintf("lists/%s", listname);
    if (stat(buffer, &sb) != 0)
      return FALSE;		/* Doesn't exist at all. */
    else if ((sb.st_mode & S_IFDIR) == 0)
      return FALSE;		/* Entry isn't a directory. */
    else {
	buffer = text_easy_sprintf("lists/%s/config", listname);
	if (stat(buffer, &sb) != 0)
	  return FALSE;
    }
    return TRUE;
}
