/*
 * Copyright (c) 1995-2010 Peter Simons <simons@cryp.to>
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

#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

#ifndef ARG_NUM_MAX
#  define ARG_NUM_MAX 4096
#endif
#ifndef ARG_MAX
#  define ARG_MAX 4096
#endif

static char *
my_strcpy(char * dst, const char * src)
    {
    while((*dst++ = *src++) != '\0')
        ;
    return dst-1;
    }

FILE *
OpenMailer(const char * envelope, const char * recipients[])
    {
    assert(1==0);
    return NULL;
    }

FILE *
vOpenMailer(const char * envelope, ...)
    {
    const struct PD_Config *   MasterConfig;
    va_list                    ap;
    FILE *                     fh;
    char *                     cmdline;
    char *                     p;
    const char *               q;
    const char *               options;
    unsigned int               cmdline_len;

    MasterConfig = getMasterConfig();

    /* Determine the length of the required buffer. */

    cmdline_len = strlen(MasterConfig->mta);
    cmdline_len += strlen(MasterConfig->mta_options);
    cmdline_len += strlen(envelope);
    va_start(ap, envelope);
    while ((q = va_arg(ap, const char *)) != NULL) {
        cmdline_len += strlen(q) + 1;
    }
    va_end(ap);
    cmdline = xmalloc(cmdline_len+8); /* we don't take any risks :) */

    /* Copy the mta's path and name into the buffer. */

    p = my_strcpy(cmdline, MasterConfig->mta);
    *p++ = ' ';

    /* Copy the mta's options into the array, while replacing '%s'
       with the envelope. */

    for (options = MasterConfig->mta_options; *options != '\0'; )
        {
        if (options[0] == '%' && options[1] == 's')
            {
            p = my_strcpy(p, envelope);
            *p++ = ' ';
            options += 2;
            break;
            }
        else
            {
            *p++ = *options++;
            }
        }
    *p++ = ' ';

    /* Append the list of recipients. */

    va_start(ap, envelope);
    while ((q = va_arg(ap, const char *)) != NULL)
        {
        p = my_strcpy(p, q);
        *p++ = ' ';
        }
    p[-1] = '\0';
    va_end(ap);

    fh = popen(cmdline, "w");
    if (fh == NULL)
        syslog(LOG_ERR, "Failed opening pipe to \"%s\": %s", cmdline, strerror(errno));

    free(cmdline);
    return fh;
    }


int
CloseMailer(FILE * fh)
    {
    return pclose(fh);
    }

static int
my_strlen(const char * p)
    {
    unsigned int  i;
    for (i = 0; *p && !isspace((int)*p); p++)
        i++;
    return i;
    }

#define MYPIPE_READ fildes[0]
#define MYPIPE_WRITE fildes[1]

int
ListMail(const char * envelope, const char * listname, const struct Mail * MailStruct)
    {
    const struct PD_Config * MasterConfig = getMasterConfig();
    const struct List_Config * ListConfig = getListConfig(listname);
    char **      arguments;
    unsigned int arguments_num = 256;
    char         buffer[256];
    char *       listfile;
    char *       nextAddress;
    char *       currAddress;
    char *       p;
    unsigned int counter;
    unsigned int len;
    unsigned int address_byte;
    unsigned int max_address_byte;
    int          fildes[2];
    pid_t        child_pid;
    int          child_status;

    /* Initialize internal stuff. */

    arguments = xmalloc((arguments_num+1) * sizeof(char *));
    max_address_byte = ARG_MAX - strlen(envelope) - strlen(MasterConfig->mta) -
        strlen(MasterConfig->mta_options) - 8;

    /* Load the list of recipients. */

    listfile = loadfile(ListConfig->address_file);
    if (listfile == NULL)
        return 1;

    /* Now go into delivery loop until we're finished. */

    for(counter = 0, currAddress = listfile; *currAddress != '\0'; counter = 0)
        {

        /* Set up the call to the MTA, including options. */

        arguments[counter++] = MasterConfig->mta;
        sprintf(buffer, MasterConfig->mta_options, envelope);
        for (p = buffer, arguments[counter++] = buffer; *p != '\0'; p++)
            {
            if (isspace((int)*p))
                {
                *p++ = '\0';
                while(*p != '\0' && isspace((int)*p))
                    p++;
                arguments[counter++] = p;
                }
            }
        if (strlen(arguments[counter-1]) == 0)
            counter--;

        /* Append as many recipients as fit. */

        for (address_byte = 0; *currAddress != '\0' ; currAddress = nextAddress)
            {
            nextAddress = text_find_next_line(currAddress);
            len = my_strlen(currAddress);
            if (address_byte + len > max_address_byte)
                break;
            if (counter > ARG_NUM_MAX)
                break;
            currAddress[len] = '\0';
            address_byte += len;
            arguments[counter++] = currAddress;
            if (counter+8 >= arguments_num)
                {
                arguments_num += 256;
                arguments = realloc(arguments, (arguments_num+1) * sizeof(char *));
                if (arguments == NULL)
                    return -1;
                }
            }

        /* Deliver the mail. */

        arguments[counter++] = NULL;
        if (pipe(fildes) == -1)
            {
            syslog(LOG_ERR, "Couldn't open a pipe to my child process: %s", strerror(errno));
            return -1;
            }
        child_pid = fork();
        switch(child_pid)
            {
            case 0:
                /* Child */
                close(MYPIPE_WRITE);
                if (dup2(MYPIPE_READ, STDIN_FILENO) == -1)
                    {
                    syslog(LOG_ERR, "Child process couldn't read from pipe: %s", strerror(errno));
                    return -1;
                    }
                close(MYPIPE_READ);
                execv(MasterConfig->mta, arguments);
                syslog(LOG_ERR, "Couldn't exec(\"%s\"): %s", MasterConfig->mta, strerror(errno));
                return -1;
            case -1:
                /* Error */
                syslog(LOG_ERR, "Couldn't fork: %s", strerror(errno));
                return -1;
            default:
                /* everything is fine */
                close(MYPIPE_READ);
            }
        write(MYPIPE_WRITE, MailStruct->Header, strlen(MailStruct->Header));
        write(MYPIPE_WRITE, "\n", 1);
        write(MYPIPE_WRITE, MailStruct->Body, strlen(MailStruct->Body));
        if (MailStruct->ListSignature != NULL)
            write(MYPIPE_WRITE, MailStruct->ListSignature, strlen(MailStruct->ListSignature));
        close(MYPIPE_WRITE);
        waitpid(child_pid, &child_status, 0);
        if (!(WIFEXITED(child_status) && WEXITSTATUS(child_status) == 0))
            {
            syslog(LOG_ERR, "The executed mail agent return error %d, aborting.",
                   WEXITSTATUS(child_status));
            return -1;
            }
        }
    return 0;
    }
