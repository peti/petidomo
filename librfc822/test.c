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

#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif
#include <progname.h>

#include "rfc822.h"

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

    set_program_name(argv[0]);

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
                free(address);
                free(local);
                free(host);
            }
            else
              printf("Syntax error: %d\n", rc);
        }

        printf("\n");
    }
    return 0;
}
