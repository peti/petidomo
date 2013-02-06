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

#ifndef __LIB_MPOOLS_H__
#define __LIB_MPOOLS_H__ 1

#include <stdlib.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

struct mp_list_entry {
    struct mp_list_entry *   next;
    void *                   block;
    size_t                   size;
};

void *   mp_malloc(const char *, size_t);
void     mp_free(const char * pool_name, void *);
void     mp_free_memory_pool(const char *);
void     mp_dump_memory_pool(const char *);
void     mp_remove_block_from_pool(const char *, void *);

#endif /* !__LIB_MPOOLS_H__ */
