/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../liblists/lists.h"
#include "mpools.h"

static List mpools_list = NULL;

/********** Internal routines **********/

static int
GetMemoryPool(const char * pool_name,
	      Node * node_ptr,
	      struct mp_list_entry ** mpool_entry_ptr)
{
    Node           node;

    /* Sanity checks. */

    assert(mpools_list != NULL);
    assert(pool_name != NULL);
    if (pool_name == NULL)
      return -1;

    /* Find the pool of the right name. */

    node = FindNodeByKey(mpools_list, (const void *) pool_name);
    if (node != NULL) {
	if (node_ptr != NULL)
	  *node_ptr = node;
	if (mpool_entry_ptr != NULL)
	  *mpool_entry_ptr = (struct mp_list_entry *) getNodeData(node);
	return 0;
    }

    /* No entry available. So we create one. */

    pool_name = strdup(pool_name);
    if (pool_name == NULL)
      return -1;

    node = AppendNode(mpools_list, (const void *) pool_name, NULL);
    if (node == NULL) {
	free((char *) pool_name);
	return -1;
    }

    /* Return result to caller. */

    if (node_ptr != NULL)
      *node_ptr = node;
    if (mpool_entry_ptr != NULL)
      *mpool_entry_ptr = (struct mp_list_entry *) getNodeData(node);

    return 0;
}


/********** User interface **********/

/* mp_malloc() allocates a block of memory using malloc(3), adds the
   pointer and the size of the block to the memory pool as identified
   by the 'pool_name' argument and returns the address of the memory
   block to the caller.

   Memory pools are identified by a byte string, terminated with a
   zero ('\\0') byte. The names are compared using strcmp(3), which is
   case-sensitive.

   RETURNS: If successfull, mp_malloc() returns a pointer to the newly
   allocated memory block. In case of failure, NULL is returned
   instead.
*/

void *
mp_malloc(const char * pool_name /* ID-String of the memory pool. */,
	  size_t block_size	 /* Size of the requested memory block. */
	  )
{
    struct mp_list_entry *   mpool;
    Node                     node;
    void *                   block;

    /* Sanity checks. */

    assert(pool_name != NULL);
    if (pool_name == NULL || block_size <= 0)
      return NULL;

    /* Init the internal list structure, if it isn't already. */

    if (mpools_list == NULL) {
	mpools_list = InitList((int (*)(const void *, const void *)) strcmp);
	if (mpools_list == NULL)
	  return NULL;
    }

    /* Get the pool structure. */

    if (GetMemoryPool(pool_name, &node, NULL) != 0)
      return NULL;

    /* Allocate the memory. */

    mpool = malloc(sizeof(struct mp_list_entry));
    if (mpool == NULL)
      return NULL;

    block = malloc(block_size);
    if (block == NULL) {
	free(mpool);
	return NULL;
    }

    /* Init the mpool structure. */

    mpool->next = NULL;
    mpool->block = block;
    mpool->size = block_size;

    /* Now append the structure to our list. */

    if (getNodeData(node) != NULL)
      mpool->next = (struct mp_list_entry *) getNodeData(node);
    setNodeData(node, (const void *) mpool);

    return block;
}

/* mp_free() will return the previously allocated memory block to the
   system and remove the entry from the memory pool. The memory block
   has to be previously allocated by mp_malloc(), or mp_free() won't
   do anything.

   It is safe to call mp_free() several times for the same memory
   block, or for an invalid memory block, that hasn't been allocated
   by mp_malloc(). mp_free will detect this and return without doing
   anything.
*/

