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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "petidomo.h"
#include "libtext/text.h"

void approve_main(char* mail)
    {
    const struct PD_Config* MasterConfig = getMasterConfig();
    struct Mail*   MailStruct;
    char*          originator;
    char*          envelope;
    FILE*          fh;
    static const char* cookie_regex = "[0-9a-f]{32}";
    regex_t preg;
    regmatch_t match[3];
    int offset;
    int number_of_hits = 0;

    if (chdir(MasterConfig->ack_queue_dir) == -1)
        {
        syslog(LOG_ERR, "Can't change directory to \"%s\": %s", MasterConfig->ack_queue_dir, strerror(errno));
        exit(EXIT_FAILURE);
        }

    if (regcomp(&preg, cookie_regex, REG_EXTENDED | REG_ICASE) != 0)
        {
        syslog(LOG_CRIT, "Can't compile my internal regular expressions. This is serious!");
        exit(EXIT_FAILURE);
        }

    offset = 0;
    while(regexec(&preg, mail + offset, sizeof(match)/sizeof(regmatch_t), match, 0) == 0)
        {
        struct stat sb;
        char buffer[33];
        char* src;
        char* dst = buffer;
        unsigned int i;

        /* Copy found string into buffer and convert it to all
           upper-case while doing so. */

        src = mail + offset + match[0].rm_so;
        for (i = 0; i < 32; ++i)
            *dst++ = toupper(*src++);
        *dst = '\0';

        /* Correct offset for the next match. */

        offset += match[0].rm_so + 1;

        /* Do we have a hit here? If, execute the file and remove it.
           Then go on. */

        if (stat(buffer, &sb) == 0)
            {
            char cmd[128];

            syslog(LOG_INFO, "Received valid approval code for spool file \"%s\".", buffer);
            ++number_of_hits;
            sprintf(cmd, "/bin/sh %s && /bin/rm -f %s", buffer, buffer);
            if (((signed char)system(cmd)) == -1)
                {
                syslog(LOG_ERR, "system() failed for \"%s\": %s", buffer, strerror(errno));
                exit(EXIT_FAILURE);
                }
            }
        else
            syslog(LOG_INFO, "Received approval code \"%s\", but there is no corresponding posting.", buffer);
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
    envelope = text_easy_sprintf("petidomo-manager@%s", MasterConfig->fqdn);

    fh = vOpenMailer(envelope, originator, NULL);
    if (fh != NULL)
        {
        fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n", MasterConfig->fqdn);
        fprintf(fh, "To: %s\n", originator);
        fprintf(fh, "Subject: Petidomo: Your approval mail has been received\n");
        if (MailStruct->Message_Id != NULL)
            fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
        fprintf(fh, "Precedence: junk\n");
        fprintf(fh, "Sender: %s\n", envelope);
        fprintf(fh, "\n");
        if (number_of_hits > 0)
            fprintf(fh, "Your approval mail has been received and been processed sucessfully.");
        else
            fprintf(fh, "I couldn't find any approval codes in your mail.");
        CloseMailer(fh);
        }
    else
        {
        syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
        exit(-1);
        }
    }
