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

#include <ctype.h>
#include "petidomo.h"

static const char * s_password = NULL;
extern char *       g_currLine;

int
setPassword(struct Mail * MailStruct,
		const char * param1,
		const char * param2,
		const char * defaultlist)
{
    char *         p;
    char *         q;

    debug((DEBUG_COMMAND, 3, "setPassword(\"%s\").", param1));

    /* Find the beginning of the parameter. */

    p = g_currLine;
    while(*p && !isspace((int)*p))
      p++;
    while(*p && isspace((int)*p))
      p++;

    /* If the rest is empty, there ain't no fucking password. */

    if (*p == '\0' || strlen(p) == 0)
      return 0;

    /* Cut trailing blanks. */

    q = p + strlen(p);
    while(isspace((int)q[-1]))
      q--;
    *q = '\0';

    /* Okay, check for quotes and that's it then. */

    if (*p == '\"' && q[-1] == '\"') {
	p++;
	q[-1] = '\0';
    }

    /* Store the result. */

    debug((DEBUG_COMMAND, 2, "Setting current password to \"%s\".", p));
    s_password = p;

    return 0;
}

const char *
getPassword(void)
{
    return s_password;
}
