/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996,97 by CyberSolutions GmbH.
 *      All rights reserved.
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
