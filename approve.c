/*
   $Source$
   $Revision$

   Copyright (C) 2000 by Peter Simons <simons@computer.org>.

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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <errno.h>

#include "petidomo.h"

void approve_main(char* mail)
    {
    const struct PD_Config* MasterConfig = getMasterConfig();
    struct Mail*   MailStruct;
    char*          originator;
    static const char* cookie_regex = "[0-9a-f]{32}";
    regex_t preg;
    regmatch_t match[3];
    int offset;
    int number_of_hits            = 0;
    int number_of_successful_hits = 0;

    if (chdir(MasterConfig->ack_queue_dir) == -1)
	{
	syslog(LOG_ERR, "Can't change directory to \"%s\": %s", MasterConfig->ack_queue_dir, strerror(errno));
	exit(1);
	}

    if (regcomp(&preg, cookie_regex, REG_EXTENDED | REG_ICASE) != 0)
	{
	syslog(LOG_CRIT, "Can't compile my internal regular expressions. This is serious!");
	exit(1);
	}

    offset = 0;
    while(regexec(&preg, mail + offset, sizeof(match)/sizeof(regmatch_t), match, 0) == 0)
	{
	struct stat sb;
	char buffer[33];
	char* src;
	char* dst = buffer;
	unsigned int i;

	++number_of_hits;

	/* Copy found string into buffer. */

	src = mail + offset + match[0].rm_so;
	for (i = 0; i < 32; ++i)
	    *dst++ = *src++;
	*dst = '\0';

	/* Correct offset for the next match. */

	offset += match[0].rm_so + 1;

	/* Do we have a hit here? If, execute the file and remove it.
	   Then go on. */

	if (stat(buffer, &sb) == 0)
	    {
	    char cmd[128];

	    ++number_of_successful_hits;
	    sprintf(cmd, "/bin/sh %s && /bin/rm -f %s", buffer, buffer);
	    if (((signed char)system(cmd)) == -1)
		{
		syslog(LOG_ERR, "system() failed for \"%s\": %s", buffer, strerror(errno));
		exit(1);
		}
	    }
	}

    /* Report results back to the originator */

    if (ParseMail(&MailStruct, mail, MasterConfig->fqdn) != 0)
	{
	syslog(LOG_ERR, "Parsing the incoming mail failed.");
	exit(-1);
	}

    if (MailStruct->From == NULL)
	{
        syslog(LOG_NOTICE, "Received mail without From: line.");
        return;
	}

    originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    }
