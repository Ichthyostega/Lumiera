/*
    psplay.h - probabilistic splay tree

   Copyright (C)       (contributed to CinelerraCV)
     2004-2006,       Christian Thaeter <chth@gmx.net>
   Copyright (C)     
     2007, 2008,      Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/


/** @file psplay.h
 ** Probabilistic splay tree.
 ** A splay trees is self-optimising (in contrast to self-balancing) datastructure.
 ** We introduce here a probabilistic bottom up approach which reduces the splay costs.
 ** Without affecting the performance. The randomisation gives also some insurance that
 ** worst case situations are extremely unlikely.
 ** 
 ** Tree nodes are very small (just 2 pointers) and are intrusively placed into the users
 ** datastructure.
 */


#ifndef LIB_PSPLAY_H
#define LIB_PSPLAY_H

#include <stdint.h>
#include <stdio.h>


/**
 * Type and handle for a psplay tree node
 * This node have to be placed inside users data.
 */
typedef struct psplaynode_struct psplaynode;
typedef psplaynode* PSplaynode;
struct psplaynode_struct
{
  PSplaynode left;
  PSplaynode right;
};

#define PSPLAYNODE_INITIALIZER {NULL, NULL}

/**
 * Function use to compare keys
 * @param a first key
 * @param b second key
 * @return shall return -1/0/1 when a is less than/equal to/biggier than b.
 */
typedef int (*psplay_cmp_fn)(const void* a, const void* b);


/**
 * Destructor for user defined data
 * Called when an element got removed from a splay tree.
 * @param node pointer to the intrusive node inside the users datastructure
 * The user is responsible for casting 'node' back to his real datastructure (maybe with OFFSET_OF()),
 * free all resources associated with it and finally free the datastructure itself.
 */
typedef void (*psplay_delete_fn)(PSplaynode node);


/**
 * Retrieve the key from a user datastructure
 * @param node pointer to the intrusive node inside the users datastructure
 * This functiom must return a pointer to the key under which the user stores his data.
 */
typedef const void* (*psplay_key_fn)(const PSplaynode node);


/**
 * Type and handle for a psplay root structure
 * This structure shall be treated opaque, its only defined in the header to allow
 * one to integrate it directly instead referencing it.
 */
typedef struct psplay_struct psplay;
typedef psplay* PSplay;

struct psplay_struct
{
  PSplaynode tree;                      /* the tree */
  PSplaynode* found_parent;             /* maybe direct parent of last found node, used for fast remove */
  psplay_cmp_fn cmp;
  psplay_key_fn key;
  psplay_delete_fn del;

  size_t elem_cnt;
  unsigned log2;                        /* roughly log2 of the elem_cnt*/
};


/**
 * Number of elements in tree
 * @param self pointer to the tree
 * @return number of elements
 */
static inline size_t
psplay_nelements (PSplay self)
{
  return self->elem_cnt;
};


/**
 * Initialize a splay tree
 * @param self pointer to the psplay structure
 * @param cmp user supplied compare function
 * @param key user supplied function to retrieve a key
 * @param delete user supplied destructor function or NULL if no destructor is necessary
 * @return self
 */
PSplay
psplay_init (PSplay self, psplay_cmp_fn cmp, psplay_key_fn key, psplay_delete_fn del);


/**
 * Destroy a splay tree
 * Frees all elements and associated resources of a splay tree
 * @param self pointer to the psplay structure
 * @return self
 */
PSplay
psplay_destroy (PSplay self);

/**
 * Allocate a splay tree
 * @param cmp user supplied compare function
 * @param key user supplied function to retrieve a key
 * @param delete user supplied destructor function or NULL if no destructor is necessary
 * @return allcoated splay tree or NULL on error
 */
PSplay
psplay_new (psplay_cmp_fn cmp, psplay_key_fn key, psplay_delete_fn del);


/**
 * Delete a splay tree
 * Frees all elements and associated resources of a splay tree and then itseld
 * @param self pointer to the psplay structure
 */
void
psplay_delete (PSplay self);


/**
 * Initialise a splay tree node
 * The user has to place this nodes within his datastructure and must
 * Initialise them before using them.
 * @param self pointer to the node to be initialised
 * @return self
 */
