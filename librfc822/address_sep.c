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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

#include "rfc822.h"

static char *
read_until_next_quote(char * p)
{
    while (*p) {
	if (*p == '"') {
	    p++;
	    break;
	}
	if (*p == '\\' && p[1] != '\0') {
	    p += 2;
	    continue;
	}
	p++;
    }
    return p;
}

static char *
read_until_close_bracket(char * p)
{
    while (*p) {
	if (*p == ')') {
	    p++;
	    break;
	}
	if (*p == '(') {
	    p = read_until_close_bracket(p+1);
	    continue;
	}
	else if (*p == '\\' && p[1] != '\0') {
	    p += 2;
	    continue;
	}
	else if (*p == '"') {
	    p = read_until_next_quote(p+1);
	    continue;
	}
	p++;
    }
    return p;
}

static int
is_source_routing(char * p)
{
    while (*p) {
	if (*p == '(')
	  p = read_until_close_bracket(p+1);

	else if (*p == ' ' || *p == '\t')
	  p++;

	else if (*p == '@' || *p == '<')
	  return 1;

	else
	  return 0;
    }
    return 0;
}

/* Split an RFC822 address line.

   This routine breaks an address line, as specified by RFC822, up
   into separate addresses. The used delimiter is the comma (",").

   The usage of the routine is very similar to strsep(3). More text to
   come.

   AUTHOR: Peter Simons <simons@rhein.de>

 */

char *
rfc822_address_sep(struct rfc822_address_sep_state *   state)
{
    char *   p,
         *   old;
    int      allow_groups;

    /* Sanity checks. */

    assert(state != NULL);
    if (!state) {
	errno = EINVAL;
	return NULL;
    }

    if (*(state->address_line) == '\0')
      return NULL;

    old = p = state->address_line;
    allow_groups = !is_source_routing(p);

    while(*p) {
	if (*p == ',') {
	    *p = '\0';
	    state->address_line = p+1;
	    return old;
	}
	else if (*p == ':' && allow_groups) {
	    old = p+1;
	    state->group_nest++;
	    allow_groups = !is_source_routing(p+1);
	}
	else if (*p == ';') {

	    /* If we are inside an address group, the ';' character is
	       interpreted like a comma. */

	    if (state->group_nest > 0) {
		state->group_nest--;
		*p = ',';
		continue;
	    }
	    else
	      /* do nothing */;
	}
	else if (*p == '(') {
	    p = read_until_close_bracket(p+1);
	    continue;
	}
	else if (*p == '\\' && p[1] != '\0')
	  p++;
	else if (*p == '"') {
	    p = read_until_next_quote(p+1);
	    continue;
	}
	p++;
    }
    state->address_line = p;
    return old;
}
