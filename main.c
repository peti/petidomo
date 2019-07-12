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

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "libargv/argv.h"
#include "libtext/text.h"
#include "petidomo.h"
#include "progname.h"

#ifndef LOG_PERROR
#  define LOG_PERROR 0
#endif

static char*  listname = NULL;
static char*  mode = NULL;
char*  masterconfig_path = SYSCONFDIR "/petidomo.conf";
char          g_is_approved = ARGV_FALSE;
const char* who_am_i;

int
main(int argc, char * argv[])
    {
    char *        incoming_mail;
    argv_t        args[] =
        {
        {ARGV_MAND, "mode", ARGV_CHAR_P, &mode, "mode", "listserv, deliver, approve or dump"},
        {ARGV_MAYBE, "listname", ARGV_CHAR_P, &listname, "listname", "default mailing list name"},
        {ARGV_MAYBE, "masterconf", ARGV_CHAR_P, &masterconfig_path, "masterconf", "path to global petidomo.conf"},
        {ARGV_MAYBE, "approved", ARGV_BOOL, &g_is_approved, "approved", "approved flag."},
        {ARGV_LAST}
        };

    set_program_name(argv[0]);

    /* Init logging routines first of all, so that we can report
       errors. */

    openlog("petidomo", LOG_CONS | LOG_PID | LOG_PERROR, LOG_MAIL);

    /* Store our full path and program name in who_am_I, so that
       queue_posting() and queue_command() know where to find the
       Petidomo binary. */

    if (argv[0][0] == '/')
        {
        who_am_i = argv[0];
        }
    else
        {
        char buf[4096];
        if (getcwd(buf, sizeof(buf)) == NULL)
            {
            syslog(LOG_CRIT, "Failed to get the path to my current working directory.");
            exit(EXIT_FAILURE);
            }
        who_am_i = text_easy_sprintf("%s/%s", buf, argv[0]);
        }

    /* Set our real user id equal to the effective user id to avoid
       confusion in case we're started as a setuid binary. */

    setreuid(geteuid(), geteuid());

    /* Parse the command line. */

    argv_help_string = "Petidomo Mailing List Server";
    argv_version_string = PACKAGE_STRING;
    argv_process(args, argc, argv);

    /* Make sure we got all required parameters. */

    if ((!strcasecmp(mode, "deliver") || !strcasecmp(mode, "dump")) && listname == NULL)
        {
        fprintf(stderr, "petidomo: %s mode requires a list name argument\n", mode);
        exit(EXIT_FAILURE);
        }

    /* Member Dump Mode */

    if (strcasecmp(mode, "dump") == 0)
        {
        char *cp;
        const struct List_Config *ListConfig;
        if (InitPetidomo(masterconfig_path) != 0)
            {
            fprintf(stderr, "petidomo: failed load master configuration.\n");
            exit(EXIT_FAILURE);
            }
        ListConfig = getListConfig(listname);
        if ((cp = loadfile(ListConfig->address_file)) == NULL)
            {
            fprintf(stderr, "petidomo: failed to open file \"%s\"\n", ListConfig->address_file);
            exit(EXIT_FAILURE);
            }
        fwrite(cp, strlen(cp), 1, stdout);
        free(cp);
        exit(EXIT_SUCCESS);
    }

    /* Log a few helpful facts about this Petidomo instance. */

    syslog(LOG_DEBUG, "%s starting up; mode=%s, listname=%s, masterconf=%s, approved=%s, ruid=%d, euid=%d, gid=%d, egid=%d",
           PACKAGE_STRING, mode, (listname != NULL ? listname : "<none>"), masterconfig_path, (g_is_approved) ? "true" : "false",
           getuid(), geteuid(), getgid(), getegid());

    /* Init Petidomo's internal stuff. */

    if (InitPetidomo(masterconfig_path) != 0) {
        syslog(LOG_CRIT, "Failed to initialize my internals.");
        exit(EXIT_FAILURE);
    }

    /* Load the file from standard input and save it, so that it isn't
       lost in case of an error. */

    incoming_mail = LoadFromDescriptor(STDIN_FILENO);
    if (incoming_mail == NULL) {
        syslog(LOG_ERR, "Failed to read incoming mail from standard input.");
        exit(EXIT_FAILURE);
    }

    /* Now decide what we actually do with the mail. */

    if (strcasecmp("listserv", mode) == 0)
        listserv_main(incoming_mail, listname);
    else if (strcasecmp("deliver", mode) == 0)
        {
        if (listname != NULL)
            hermes_main(incoming_mail, listname);
        else
            {
            syslog(LOG_ERR, "Wrong command line syntax; deliver mode requires a parameter.");
            exit(EXIT_FAILURE);
            }
        }
    else if (strcasecmp("approve", mode) == 0)
        {
        approve_main(incoming_mail);
        }
    else
        {
        syslog(LOG_ERR, "I don't know anything about mode \"%s\".", mode);
        exit(EXIT_FAILURE);
        }

    /* Exit gracefully. */

    return 0;
    }
