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

#include <stdio.h>
#include <assert.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"

/* parse an rfc822 address

   This routine is absolutely magnificent. It understands the full
   variety of Internet addresses as specified by RFC822 and parses
   them. "Parsing" means that it will check the address to be
   syntactically corrent and then returns the two parts an address
   consists of: localpart@hostpart. This may not sound like being a
   big thing, but you're invited to write a parser yourself to get an
   idea. :-)

   All kind of comments and delimeters are understood, due to
   sophisticated memory pooling hopefully no memory is leaking and the
   speed is absolutely untested.

   RETURNS: If the parsing process was successful, the local- and
   hostpart of the address will be stored in the location specified by
   the parameters. It will also store the parsed, canonic address
   (without any blanks, comments, etc...) in the provided location. If
   the address is syntactically incorrect, or if parsing failed due to
   an error, NULL will be stored in all these locations. The hostpart
   may also be NULL, if the address is a local address, e.g.
   "username".

   If either the "address", "localpart" or "hostpart" parameters are
   NULL, no result will be stored, what allows you to check an address
   for syntax errors.

   If the "string" parameter is NULL, the routine will return with a
   RFC822_FATAL_ERROR and set the global errno variable to EINVAL.

   RFC822_OK: Success.

   RFC822_FATAL_ERROR: A fatal system error occured, like malloc()
   failed. The global errno variable will be set accordingly.

   RFC822_UNCLOSED_COMMENT: A "(comment)"-like expression was not
   closed properly.

   RFC822_UNMATCHED_CLOSE_BRACKET: A close bracket (")") was
   encountered outside the comment context.

   RFC822_UNCLOSED_QUOTE: A quoted `"string"'-like expression was not
   closed properly.

   RFC822_UNCLOSED_ANGEL_BRACKET: An "<address>"-like expression was
   not closed properly.

   RFC822_NESTED_ANGEL_BRACKET: An open angel bracket ("<") was
   encountered inside the "<address>" context.

   RFC822_UNMATCHED_CLOSE_ANGEL_BRACKET: A close angel bracket (">") was
   encountered outside the "<address>" context.

   RFC822_SYNTAX_ERROR: The address does not follow the rfc822 syntax.

   NOTE: All buffers returned by the routine are allocated by
   malloc(3) and should be freed by the user when they're not needed
   anymore.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

int
rfc822_parse_address(const char *   string    /* String containing the address. */,
		     char **  address   /* Where to store the canonic address. */,
		     char **  localpart /* Where to store the local part. */,
		     char **  hostpart  /* Where to store the host part. */)
{
    int     rc;

    assert(string != NULL);
    if (!string) {
	errno = EINVAL;
	return RFC822_FATAL_ERROR;
    }

    rc = rfc822_decomment(string, &rfc822_address_buffer);
    if (rc != RFC822_OK)
      return rc;
    else
      rfc822_address_buffer_pos = 0;
    poolname = rfc822_address_buffer;

    no_memory_flag = 0;
    result_address = NULL;
    result_hostpart = NULL;
    result_localpart = NULL;

    rc = yyparse();
    if (rc == RFC822_OK) {
	if (address && (*address = result_address))
	  mp_remove_block_from_pool(poolname, *address);
	if (localpart && (*localpart = result_localpart))
	  mp_remove_block_from_pool(poolname, *localpart);
	if (hostpart && (*hostpart = result_hostpart))
	  mp_remove_block_from_pool(poolname, *hostpart);
    }
    else if (no_memory_flag != 0) {
	errno = ENOMEM;
	rc    = RFC822_FATAL_ERROR;
    }
    else
      rc = RFC822_SYNTAX_ERROR;

    mp_free_memory_pool(poolname);
    free(rfc822_address_buffer);

    return rc;
}
