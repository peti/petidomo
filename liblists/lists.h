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

   You should have received a copy of the GNU General Public License
   along with OpenPetidomo; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __LIB_LISTS_H__
#define __LIB_LISTS_H__ 1

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

/********** Useful defines and declarations **********/

#ifndef __cplusplus
#ifndef __HAVE_DEFINED_BOOL__
#  define __HAVE_DEFINED_BOOL__ 1
typedef int bool;
#endif
#ifndef FALSE
#  define FALSE (0==1)
#endif
#ifndef TRUE
#  define TRUE (1==1)
#endif
#endif

/********** Structures **********/

struct ListHead {
    struct ListNode *   lh_FirstNode;
    int                 (*lh_Compare)(const void *, const void *);
};
typedef struct ListHead * List;

struct ListNode {
    struct ListNode *   ln_Next;
    struct ListNode *   ln_Prev;
    struct ListHead *   ln_Head;
    const void *        ln_Key;
    const void *        ln_Data;
};
typedef struct ListNode * Node;

/********** Prototypes **********/

List getListHead(const Node node);
const void * getNodeKey(const Node node);
const void * getNodeData(const Node node);
void setNodeData(const Node node, const void * data);
Node getFirstNode(const List head);
Node getNextNode(const Node node);
Node getPrevNode(const Node node);
bool isListEmpty(const List head);
bool isEndOfList(const Node node);
List InitList(int (*compare)(const void *, const void *));
void FreeList(List head);
Node AppendNode(List head, const void * key, const void * data);
Node InitNode(const List head, const void * key, const void * data);
void FreeNode(Node node);
void RemoveNode(const Node node);
Node InsertNodeBeforeNode(Node node, const void *key, const void *data);
Node InsertNodeAfterNode(Node node, const void *key, const void *data);
Node InsertNodeByKey(List head, const void * key, const void * data);
Node FindNodeByKey(List head, const void * key);
unsigned int CountElements(List);

#endif /* !__LIB_LISTS_H__ */
