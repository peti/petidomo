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

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "libtext/text.h"
#include "petidomo.h"

void hermes_main(char * incoming_mail, const char * listname)
    {
    const struct PD_Config *     MasterConfig;
    const struct List_Config *   ListConfig;
    struct stat     sb;
    struct Mail *   MailStruct;
    FILE *          fh;
    char *          PostingHeaders;
    char *          currLine;
    char *          nextLine;
    char *          dst;
    char *          parameter;
    char *          buffer;
    char            envelope[1024];
    char            owner[1024];
    int             rc, len, operation;

    assert(listname != NULL);

    /* Initialize internals. */

    MasterConfig = getMasterConfig();
    ListConfig = getListConfig(listname);

    /* Parse the incoming mail. */

    rc = ParseMail(&MailStruct, incoming_mail, ListConfig->fqdn);
    if (rc != 0)
	{
	syslog(LOG_ERR, "Parsing the incoming mail failed.");
	exit(rc);
	}

    /* Do sanity checks. */

    if (MailStruct->Envelope == NULL)
	{
	syslog(LOG_ERR, "Received mail without a valid envelope.");
	return;
	}
    if (MailStruct->From == NULL)
	{
	syslog(LOG_ERR, "Received mail without From: line.");
	return;
	}
    if (*MailStruct->Body == '\0')
	{
	syslog(LOG_INFO, "Received mail with empty body.");
	return;
	}

    /* Initialize internal stuff. */

    if (isValidListName(listname) == FALSE)
	{
	syslog(LOG_ERR, "Mailing list \"%s\" does not exist.", listname);
	exit(EXIT_FAILURE);
	}
    PostingHeaders = xmalloc(strlen(MailStruct->Header)+1024);
    sprintf(envelope, "%s-owner@%s", listname, ListConfig->fqdn);
    sprintf(owner, "%s-owner@%s", listname, ListConfig->fqdn);

    /* Check for authorization. */

    if (FindBodyPassword(MailStruct) != 0)
	exit(EXIT_FAILURE);

    if (isValidPostingPassword(MailStruct->Approve, listname) == FALSE)
	{
	/* If no valid posting password has been provided, the mail is
           subject to the ACL mechanism. Please note that the ACL may
	   actually set a correct posting password via the 'approve'
	   command. So just because there wasn't a valid posting
	   password here, it doesn't mean there might not be after ACL
	   processing is over. That's why we check the posting
	   password again below. */

	if (checkACL(MailStruct, listname, &operation, &parameter, ACL_PRE) != 0)
	    {
	    syslog(LOG_ERR, "checkACL() failed with an error.");
	    exit(EXIT_FAILURE);
	    }
	rc = handleACL(MailStruct, listname, operation, parameter);
	switch(rc)
	    {
	    case -1:
		syslog(LOG_ERR, "handleACL() failed with an error.");
		exit(EXIT_FAILURE);
	    case 0:
		break;
	    case 1:
		return;
	    }
	}

    if (isValidPostingPassword(MailStruct->Approve, listname) == FALSE)
	{
	/* Reject the article, if the list is of type 'moderated'. */

	if (ListConfig->listtype == LIST_MODERATED)
	    {
	    syslog(LOG_INFO, "\"%s\" tried to post to list \"%s\", but failed to " \
		   "provide a correct password.", MailStruct->From, listname);

	    fh = vOpenMailer(envelope, owner, NULL);
	    if (fh != NULL)
		{
		fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", owner);
		fprintf(fh, "To: %s\n", owner);
		fprintf(fh, "Subject: Petidomo: BOUNCE %s@%s: Moderator approval required\n", listname, ListConfig->fqdn);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", owner);
		fprintf(fh, "\n");
		fprintf(fh, "The following posting requires your explicit approval:\n\n");
		fprintf(fh, "%s\n", MailStruct->Header);
		fprintf(fh, "%s", MailStruct->Body);
		CloseMailer(fh);
		}
	    else
		{
		syslog(LOG_ERR, "Failed to send email to \"%s\" concerning this request.", owner);
		exit(-1);
		}
	    return;
	    }

	else if (ListConfig->listtype == LIST_CLOSED)
	    {
	    /* Only subscribers may post */

	    if (isSubscribed(listname, MailStruct->From, NULL, NULL, TRUE) == FALSE)
		{
		syslog(LOG_INFO, "\"%s\" tried to post to closed list \"%s\", but " \
		       "he is no subscriber.", MailStruct->From, listname);

		fh = vOpenMailer(envelope, owner, NULL);
		if (fh != NULL)
		    {
		    fprintf(fh, "From: %s (Petidomo Mailing List Server)\n", owner);
		    fprintf(fh, "To: %s\n", owner);
		    fprintf(fh, "Subject: Petidomo: BOUNCE %s@%s: Non-member submission from \"%s\"\n", listname, ListConfig->fqdn, MailStruct->From);
		    fprintf(fh, "Precedence: junk\n");
		    fprintf(fh, "Sender: %s\n", owner);
		    fprintf(fh, "\n");
		    fprintf(fh, "The following posting was rejected, because the sender\n" \
			    "\"%s\" is not subscribed to the list:\n\n", MailStruct->From);
		    fprintf(fh, "%s\n", MailStruct->Header);
		    fprintf(fh, "%s", MailStruct->Body);
		    CloseMailer(fh);
		    }
		else
		    {
		    syslog(LOG_ERR, "Failed to send email to \"%s\" concerning this request.", owner);
		    exit(-1);
		    }
		return;
		}
	    }

	else if (ListConfig->listtype == LIST_ACKED && !g_is_approved)
	    {
	    /* Every posting needs an acknowledgement. */

	    char* cookie;
	    char c;
	    char* originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;

	    syslog(LOG_INFO, "\"%s\" tried to post to acknowledged list \"%s\"; posting " \
		   "has been deferred.", MailStruct->From, listname);

	    cookie = queue_posting(MailStruct, listname);
            fh = vOpenMailer(owner, originator, NULL);
            if (fh != NULL)
		{
		fprintf(fh, "From: petidomo-approve@%s (Petidomo Mailing List Server)\n", ListConfig->fqdn);
		fprintf(fh, "To: %s\n", originator);
		fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Your posting to list \"%s\"\n", listname, ListConfig->fqdn, listname);
		fprintf(fh, "Precedence: junk\n");
		fprintf(fh, "Sender: %s\n", owner);
		fprintf(fh, "\n");
		fprintf(fh, "Your posting needs to be approved. Reply to this mail and\n");
		fprintf(fh, "concatenate the two following strings to one large string\n");
		fprintf(fh, "\n");
 		c = cookie[16];
 		cookie[16] = '\0'; fprintf(fh, "    %s\n", &cookie[ 0]);
 		cookie[16] = c;    fprintf(fh, "    %s\n", &cookie[16]);
		fprintf(fh, "\n");
		fprintf(fh, "This validates your will to send and your will to read.\n");
		CloseMailer(fh);
		}
	    else
		{
		syslog(LOG_ERR, "Failed to send email to \"%s\" concerning this request.", owner);
		exit(-1);
		}
	    return;
	    }

	else if (ListConfig->listtype == LIST_ACKED_ONCE)
	    {
	    /* First posting needs an acknowledgement. */

	    if (g_is_approved)
		{
		syslog(LOG_INFO, "\"%s\" acknowledged a former posting attempt on ack-once list \"%s\"; " \
		       "add him to the ack file and let the posting pass.", MailStruct->From, listname);

		rc = add_address(ListConfig->ack_file, MailStruct->From);
		if (rc < 0)
		    {
		    syslog(LOG_ERR, "Can't add address to ack file.");
		    exit(-1);
		    }
		}
	    else
		{
		char* originator = (MailStruct->Reply_To) ? MailStruct->Reply_To : MailStruct->From;
		rc = is_address_on_list(ListConfig->ack_file, MailStruct->From);
		if (rc == 0 && MailStruct->Reply_To)
		    rc = is_address_on_list(ListConfig->ack_file, MailStruct->Reply_To);
		if (rc < 0)
		    {
		    syslog(LOG_ERR, "Can't verify whether address \"%s\" needs to be acknowledged or not.", MailStruct->From);
		    exit(-1);
		    }
		else if (rc == 0)
		    {
		    char* cookie;
		    char c;

		    syslog(LOG_INFO, "\"%s\" tried to post to ack-once list \"%s\", but is posting " \
			   "for the first time; posting has been deferred.", MailStruct->From, listname);

		    cookie = queue_posting(MailStruct, listname);
		    fh = vOpenMailer(owner, originator, NULL);
		    if (fh != NULL)
			{
			fprintf(fh, "From: petidomo-approve@%s (Petidomo Mailing List Server)\n", ListConfig->fqdn);
			fprintf(fh, "To: %s\n", originator);
			fprintf(fh, "Subject: Petidomo: APPROVE %s@%s: Your posting to list \"%s\"\n",
				listname, ListConfig->fqdn, listname);
			fprintf(fh, "Precedence: junk\n");
			fprintf(fh, "Sender: %s\n", owner);
			fprintf(fh, "\n");
			fprintf(fh, "Your posting needs to be approved. Reply to this mail and\n");
			fprintf(fh, "concatenate the two following strings to one large string\n");
			fprintf(fh, "\n");
			c = cookie[16];
			cookie[16] = '\0'; fprintf(fh, "    %s\n", &cookie[ 0]);
			cookie[16] = c;    fprintf(fh, "    %s\n", &cookie[16]);
			fprintf(fh, "\n");
			fprintf(fh, "This validates your will to send and your will to read.\n");
			fprintf(fh, "You won't have to repeat that action in the future.\n");
			CloseMailer(fh);
			}
		    else
			{
			syslog(LOG_ERR, "Failed to send email to \"%s\" concerning this request.", owner);
			exit(-1);
			}
		    return;
		    }
		else
		    syslog(LOG_INFO, "\"%s\" tried to post to ack-once list \"%s\" and has been found in " \
			   "the ack file; letting posting pass.", MailStruct->From, listname);
		}
	    }
	}

    /* additional ACL check */
    if (isValidPostingPassword(MailStruct->Approve, listname) == FALSE)
	{
        if (checkACL(MailStruct, listname, &operation, &parameter, ACL_POST) != 0)
            {
            syslog(LOG_ERR, "checkACL() failed with an error.");
            exit(EXIT_FAILURE);
            }
        rc = handleACL(MailStruct, listname, operation, parameter);
        switch(rc)
            {
            case -1:
                syslog(LOG_ERR, "handleACL() failed with an error.");
                exit(EXIT_FAILURE);
            case 0:
                break;
            case 1:
                return;
            }
        }

    /* Copy the desired headers from the original mail to our own
       buffer. */

    for(len = 0, currLine = MailStruct->Header, dst = PostingHeaders;
	*currLine != '\0';
	currLine = nextLine)
	{
	/* Find next header line. */

	nextLine = text_find_next_line(currLine);
	while (*nextLine == '\t' || *nextLine == ' ')
	    nextLine = text_find_next_line(nextLine);

	/* Copy the current line into our own buffer. */

	if (!strncasecmp(currLine, "From:", 5) ||
	    !strncasecmp(currLine, "To:", 3) ||
	    !strncasecmp(currLine, "Cc:", 3) ||
	    !strncasecmp(currLine, "Subject:", 8) ||
	    !strncasecmp(currLine, "Date:", 5) ||
	    !strncasecmp(currLine, "MIME-Version:", 13) ||
	    !strncasecmp(currLine, "Content-Type:", 13) ||
	    !strncasecmp(currLine, "Content-Transfer-Encoding:", 26) ||
	    !strncasecmp(currLine, "In-Reply-To:", 12) ||
	    !strncasecmp(currLine, "References:", 11) ||
	    !strncasecmp(currLine, "Message-Id:", 11) ||
  	    !strncasecmp(currLine, "X-Spam-Flag:", 12) ||
	    !strncasecmp(currLine, "Keywords:", 11) ||
	    !strncasecmp(currLine, "Summary:", 11) ||
	    !strncasecmp(currLine, "Received:", 9))
	    {
	    len = nextLine - currLine;
	    memmove(dst, currLine, len);
	    dst += len;
	    }
	}

    /* Add a Reply-To: field. */

    if (ListConfig->reply_to == NULL)
	len = sprintf(dst, "Reply-To: %s@%s\n", listname, ListConfig->fqdn);
    else if (!strcasecmp(ListConfig->reply_to, "none"))
	{
	if (MailStruct->Reply_To != NULL)
	    {
	    /* Copy Reply-To: line from original header. */

	    for(len = 0, currLine = MailStruct->Header;
		*currLine != '\0';
		currLine = nextLine)
		{

		nextLine = text_find_next_line(currLine);
		while (*nextLine == '\t' || *nextLine == ' ')
		    nextLine = text_find_next_line(nextLine);

		if (!strncasecmp(currLine, "Reply-To:", 9))
		    {
		    len = nextLine - currLine;
		    memmove(dst, currLine, len);
		    }
		}

	    }
	else
	    len = 0;
	}
    else
	{
	len = sprintf(dst, "Reply-To: %s\n", ListConfig->reply_to);
	}
    dst += len;

    /* Add a Sender: field. */

    len = sprintf(dst, "Sender: %s\n", owner);
    dst += len;

    /* Add a Precedence: field. */

    len = sprintf(dst, "Precedence: list\n");
    dst += len;
    *dst = '\0';

    /* Add custom headers if there are any. */

    if (stat(ListConfig->header_file, &sb) == 0)
	{
 	char* p = loadfile(ListConfig->header_file);
 	if (p == NULL)
	    {
 	    syslog(LOG_ERR, "Failed reading the header file for list \"%s\".", listname);
 	    exit(EXIT_FAILURE);
	    }
	strcpy(dst, p);
	dst += strlen(p);
	free(p);
	}

    /* Add the signature if there is one. */

    if (stat(ListConfig->sig_file, &sb) == 0)
	{
	buffer = loadfile(ListConfig->sig_file);
	if (buffer == NULL)
	    {
	    syslog(LOG_ERR, "Failed reading the signature file for list \"%s\".", listname);
	    exit(EXIT_FAILURE);
	    }
	MailStruct->ListSignature = buffer;
	}

    /* No more modifications will be made. Now copy the posting
       headers into the structure instead of the original ones. */

    MailStruct->Header = PostingHeaders;

    /* Apply the posting filter. */

    if (ListConfig->postingfilter != NULL)
	{
	rc = MailFilter(MailStruct, ListConfig->postingfilter);
	if (rc != 0)
	    {
	    syslog(LOG_ERR, "Postingfilter \"%s\" returned error %d while processing posting " \
		   "for list \"%s\".", ListConfig->postingfilter, rc, listname);
	    exit(EXIT_FAILURE);
	    }
	}

    /* Deliver the article to all recipients. */

    rc = ListMail(envelope, listname, MailStruct);
    if (rc != 0)
	{
	syslog(LOG_ERR, "The attempt to deliver the article to the subscribers failed.");
	exit(EXIT_FAILURE);
	}

    syslog(LOG_INFO, "Posted article from \"%s\" to list \"%s\" successfully.",
	   MailStruct->From, listname);

    /* Archive the article. */

    ArchiveMail(MailStruct, listname);
    }
