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

#include <config.h>

#include <string.h>
#include <errno.h>
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
        syslog(LOG_ERR, "Opening ack spool file \"%s\" failed: %s", buffer, strerror(errno));
        exit(EXIT_FAILURE);
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
