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
#include <errno.h>

#include "libtext/text.h"
#include "petidomo.h"

int
AddAddress(struct Mail * MailStruct,
                const char * param1,
                const char * param2,
                const char * defaultlist)
    {
    const struct PD_Config   * MasterConfig;
    const struct List_Config * ListConfig;
    FILE *         fh;
    const char *   address = NULL;
    const char *   listname = NULL;
    char           owner[4096];
    char           envelope[4096];
    char *         buffer;
    char *         originator;
    char *         p;

    /* Initialize internal stuff from master config file. */

    MasterConfig = getMasterConfig();
    sprintf(envelope, "petidomo-manager@%s", MasterConfig->fqdn);
    originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

    /* Try to find out, which parameter is what. */

    if (param1 != NULL)
        {
        if (isValidListName(param1) == TRUE)
            listname = param1;
        else if (isRFC822Address(param1) == TRUE)
            address = param1;

        if (param2 != NULL)
            {
            if (listname == NULL && isValidListName(param2) == TRUE)
                listname = param2;
            else if (address == NULL && isRFC822Address(param2) == TRUE)
                address = param2;
            }
        }

    if (address == NULL)
        address = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
    assert(address != NULL);

    if (listname == NULL)
        {
        if (defaultlist != NULL)
            listname = defaultlist;
        else
            {
            syslog(LOG_INFO, "%s: subscribe-command invalid: No list specified.", MailStruct->From);
            fh = vOpenMailer(envelope, originator, NULL);
            if (fh != NULL)
                {
                fprintf(fh, "From: petidomo@%s (Petidomo Mailing List Server)\n", MasterConfig->fqdn);
                fprintf(fh, "To: %s\n", originator);
                fprintf(fh, "Subject: Petidomo: Your request \"subscribe %s\"\n", address);
                if (MailStruct->Message_Id != NULL)
                    fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
                fprintf(fh, "Precedence: junk\n");
                fprintf(fh, "Sender: %s\n", envelope);
                fprintf(fh, "\n");
                buffer = text_easy_sprintf("You tried to subscribe the address \"%s\" to a mailing list. "   \
                                           "Unfortunately, your request could not be processed, because "    \
                                           "you did not specify a valid mailing list name to which the "      \
                                           "address should be subscribed to. You may use the command INDEX " \
                                           "to receive an overview of the available mailing lists. Also, " \
                                           "use the command HELP to verify that you got the command syntax " \
                                           "right.", address);
                text_wordwrap(buffer, 70);
                fprintf(fh, "%s\n", buffer);
                CloseMailer(fh);
                }
            else
                syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
            return 0;
            }
        }

    /* Initialize internal stuff again from list config. */

    ListConfig = getListConfig(listname);
    sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);
    sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);

    /* Check whether the request is authorized at all. */

    if (isValidAdminPassword(getPassword(), listname) == FALSE)
        {
        /* No valid password, check further. */

        if (ListConfig->subtype == SUBSCRIPTION_ADMIN)
            {
            /* Access was unauthorized, notify the originator. */

            syslog(LOG_INFO, "%s: Attempt to subscribe \"%s\" to list \"%s\" rejected due to lack of " \
                   "a correct admin password.", MailStruct->From, address, listname);

            fh = vOpenMailer(envelope, originator, NULL);
            if (fh != NULL)
                {
                fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
                        listname, ListConfig->fqdn);
                fprintf(fh, "To: %s\n", originator);
                fprintf(fh, "Subject: Petidomo: Your request \"subscribe %s %s\"\n", address, listname);
                if (MailStruct->Message_Id != NULL)
                    fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
                fprintf(fh, "Precedence: junk\n");
                fprintf(fh, "Sender: %s\n", envelope);
                fprintf(fh, "\n");
                buffer = text_easy_sprintf(
                                           "The mailing list \"%s\" is a closed forum and only the maintainer may " \
                                           "subscribe addresses. Your request has been forwarded to the " \
                                           "appropriate person, so please don't send any further mail. You will " \
                                           "be notified as soon as possible.", listname);
                text_wordwrap(buffer, 70);
                fprintf(fh, "%s\n", buffer);
                CloseMailer(fh);
                }
            else
                syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);

            /* Notify the owner. */

            fh = vOpenMailer(envelope, owner, NULL);
            if (fh != NULL)
                {
                fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
                        listname, ListConfig->fqdn);
                fprintf(fh, "To: %s\n", owner);
                fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Unauthorized request from \"%s\"\n", listname, ListConfig->fqdn, originator);
                fprintf(fh, "Precedence: junk\n");
                fprintf(fh, "Sender: %s\n", envelope);
                fprintf(fh, "\n");
                buffer = text_easy_sprintf(
                                           "\"%s\" tried to subscribe the address \"%s\" to the \"%s\" mailing list, " \
                                           "but couldn't provide the correct password. To subscribe him, send the " \
                                           "following commands to the server:", originator, address, listname);
                text_wordwrap(buffer, 70);
                fprintf(fh, "%s\n\n", buffer);
                fprintf(fh, "password <AdminPassword>\n");
                fprintf(fh, "subscribe %s %s\n", address, listname);
                CloseMailer(fh);
                }
            else
                {
                syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
                return -1;
                }
            return 0;
            }
        }

    /* Check whether the address is subscribed already. */

    if (isSubscribed(listname, address, NULL, NULL, FALSE) == TRUE)
        {
        syslog(LOG_INFO, "%s: Attempt to subscribe \"%s\" to list \"%s\" rejected, because the " \
               "address is already on the list.", MailStruct->From, address, listname);

        /* Notify the originator, that the address is already a
           member. */

        fh = vOpenMailer(envelope, originator, NULL);
        if (fh != NULL)
            {
            fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n", listname, ListConfig->fqdn);
            fprintf(fh, "To: %s\n", originator);
            fprintf(fh, "Subject: Petidomo: Your request \"subscribe %s %s\"\n", address, listname);
            if (MailStruct->Message_Id != NULL)
                fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
            fprintf(fh, "Precedence: junk\n");
            fprintf(fh, "Sender: %s\n", envelope);
            fprintf(fh, "\n");
            fprintf(fh, "The address \"%s\" is already subscribed to list \"%s\".\n", address, listname);
            CloseMailer(fh);
            }
        else
            {
            syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
            return -1;
            }
        return 0;
        }

    if (isValidAdminPassword(getPassword(), listname) == FALSE &&
        ListConfig->subtype == SUBSCRIPTION_ACKED && !g_is_approved)
        {
        /* Require approval. */

        char* command;
        char  c;
        char* cookie;

        syslog(LOG_INFO, "%s: Attempt to subscribe \"%s\" to list \"%s\" deferred, because the " \
               "request must be acknowledged first.", MailStruct->From, address, listname);

        command = text_easy_sprintf("subscribe %s %s", address, listname);
        cookie  = queue_command(MailStruct, command);

        /* Send request for approval to the user. */

        fh = vOpenMailer(envelope, address, NULL);
        if (fh != NULL)
            {
            fprintf(fh, "From: petidomo-approve@%s (Petidomo Mailing List Server)\n", ListConfig->fqdn);
            fprintf(fh, "To: %s\n", address);
            if (strcasecmp(address, originator) == 0)
                fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Your request \"subscribe %s\"\n", listname, ListConfig->fqdn, listname);
            else
                fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Request \"subscribe %s\" from \"%s\"\n", listname, ListConfig->fqdn, listname, originator);
            fprintf(fh, "Precedence: junk\n");
            fprintf(fh, "Sender: %s\n", envelope);
            fprintf(fh, "\n");
            if (strcasecmp(address, originator) == 0)
                buffer = text_easy_sprintf("You requested that the address \"%s\" should be subscribed to " \
                                           "the mailing list \"%s\". This will not happen unless you approve the " \
                                           "request by replying to this mail and concatenating the following two strings into one",
                                           address, listname);
            else
                buffer = text_easy_sprintf("Per request from \"%s\", the address \"%s\" should be subscribed to " \
                                           "the mailing list \"%s\". This will not happen unless you approve the " \
                                           "request by replying to this mail and concatenating the following two strings into one",
                                           originator, address, listname);
            text_wordwrap(buffer, 70);
            fprintf(fh, "%s\n", buffer);
            fprintf(fh, "\n");
            c = cookie[16];
            cookie[16] = '\0'; fprintf(fh, "    %s\n", &cookie[ 0]);
            cookie[16] = c;    fprintf(fh, "    %s\n", &cookie[16]);
            fprintf(fh, "\n");
            fprintf(fh, "in your reply.\n");
            CloseMailer(fh);

            /* If the request for approval has been sent to an
               address different to that of the originator, notify him
               what happened. */

            if (strcasecmp(address, originator) != 0)
                {
                fh = vOpenMailer(envelope, originator, NULL);
                if (fh != NULL)
                    {
                    fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n", listname, ListConfig->fqdn);
                    fprintf(fh, "To: %s\n", originator);
                    fprintf(fh, "Subject: Petidomo: Your request \"subscribe %s %s\"\n", address, listname);
                    if (MailStruct->Message_Id != NULL)
                        fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
                    fprintf(fh, "Precedence: junk\n");
                    fprintf(fh, "Sender: %s\n", envelope);
                    fprintf(fh, "\n");
                    fprintf(fh, "Subscribing the address \"%s\" to the list \"%s\"\n", address, listname);
                    fprintf(fh, "requires additional approval by \"%s\". A request\n", address);
            fprintf(fh, "has been sent to this address. We are now awaiting the approval.\n");
                    CloseMailer(fh);
                    }
                else
                    {
                    syslog(LOG_ERR, "Failed to send email to \"%s\" concerning his request.", originator);
                    return -1;
                    }
                }
            }
        else
            {
            syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
            return -1;
            }
        return 0;
        }

    /* Okay, add the address to the list. */

    syslog(LOG_INFO, "%s: Okay; subscribing address \"%s\" to list \"%s\".", MailStruct->From, address, listname);

    fh = fopen(ListConfig->address_file, "a");
    if (fh == NULL)
        {
        syslog(LOG_ERR, "Failed to open file \"%s\" for writing: %s", ListConfig->address_file, strerror(errno));
        return -1;
        }
    fprintf(fh, "%s\n", address);
    fclose(fh);

    /* Send success notification to the originator and the new subscriber */

    if (!strcasecmp(address, originator))
        fh = vOpenMailer(envelope, address, NULL);
    else
        fh = vOpenMailer(envelope, address, originator, NULL);
    if (fh != NULL)
        {
        fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
                listname, ListConfig->fqdn);
        fprintf(fh, "To: %s\n", address);
        if (strcasecmp(address, originator) == TRUE)
            fprintf(fh, "Cc: %s\n", originator);
        fprintf(fh, "Subject: Petidomo: Request \"subscribe %s %s\"\n", address, listname);
        if (MailStruct->Message_Id != NULL)
            fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
        fprintf(fh, "Precedence: junk\n");
        fprintf(fh, "Sender: %s\n", envelope);
        fprintf(fh, "\n");
        if (!strcasecmp(address, originator))
            {
            buffer = text_easy_sprintf(
                                       "Per your request, the address \"%s\" has been subscribed to the " \
                                       "\"%s\" mailing list. If you want to unsubscribe later, you can " \
                                       "do so by sending the following command to \"%s-request@%s\":",
                                       address, listname, listname, ListConfig->fqdn);
            }
        else
            {
            buffer = text_easy_sprintf(
                                       "Per request from \"%s\", the address \"%s\" has been subscribed to the " \
                                       "\"%s\" mailing list. If you want to unsubscribe later, you can " \
                                       "do so by sending the following command to \"%s-request@%s\":",
                                       originator, address, listname, listname, ListConfig->fqdn);
            }
        text_wordwrap(buffer, 70);
        fprintf(fh, "%s\n\n", buffer);
        fprintf(fh, "unsubscribe %s\n\n", address);
        fprintf(fh, "Please save a copy of this mail, to make sure you remember how " \
                "to\nunsubscribe!\n");
        CloseMailer(fh);
        }
    else
        {
        syslog(LOG_ERR, "Failed to send email to \"%s\"!", address);
        return -1;
        }

    /* Send success notification to the list owner */

    fh = vOpenMailer(envelope, owner, NULL);
    if (fh != NULL) {
        fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
                listname, ListConfig->fqdn);
        fprintf(fh, "To: %s\n", owner);
        fprintf(fh, "Subject: Petidomo: SUBSCRIBE %s@%s: %s\n", listname, ListConfig->fqdn, address);
        if (MailStruct->Message_Id != NULL)
            fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
        fprintf(fh, "Precedence: junk\n");
        fprintf(fh, "Sender: %s\n", envelope);
        fprintf(fh, "\n");
        if (!strcasecmp(address, originator)) {
            buffer = text_easy_sprintf(
                         "Per request from the subscriber, the address \"%s\"\n"
                         "has been subscribed to the \"%s\" mailing list.\n",
                         address, listname, ListConfig->fqdn);
        }
        else {
            buffer = text_easy_sprintf(
                         "Per request from \"%s\", the address \"%s\"\n"
                         "has been subscribed to the \"%s\" mailing list.\n",
                         originator, address, listname, ListConfig->fqdn);
        }
        text_wordwrap(buffer, 75);
        fprintf(fh, "%s\n", buffer);
        fprintf(fh, "No action is required on your part.\n");
        CloseMailer(fh);
    }
    else {
        syslog(LOG_ERR, "Failed to send email to \"%s\"!", owner);
        return -1;
    }

    /* Send introduction text to the new member. */

    p = loadfile(ListConfig->intro_file);
    if (p != NULL)
        {
        fh = vOpenMailer(envelope, address, NULL);
        if (fh != NULL)
            {
            fprintf(fh, "From: %s-request@%s (Petidomo Mailing List Server)\n",
                    listname, ListConfig->fqdn);
            fprintf(fh, "To: %s\n", address);
            fprintf(fh, "Subject: Petidomo: Welcome to \"%s@%s\"!\n", listname, ListConfig->fqdn);
            if (MailStruct->Message_Id != NULL)
                fprintf(fh, "In-Reply-To: %s\n", MailStruct->Message_Id);
            fprintf(fh, "Precedence: junk\n");
            fprintf(fh, "Sender: %s\n", envelope);
            fprintf(fh, "\n");
            fprintf(fh, "%s\n", p);
            CloseMailer(fh);
            free(p);
            }
        else
            {
            free(p);
            syslog(LOG_ERR, "Failed to send introduction mail to \"%s\"!", address);
            return -1;
            }
        }
    return 0;
    }
