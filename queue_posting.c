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
#include "libtext/text.h"
#include "petidomo.h"

char* queue_posting(const struct Mail* mail, const char* listname)
    {
    const struct PD_Config   * MasterConfig = getMasterConfig();
    char*   buffer;
    char*   cookie;
    FILE*   fh;

    cookie = generate_cookie(mail->Header);
    buffer = text_easy_sprintf("%s/%s", MasterConfig->ack_queue_dir, cookie);
    fh = fopen(buffer, "w");
    if (fh == NULL)
	{
	syslog(LOG_ERR, "Opening ack spool file \"%s\" failed: %m", buffer);
	exit(1);
	}
    fprintf(fh, "#! /bin/sh\n");
    fprintf(fh, "\n");
    fprintf(fh, "%s --mode=deliver --listname=%s --approved <<[end-of-%s-marker]\n", who_am_i, listname, cookie);
    fprintf(fh, "%s\n", mail->Header);
    fprintf(fh, "%s", mail->Body);
    fprintf(fh, "[end-of-%s-marker]\n", cookie);
    fclose(fh);
    chmod(buffer, 0755);
    free(buffer);
    return cookie;
    }
