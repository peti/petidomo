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
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"
#define safe_free(x) if (x) free(x)

int
main(int argc, char ** argv)
{
    struct rfc822_address_sep_state   sep_state;
    char     buffer[4096];
    char *   p,
	 *   address,
	 *   local,
	 *   host;
    int      rc;

    while(gets(buffer)) {

	printf("Read line: '%s'\n", buffer);
	sep_state.address_line = buffer;
	sep_state.group_nest   = 0;
	while((p = rfc822_address_sep(&sep_state))) {

	    if (*p == '\0')
	      continue;
	    else
	      printf("One part is: '%s'\n", p);

	    rc = rfc822_parse_address(p, &address, &local, &host);
	    if (rc == RFC822_OK) {
		printf("Address: '%s'\nLocal: '%s'\nHost: '%s'\n", address, local, host);
		safe_free(address);
		safe_free(local);
		safe_free(host);
	    }
	    else
	      printf("Syntax error: %d\n", rc);
	}

	printf("\n");
    }
    return 0;
}
