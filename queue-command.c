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

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libtext/text.h"
#include "petidomo.h"

char* queue_command(const struct Mail* mail, const char* command)
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
	syslog(LOG_ERR, "Opening ack spool file \"%s\" failed: %s", buffer, strerror(errno));
	exit(1);
	}
    fprintf(fh, "#!/bin/sh\n");
    fprintf(fh, "%s \\\n--masterconf=%s \\\n--mode=listserv --approved \\\n<<[end-of-%s-marker]\n", who_am_i, masterconfig_path, cookie);
    fprintf(fh, "Sender: %s\n", mail->Envelope);
    fprintf(fh, "From: %s\n", mail->From);
    if (mail->Reply_To)
	fprintf(fh, "Reply-To: %s\n", mail->Reply_To);
    if (mail->Message_Id)
	fprintf(fh, "Message-Id: %s\n", mail->Message_Id);
    if (mail->Approve)
	fprintf(fh, "Approve: %s\n", mail->Approve);
    fprintf(fh, "\n");
    fprintf(fh, "%s\n", command);
    fprintf(fh, "[end-of-%s-marker]\n", cookie);
    fclose(fh);
    chmod(buffer, 0755);
    free(buffer);
    return cookie;
    }