PSplaynode
psplaynode_init (PSplaynode self);


/**
 * Insert a element into a splay tree
 * @param self pointer to the splay tree
 * @param node pointer to the node to be inserted
 * @param splayfactor weight for the probabilistic splaying,
 *   0 disables the splaying, 100 is the expected normal value
 *   use 100 when in doubt
 * @return self or NULL when a node with same key already in the tree
 */
PSplaynode
psplay_insert (PSplay self, PSplaynode node, int splayfactor);


/**
 * Find a element in a splay tree
 * @param self pointer to the splay tree
 * @param key pointer to the key to be searched
 * @param splayfactor weight for the probabilistic splaying,
 *   0 disables the splaying, 100 is the expected normal value
 * @return found node or NULL if the key was not found in the tree
 */
PSplaynode
psplay_find (PSplay self, const void* key, int splayfactor);


/**
 * Remove a node from a splay tree
 * @param self pointer to the splay tree
 * @param node node to be removed
 * @return pointer to the removed node
 * removal is optimised for the case where one call it immediately after one did a
 * psplay_find() as last operation on that tree
 */
PSplaynode
psplay_remove (PSplay self, PSplaynode node);


/**
 * Remove a node by key from a splay tree
 * @param self pointer to the splay tree
 * @param key key of the node to be removed
 * @return pointer to the removed node
 */
PSplaynode
psplay_remove_key (PSplay self, void* key);


/**
 * Delete a node from a splay tree
 * @param self pointer to the splay tree
 * @param node node to be removed
 * Calls the registered delete handler, frees all resources.
 */
void
psplay_delete_node (PSplay self, PSplaynode node);


/**
 * Delete a node by key from a splay tree
 * @param self pointer to the splay tree
 * @param key key of the node to be removed
 * Calls the registered delete handler, frees all resources.
 */
void
psplay_delete_key (PSplay self, void* key);


enum psplay_order_enum
  {
    PSPLAY_PREORDER,
    PSPLAY_INORDER,
    PSPLAY_POSTORDER
  };

/**
 * Traverse a splay tree
 * Traversing a tree calls a user supplied action three times
 * An 'action' must not alter the tree itself but it can indicate aborting the tree traversal and
 * how the current node is handled by its return value.
 * @param node pointer to the currently traversed node
 * @param which state of the traversal:
 *   PSPLAY_PREORDER before visiting the left subtree,
 *   PSPLAY_INORDER after visiting the left subtree and before the right subtree
 *   PSPLAY_POSTORDER finally after visiting the right subtree.
 * Example: For to traverse the tree in order action would only handle PSPLAY_INORDER.
 * This action shall return PSPLAY_CONT when the traversal of the tree shall continue.
 * @param level depth of the node in the tree
 * @param data user supplied data which is transparently passed around
 * @return a pointer to a function which indicates how to proceed, there are three special
 *  return values predefined:
 *  PSPLAY_CONT   - continue with the traversal
 *  PSPLAY_STOP   - stop the traversal
 *  PSPLAY_REMOVE - stops the traversal and removes the current node, calling the delete handler
 *  any other psplay_delete_fn  - stops the traversal and removes the current node, calling the returned delete handler with it
 */
typedef psplay_delete_fn (*psplay_action_fn)(PSplaynode node, const enum psplay_order_enum which, int level, void* data);

extern const psplay_delete_fn PSPLAY_CONT;
extern const psplay_delete_fn PSPLAY_STOP;
extern const psplay_delete_fn PSPLAY_REMOVE;

/**
 * Start a tree traversal
 * @param self the tree to be traversed
 * @param node pointer to root node where traversal shall start, use NULL for the whole tree
 * @param action handler function as defined above
 * @param level initial value for the level
 * @param data user supplied data which is transparently passed to the action
 * @return 0 when the tree traversal got aborted (by anything but PSPLAY_CONT as action handler return)
 *         1 when the whole tree was traversed successfully
 */
int
psplay_walk (PSplay self, PSplaynode node, psplay_action_fn action, int level, void* data);


void
psplay_dump (PSplay self, FILE* dest);

#endif /*LIB_PSPLAY_H*/