void
mp_free(const char * pool_name  /* ID-String of the memory pool. */,
	void * block		/* Pointer to a memory block
				   previously allocated by mp_malloc(). */
	)
{
    struct mp_list_entry *   mpool,
                         *   prev_mpool;
    Node                     node;

    /* Sanity checks. */

    assert(pool_name != NULL);
    if (pool_name == NULL)
      return;

    /* Init the internal list structure, if it isn't already. */

    if (mpools_list == NULL) {
	mpools_list = InitList(NULL);
	if (mpools_list == NULL)
	  return;
    }

    /* Get the pool structure. */

    if (GetMemoryPool(pool_name, &node, &mpool) != 0)
      return;

    /* Find the block we should free. */

    for (prev_mpool = NULL;
	 mpool != NULL && mpool->block != block;
	 prev_mpool = mpool, mpool = mpool->next)
      ;

    if (mpool == NULL) {	/* block not found */
	printf("Warning\n");
	return;
    }

    /* Remove the node from the linked list. */

    if (prev_mpool == NULL)
      setNodeData(node, mpool->next);
    else
      prev_mpool->next = mpool->next;

    /* And free it plus our own structure stuff. */

    free(mpool->block);
    free(mpool);
}


/*
   Remove the provided memory block from the memory pool, without
   freeing the memory itself.

   It is safe to call mp_remove_block_from_pool() with invalid data.
 */

void
mp_remove_block_from_pool(const char * pool_name  /* ID-String of the memory pool. */,
			  void * block		  /* Pointer to a memory block
						     previously allocated by mp_malloc(). */
			  )
{
    struct mp_list_entry *   mpool,
                         *   prev_mpool;
    Node                     node;

    /* Sanity checks. */

    assert(pool_name != NULL);
    assert(block != NULL);
    if (!pool_name || !block)
      return;

    /* Init the internal list structure, if it isn't already. */

    if (mpools_list == NULL) {
	mpools_list = InitList(NULL);
	return;
    }

    /* Get the pool structure. */

    if (GetMemoryPool(pool_name, &node, &mpool) != 0)
      return;

    /* Find the block we should free. */

    for (prev_mpool = NULL;
	 mpool != NULL && mpool->block != block;
	 prev_mpool = mpool, mpool = mpool->next)
      ;

    if (mpool == NULL) 	/* block not found */
      return;

    /* Remove the node from the linked list. */

    if (prev_mpool == NULL)
      setNodeData(node, mpool->next);
    else
      prev_mpool->next = mpool->next;

    /* Free the structure stuff. */

    free(mpool);
}

/* This routine will return all allocated memory blocks contained in
   pool 'pool_name' to the system.
*/

void
mp_free_memory_pool(const char * pool_name /* ID-String of the memory pool. */
		    )
{
    struct mp_list_entry *    mpool_entry,
	                 *    next_mpool_entry;
    Node                      node;

    /* Sanity checks. */

    assert(pool_name != NULL);
    if (pool_name == NULL)
      return;

    if (mpools_list == NULL)
      return;

    /* Find our memory pool. */

    node = FindNodeByKey(mpools_list, pool_name);
    if (node == NULL)
      return;
    mpool_entry = (struct mp_list_entry *) getNodeData(node);

    /* And now we move the pool completely. */

    RemoveNode(node);
    free((void *) getNodeKey(node)); /* kill the pool name buffer */
    FreeNode(node);

    for ( ; mpool_entry != NULL; mpool_entry = next_mpool_entry) {
	next_mpool_entry = mpool_entry->next;
	free(mpool_entry->block);
	free(mpool_entry);
    }
}


/* This routine is experimental and for debugging purposes only. In
   the current version, it will dump the contents of pool 'pool_name'
   to the screen, using printf(3).
*/

void
mp_dump_memory_pool(const char * pool_name /* ID-String of the memory pool. */
		    )
{
    struct mp_list_entry *    mpool_entry;
    unsigned int              total;

    /* Sanity checks. */

    assert(pool_name != NULL);
    if (pool_name == NULL)
      return;

    /* Dump the specified pool. */

    if (GetMemoryPool(pool_name, NULL, &mpool_entry) != 0)
      return;

    if (mpool_entry == NULL) {
	printf("mpool \"%s\" is empty.\n", pool_name);
	return;
    }

    for (total = 0; mpool_entry != NULL; mpool_entry = mpool_entry->next) {
	printf("\"%s\": %d byte block at $%08x.\n", pool_name, mpool_entry->size,
	       (unsigned int) mpool_entry->block);
	total += mpool_entry->size;
    }
    printf("Total size of mpool \"%s\" is %u byte.\n", pool_name, total);
}
