/*
   $Source$
   $Revision$

   Copyright (C) 2000 by Peter Simons <simons@computer.org>

   This file is part of Petidomo.

   Petidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Petidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
*/

#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "libtext/text.h"
#include "petidomo.h"

/*
 * rc ==  0: Address is not on list.
 * rc == -1: Error occured while reading the file.
 * rc ==  1: Address is on list.
 */
int is_address_on_list(const char* file, const char* address)
    {
    char *         list;
    char *         p;
    unsigned int   len;
    int            rc;

    list = loadfile(file);
    if (list == NULL)
	{
	if (errno == ENOENT)
	    return 0;
	else
	    return -1;
	}

    for (len = strlen(address), p = list; *p != '\0'; p = text_find_next_line(p))
	{
	if (strncasecmp(p, address, len) == 0 &&
	    (p == list || p[-1] == '\n') &&
	    (isspace((int)p[len]) || p[len] == '\0'))
	    {
	    break;
	    }
	}

    rc = ((*p != '\0') ? 1 : 0);
    free(list);
    return rc;
    }

int add_address(const char* file, const char* address)
    {
    FILE* fh;
    int rc = is_address_on_list(file, address);
    if (rc != 0)
	return rc;

    fh = fopen(file, "a");
    if (fh == NULL)
	{
	syslog(LOG_ERR, "Failed to open file \"%s\" for writing: %s", file, strerror(errno));
	return -1;
	}
    fprintf(fh, "%s\n", address);
    fclose(fh);
    return 0;
    }
