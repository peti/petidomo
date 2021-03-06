/*
 * Copyright (c) 1995-2019 Peter Simons <simons@cryp.to>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

int
GenIndex(struct Mail * MailStruct,
                const char * param1,
                const char * param2,
                const char * defaultlist)
{
    const struct PD_Config * MasterConfig = getMasterConfig();
    FILE *           fh;
    const char *     address = NULL;
    char             from[4096];
    char             envelope[4096];
    char *           p;

    address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    /* Initialize internal stuff. */

    MasterConfig = getMasterConfig();
    sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    if (defaultlist != NULL)
      sprintf(from, "%s-request@%s", defaultlist, MasterConfig->fqdn);
    else
      sprintf(from, "petidomo@%s", MasterConfig->fqdn);

    /* Open the mailer. */

    fh = vOpenMailer(envelope, address, NULL);
    if (fh == NULL) {
        syslog(LOG_ERR, "Failed to send mail to \"%s\": %s", address, strerror(errno));
        return -1;
    }
    fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", from);
    fprintf(fh, "To: %s\n", address);
    fprintf(fh, "Subject: Petidomo: Your request \"index\"\n");
    if (MailStruct->Message_Id != NULL)
      fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
    fprintf(fh, "Precedence: junk\n");
    fprintf(fh, "Sender: %s\n", envelope);
    fprintf(fh, "\n");
    p = loadfile(MasterConfig->index_file);
    if (p != NULL)
        {
        fprintf(fh, "%s\n", p);
        free(p);
        }
    else
        {
        syslog(LOG_ERR, "There is no index file for Petidomo!");
        fprintf(fh, "No index available.\n");
        }
    CloseMailer(fh);
    return 0;
}
