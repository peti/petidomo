/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

   This file is part of OpenPetidomo.

   OpenPetidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   OpenPetidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "librfc822/rfc822.h"
#include "libtext/text.h"
#include "petidomo.h"

void
RemoveCarrigeReturns(char * buffer)
{
    char *   src = buffer;
    char *   dst = buffer;

    while(*src != '\0') {
	switch(*src) {
	  case '\n':
	      *dst++ = ' ';
	      while(isspace((int)*src))
		src++;
	      break;
	  default:
	      *dst++ = *src++;
	}
    }
    *dst++ = '\0';
}

bool
isRFC822Address(const char * buffer)
{
    char *   address;
    int      rc;

    rc = rfc822_parse_address(buffer, &address, NULL, NULL);
    if (rc == RFC822_OK) {
	debug((DEBUG_RFCPARSE, 3, "'%s' is a valid rfc address.", address));
	if (address) {
	    free(address);
	}
	return TRUE;
    }
    else {
	debug((DEBUG_RFCPARSE, 4, "'%s' is not a valid rfc address", buffer));
	return FALSE;
    }
}

int
ParseAddressLine(char * buffer)
{
    struct rfc822_address_sep_state   sep_state;
    char *   p,
	 *   address;
    int      rc;

    /* Handle continuation lines. */

    RemoveCarrigeReturns(buffer);

    /* Initialize the structure needed for address_sep(). */

    sep_state.address_line = buffer;
    sep_state.group_nest   = 0;

    debug((DEBUG_RFCPARSE, 2, "Original address is \"%s\".", buffer));

    /* We want only the first address, if multiple are there. */

    while ((p = rfc822_address_sep(&sep_state)) != NULL) {
	if (*p == '\0')
	  continue;
	else
	  break;
    }

    if (p == NULL) {
	/* line is empty */
	return -1;
    }

    debug((DEBUG_RFCPARSE, 2, "First part is \"%s\".", p));

    rc = rfc822_parse_address(p, &address, NULL, NULL);
    if (rc == RFC822_OK && address != NULL) {
	debug((DEBUG_RFCPARSE, 2, "Parsed address is: '%s'", address));
	strcpy(buffer, address);
	free(address);
	return 0;
    }
    else
      return -1;
}

int
ParseReplyToLine(char * buffer)
{
    return ParseAddressLine(buffer);
}

int
ParseFromLine(char * buffer)
{
    return ParseAddressLine(buffer);
}

int
ParseMessageIdLine(char * buffer)
{
    int   rc;

    debug((DEBUG_RFCPARSE, 2, "Unparsed Message-Id: '%s'", buffer));

    rc = ParseAddressLine(buffer);
    if (rc != 0)
      return rc;		/* Error! */

    memmove(buffer+1, buffer, strlen(buffer)+1);
    buffer[0] = '<';
    strcat(buffer, ">");

    debug((DEBUG_RFCPARSE, 2, "Parsed Message-Id: '%s'", buffer));

    return rc;
}

int
ParseApproveLine(char * buffer)
{
    char *       src;
    char *       dst;

    RemoveCarrigeReturns(buffer);

    src = buffer;
    dst = buffer;

    /* Skip leading whitespace. */

    while(isspace((int)*src))
      src++;

    /* Skip a quote if there is one. */

    if (*src == '\"')
      src++;

    /* Copy String. */

    while((*dst++ = *src++) != '\0')
      ;
    dst--;

    /* Kill trailing whitespace. */

    while(isspace((int)dst[-1]))
      *(--dst) = '\0';

    /* Kill a quote if there is one. */

    if (dst[-1] == '\"')
      *(--dst) = '\0';

    return 0;
}

