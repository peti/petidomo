/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996,97 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include "lists.h"

/* Routines to access the elements of the structures transparently. */

List
getListHead(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return node->ln_Head;
}

const void *
getNodeKey(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return node->ln_Key;
}

const void *
getNodeData(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return node->ln_Data;
}

void
setNodeData(const Node node, const void * data)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return;

    node->ln_Data = data;
}

Node
getFirstNode(const List head)
{
    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    return head->lh_FirstNode;
}

Node
getNextNode(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return node->ln_Next;
}

Node
getPrevNode(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return node->ln_Prev;
}

bool
isListEmpty(const List head)
{
    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return 1;

    return (getFirstNode(head) == NULL);
}

bool
isEndOfList(const Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    return (node->ln_Next == NULL);
}

/* InitList() will initialize a header structure for a linked list and
   return the pointer. The parameter points to a function that is used
   to compare the keys. By providing a function of their own, the user
   is able to maintain linked lists which use an entirely different
   type of data and key, without having to change the code here.

   If NULL is given instead of a pointer, the list will use
   strcasecmp() as default, assuming the keys are strings.
*/

List
InitList(int (*compare)(const void *, const void *))
{
    List  head;

    if ((head = malloc(sizeof(struct ListHead)))) {
	head->lh_FirstNode = NULL;
	head->lh_Compare = ((compare != NULL) ?
			    compare :
			    (int (*)(const void *, const void *)) strcasecmp);
    }

    return head;
}


/* FreeList() will destroy the linked list given as parameter and
   return all allocated memory to the system. FreeList() does -not-
   know about the key and data buffers in each Node structure. The
   caller has to take care of returning these buffers himself. */

void
FreeList(List head)
{
    Node currNode, nextNode;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return;

    currNode = getFirstNode(head);
    free(head);
    if (currNode == NULL)
      return;
    for (nextNode = getNextNode(currNode);
	 currNode != NULL;
	 currNode = nextNode) {
	nextNode = getNextNode(currNode);
	FreeNode(currNode);
    }
}

/* AppendNode() will a new node at the end of the specified list. */

Node
AppendNode(List head, const void * key, const void * data)
{
    Node newNode, currNode;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    /* Create node structure. */

    newNode = InitNode(head, key, data);
    if ((newNode == NULL))
      return NULL;

    /* Append the node at the end of the list. */

    if (isListEmpty(head)) {
	head->lh_FirstNode = newNode;
    }
    else {
	for (currNode = getFirstNode(head);
	     ! isEndOfList(currNode);
	     currNode = getNextNode(currNode))
	  ;
	currNode->ln_Next = newNode;
	newNode->ln_Prev = currNode;
    }

    return newNode;
}

/* InitNode() will accept the key- and the data-pointers and then set
   up a correctly initialized node structure. */

Node
InitNode(const List head, const void * key, const void * data)
{
    Node newNode;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    newNode = malloc(sizeof(struct ListNode));
    if (newNode != NULL) {
	newNode->ln_Next = NULL;
	newNode->ln_Prev = NULL;
	newNode->ln_Head = head;
	newNode->ln_Key = key;
	newNode->ln_Data = data;
    }
    return newNode;
}

/* Return all allocated memory from the node structure to the system. */

void
FreeNode(Node node)
{
    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return;

    free(node);
}

/* RemoveNode() remove a node from a list. */

void
RemoveNode(const Node node)
{
    Node prevNode, nextNode;

    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return;

    prevNode = getPrevNode(node);
    nextNode = getNextNode(node);

    if (prevNode == NULL) {	/* We are the first node. */
	getListHead(node)->lh_FirstNode = nextNode;
	if (nextNode) {  	/* We're not the last one. */
	    nextNode->ln_Prev = NULL;
	}
    }
    else if (nextNode == NULL) { /* We are the last node. */
	prevNode->ln_Next = NULL;
    }
    else {
	/* We're neither first nor last node. */
	nextNode->ln_Prev = prevNode;
	prevNode->ln_Next = nextNode;
    }
}

Node
InsertNodeBeforeNode(Node node, const void *key, const void *data)
{
    Node   newNode, prevNode;
    List   head;

    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    head = node->ln_Head;
    prevNode = getPrevNode(node);
    newNode = InitNode(head, key, data);
    if (newNode == NULL)
      return NULL;

    if (prevNode == NULL) { /* We'll become the new first node. */
	head->lh_FirstNode = newNode;
	newNode->ln_Next = node;
	node->ln_Prev = newNode;
    }
    else {
	prevNode->ln_Next = newNode;
	newNode->ln_Next = node;
	newNode->ln_Prev = prevNode;
	node->ln_Prev = newNode;
    }

    return newNode;
}

Node
InsertNodeAfterNode(Node node, const void *key, const void *data)
{
    Node   newNode, nextNode;
    List   head;

    /* Sanity checks. */

    assert(node != NULL);
    if (node == NULL)
      return NULL;

    head = node->ln_Head;
    nextNode = getNextNode(node);
    newNode = InitNode(head, key, data);
    if (newNode == NULL)
      return NULL;

    if (nextNode == NULL) { /* We'll become the new last node. */
	node->ln_Next = newNode;
	newNode->ln_Prev = node;
    }
    else {
	node->ln_Next = newNode;
	newNode->ln_Next = nextNode;
	newNode->ln_Prev = node;
	nextNode->ln_Prev = newNode;
    }

    return newNode;
}

/* Insert a node into the list in lexicographical order. The
   comparison is done between the keys of each element, using the
   function provided by the caller. */

Node
InsertNodeByKey(List head, const void * key, const void * data)
{
    Node   currNode;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    currNode = getFirstNode(head);

    while(currNode != NULL) {
	if (((*head->lh_Compare)(getNodeKey(currNode), key) < 0))
	  currNode = getNextNode(currNode);
	else
	  return InsertNodeBeforeNode(currNode, key, data);
    }

    /* We are going to become the last list element. */

    return AppendNode(head, key, data);
}

/* Find a node in the list by it's key. If no such element can be
   found, return NULL. If several elements have the same key, the
   first hit is returned.

   It is guaranteed, that the argument the user provides here, will be
   used as second parameter, when the callback is called. This may be
   important when comparing differing structures to each other! */

Node
FindNodeByKey(List head, const void * key)
{
    Node   currNode;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    currNode = getFirstNode(head);

    while(currNode != NULL) {
	if (((*head->lh_Compare)(getNodeKey(currNode), key) != 0))
	  currNode = getNextNode(currNode);
	else
	  return currNode;
    }

    return NULL;
}

/* This routines counts the elements in the specified list and returns
   the number. */

unsigned int
CountElements(List head)
{
    unsigned int  i;
    Node          node;

    /* Sanity checks. */

    assert(head != NULL);
    if (head == NULL)
      return NULL;

    for (i = 0, node = getFirstNode(head); node != NULL; node = getNextNode(node))
      i++;

    return i;
}
