/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <ctype.h>

#include <petidomo.h>

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
