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