void
CanonizeAddress(char ** buffer, const char * fqdn)
{
    const struct PD_Config *     MasterConfig;
    char *                       newbuf;
    char *   local,
         *   host;
    int      rc;

    assert(buffer != NULL);
    assert(*buffer != NULL);

    if (buffer == NULL || *buffer == NULL)
      return;

    debug((DEBUG_RFCPARSE, 3, "Check whether \"%s\" is a canon address.", *buffer));

    rc = rfc822_parse_address(*buffer, NULL, &local, &host);
    if (rc == RFC822_OK) {
        if (local != NULL && host == NULL) {
	    debug((DEBUG_RFCPARSE, 3, "'%s' is a local address, appending my hostname.",
		   *buffer));
	    if (fqdn == NULL) {
		MasterConfig = getMasterConfig();
		fqdn = MasterConfig->fqdn;
	    }
	    newbuf = xmalloc(strlen(*buffer) + strlen(fqdn) + 3);
	    sprintf(newbuf, "%s@%s", local, fqdn);
	    free(local);
	    *buffer = newbuf;
	    debug((DEBUG_RFCPARSE, 3, "Canonized address: '%s'.", *buffer));
        }
	else
	  debug((DEBUG_RFCPARSE, 3, "\"%s\" is a full address.", *buffer));
    }
}

int
ParseMail(struct Mail **result, char * incoming_mail, const char * fqdn)
{
    struct Mail *   MailStruct;
    char *          currLine;
    char *          nextLine;
    int             rc;

    /* Allocate structure. */

    MailStruct = calloc(sizeof(struct Mail), 1);
    if (MailStruct == NULL) {
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", sizeof(struct Mail));
	return -1;
    }

    /* Rescue the mail in its original state, before the parsing
       routines have havoc in the buffer. */

    MailStruct->Header = strdup(incoming_mail);
    if (MailStruct->Header == NULL) {
	syslog(LOG_ERR, "Failed to allocate %d byte of memory.", strlen(incoming_mail));
	return -1;
    }
    for (MailStruct->Body = MailStruct->Header;
	 *MailStruct->Body != '\n' && *MailStruct->Body != '\0';
	 MailStruct->Body = text_find_next_line(MailStruct->Body))
      ;
    if (*MailStruct->Body == '\n') {
	*MailStruct->Body = '\0';
	MailStruct->Body++;
    }

    /* Get the envelope. */

    currLine = incoming_mail;
    nextLine = text_find_next_line(incoming_mail);
    if (strncasecmp("From ", currLine, strlen("From ")) == 0) {
	if (nextLine[-1] == '\n')
	  nextLine[-1] = '\0';
	currLine += strlen("From ");
	while (isspace((int)*currLine))
	  currLine++;
	MailStruct->Envelope = currLine;
	while (!isspace((int)*currLine))
	  currLine++;
	*currLine = '\0';
	CanonizeAddress(&(MailStruct->Envelope), fqdn);
	debug((DEBUG_RFCPARSE, 5, "Envelope is \"%s\".", MailStruct->Envelope));
	currLine = nextLine;
    }

    /* Parse the incoming mail's header. */

    for (nextLine = text_find_next_line(currLine);
	 *currLine != '\n' && *currLine != '\0';
	 currLine = nextLine, nextLine = text_find_next_line(currLine)) {

	/* Find continuation lines. */

	while (*nextLine == ' ' || *nextLine == '\t')
	  nextLine = text_find_next_line(nextLine);

	/* remove trailing \n */

	if (nextLine[-1] == '\n')
	  nextLine[-1] = '\0';

	/* Log contents of current line. */

	debug((DEBUG_RFCPARSE, 6, "Parsing line \"%s\".", currLine));

	/* Check whether it is a header we're interested in. */

	if (strncasecmp("From:", currLine, strlen("From:")) == 0) {
	    if (MailStruct->From != NULL) {
		syslog(LOG_NOTICE, "Received mail with multiple From: lines.");
		continue;
	    }
	    MailStruct->From = &currLine[strlen("From:")];
	    rc = ParseFromLine(MailStruct->From);
	    if (rc != 0)
	      return rc;
	    CanonizeAddress(&(MailStruct->From), fqdn);
	    debug((DEBUG_RFCPARSE, 5, "From: is \"%s\".", MailStruct->From));
	} else if (strncasecmp("Reply-To:", currLine, strlen("Reply-To:")) == 0) {
	    if (MailStruct->Reply_To != NULL) {
		syslog(LOG_NOTICE, "Received mail with multiple Reply-To: lines.");
		continue;
	    }
	    MailStruct->Reply_To = &currLine[strlen("Reply-To:")];
	    rc = ParseReplyToLine(MailStruct->Reply_To);
	    if (rc != 0)
	      return rc;
	    CanonizeAddress(&(MailStruct->Reply_To), fqdn);
	    debug((DEBUG_RFCPARSE, 5, "Reply-To: is \"%s\".", MailStruct->Reply_To));
	} else if (strncasecmp("Message-Id:", currLine, strlen("Message-Id:")) == 0) {
	    if (MailStruct->Message_Id != NULL) {
		syslog(LOG_NOTICE, "Received mail with multiple Message-Id: lines.");
		continue;
	    }
	    MailStruct->Message_Id = &currLine[strlen("Message-Id:")];
	    rc = ParseMessageIdLine(MailStruct->Message_Id);
	    if (rc != 0)
	      return rc;
	    debug((DEBUG_RFCPARSE, 5, "Message-Id: is \"%s\".", MailStruct->Message_Id));
	}
	else if (strncasecmp("Approve:", currLine, strlen("Approve:")) == 0) {
	    if (MailStruct->Approve != NULL)
	      syslog(LOG_NOTICE, "Received mail with multiple Approve: lines.");
	    MailStruct->Approve = &currLine[strlen("Approve:")];
	    rc = ParseApproveLine(MailStruct->Approve);
	    if (rc != 0)
	      return rc;
	    debug((DEBUG_RFCPARSE, 5, "Approve: is \"%s\".", MailStruct->Approve));
	}
	else if (strncasecmp("Approved:", currLine, strlen("Approved:")) == 0) {
	    if (MailStruct->Approve != NULL)
	      syslog(LOG_NOTICE, "Received mail with multiple Approve: lines.");
	    MailStruct->Approve = &currLine[strlen("Approved:")];
	    rc = ParseApproveLine(MailStruct->Approve);
	    if (rc != 0)
	      return rc;
	    debug((DEBUG_RFCPARSE, 5, "Approve: is \"%s\".", MailStruct->Approve));
	}
	else if (strncasecmp("Subject:", currLine, strlen("Subject:")) == 0) {
	    if (MailStruct->Subject != NULL)
	      syslog(LOG_NOTICE, "Received mail with multiple Subject: lines.");
	    MailStruct->Subject = &currLine[strlen("Subject:")];
	    if (*MailStruct->Subject == ' ')
	      MailStruct->Subject += 1;
	    debug((DEBUG_RFCPARSE, 5, "Subject: is \"%s\".", MailStruct->Subject));
	}
	else if (strncasecmp("Sender:", currLine, strlen("Sender:")) == 0) {
	    if (MailStruct->Envelope != NULL)
	      syslog(LOG_NOTICE, "Received mail with multiple sender addresses.");
	    MailStruct->Envelope = &currLine[strlen("Sender:")];
	    if (*MailStruct->Envelope == ' ')
	      MailStruct->Envelope += 1;
	    debug((DEBUG_RFCPARSE, 5, "Sender: is \"%s\".", MailStruct->Envelope));
	}
	else if (strncasecmp("Return-Path:", currLine, strlen("Return-Path:")) == 0 &&
		 MailStruct->Envelope == NULL)
	{
	    if (MailStruct->Envelope != NULL)
	      syslog(LOG_NOTICE, "Received mail with multiple sender addresses.");
	    MailStruct->Envelope = &currLine[strlen("Return-Path:")];
	    if (*MailStruct->Envelope == ' ')
	      MailStruct->Envelope += 1;
	    debug((DEBUG_RFCPARSE, 5, "Return-Path: is \"%s\".", MailStruct->Envelope));
	}
    }

    *result = MailStruct;
    return 0;
}
