/*
    psplay.c - probabilistic splay tree

  Copyright (C)
    2004, 2005, 2006,   Christian Thaeter <chth@gmx.net>
  Copyright (C)         CinelerraCV
    2007, 2008,         Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


/** @file psplay.c
 ** Probabilistic splay tree implementation
 */

#include "include/logging.h"
#include "lib/psplay.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nobug.h>

//NOBUG_DEFINE_FLAG (psplay);

#ifndef PSPLAY_TRAIL_DEPTH
#define PSPLAY_TRAIL_DEPTH 128
#endif

/*
  probabilistic distribution, this are the direct splay equations used to determine if to splay
  or break out of the splaying algorithm.

  useable variables/functions are:
   self->log2                   - log2 of the tree elements, this would be the depth of a fully balanced tree
   splayfactor                  - user defined weigth for splaying, we define '100' to be the default
   depth                        - depth of the current node in the tree
   trail->dir                   - dervitation from tree center
   psplay_fast_prng ()          - returns a prng in the range 1...2^31
*/

#define PSPLAY_FORMULA (self->log2*100/(depth + (psplay_fast_prng () & 63)) + trail->dir) * splayfactor

#ifndef PSPLAY_PROB_ZIGZIG
#define PSPLAY_PROB_ZIGZIG 5000
#endif
#ifndef PSPLAY_PROB_ZIGZAG
#define PSPLAY_PROB_ZIGZAG 2500
#endif



/* simple prng with 2^31-1 cycle */
static inline uint32_t psplay_fast_prng ()
{
  static uint32_t rnd=0xbabeface;
  return rnd = rnd<<1 ^ ((rnd >> 30) & 1) ^ ((rnd>>2) & 1);
}


PSplay
psplay_init (PSplay self, psplay_cmp_fn cmp, psplay_key_fn key, psplay_delete_fn del)
{
  //NOBUG_INIT_FLAG (psplay);
  TRACE (psplay_dbg);
  REQUIRE (cmp);
  REQUIRE (key);

  if (self)
    {
      self->tree = NULL;
      self->found_parent = &self->tree;
      self->cmp = cmp;
      self->key = key;
      self->del = del;
      self->elem_cnt = 0;
      self->log2 = 0;
    }
  return self;
}


PSplay
psplay_new (psplay_cmp_fn cmp, psplay_key_fn key, psplay_delete_fn del)
{
  PSplay self = malloc (sizeof *self);
  if (self)
    {
      psplay_init (self , cmp, key, del);
    }
  return self;
}



PSplay
psplay_destroy (PSplay self)
{
  TRACE (psplay_dbg);
  if (self) while (self->tree)
    {
      PSplaynode n = psplay_remove (self, self->tree);
      if (self->del)
        self->del (n);
    }
  return self;
}


void
psplay_delete (PSplay self)
{
  free (psplay_destroy (self));
}


PSplaynode
psplaynode_init (PSplaynode self)
{
  if (self)
    self->left = self->right = NULL;
  return self;
}



/*
  Lookup operations (lookup and insert) record the path as they decend into the tree
  this will allow bottom up splaying without storing 'up' pointers in the node.
  The length of this trail (PSPLAY_TRAIL_DEPTH) also define the maximal limit on how much
  a node can be splayed up giving some hard bound for the splay operation.

  General wisdom tells that top down splaying is more efficent to implement than bottom
  up splaying. Nevertheless we do bottom up splaying here because we can decide
  randomly on each level if we want to continue splaying or not. No splaying
  is certainly more efficent than top-down splaying.
*/
struct psplaytrail
{
  int dir;
  unsigned depth;
  PSplaynode* trail[PSPLAY_TRAIL_DEPTH];
};

static inline unsigned
trailidx (unsigned n)
{
  return n & (PSPLAY_TRAIL_DEPTH-1);
}


static inline void
psplay_splay (PSplay self, struct psplaytrail* trail, unsigned splayfactor)
{
  TRACE (psplay_dbg, "%p %u", self, splayfactor);

  if (trail->dir < 0) trail->dir = - trail->dir;

  for (unsigned lim = PSPLAY_TRAIL_DEPTH, depth = trail->depth; lim > 2 && depth > 2; lim-=2, depth-=2)
    {
      PSplaynode node = *trail->trail [trailidx (depth)];
      PSplaynode parent = *trail->trail [trailidx (depth-1)];
      PSplaynode grandparent = *trail->trail [trailidx (depth-2)];

      unsigned r = PSPLAY_FORMULA;
      TRACE (psplay_dbg, "r is %u", r);

      if (parent == grandparent->left)
        {
          TRACE (psplay_dbg, "ZIG..");
          if (node == parent->left)
            {
              TRACE (psplay_dbg, "..ZIG");
              if (r < PSPLAY_PROB_ZIGZIG)
                {
                  TRACE (psplay_dbg, "BREAK");
                  return;
                }

              grandparent->left = parent->right;
              parent->right = grandparent;

              parent->left = node->right;
              node->right = parent;
            }
          else
            {
              TRACE (psplay_dbg, "..ZAG");
              if (r < PSPLAY_PROB_ZIGZAG)
                {
                  TRACE (psplay_dbg, "BREAK");
                  return;
                }

              parent->right = node->left;
              node->left = parent;

              grandparent->left = node->right;
              node->right = grandparent;
            }
        }
      else
        {
          TRACE (psplay_dbg, "ZAG..");
          if (node == parent->left)
            {
              TRACE (psplay_dbg, "..ZIG");
              if (r < PSPLAY_PROB_ZIGZAG)
                {
                  TRACE (psplay_dbg, "BREAK");
                  return;
                }

              parent->left = node->right;
              node->right = parent;

              grandparent->right = node->left;
              node->left = grandparent;
            }
          else
            {
              TRACE (psplay_dbg, "..ZAG");
              if (r < PSPLAY_PROB_ZIGZIG)
                {
                  TRACE (psplay_dbg, "BREAK");
                  return;
                }

              grandparent->right = parent->left;
              parent->left = grandparent;

              parent->right = node->left;
              node->left = parent;
            }
        }
      *trail->trail [trailidx (depth-2)] = node;
    }
}


PSplaynode
psplay_insert (PSplay self, PSplaynode node, int splayfactor)
{
  TRACE (psplay_dbg);
  PSplaynode n = self->tree;
  struct psplaytrail trail;

  trail.dir = 0;
  trail.depth = 0;
  trail.trail [0] = &self->tree;

  if (!n)
    self->tree = node;
  else
    {
      while (n != node)
        {
          int c;
          c = self->cmp (self->key (node), self->key (n));
          ++trail.depth;

          if (c < 0)
            {
              --trail.dir;
              if (!n->left)
                n->left = node;
              trail.trail [trailidx (trail.depth)] = &n->left;
              n = n->left;
            }
          else if (c > 0)
            {
              ++trail.dir;
              if (!n->right)
                n->right = node;
              trail.trail [trailidx (trail.depth)] = &n->right;
              n = n->right;
            }
          else
            {
              WARN (psplay_dbg, "dropping duplicate entry for psplay");
              ///////////////////////////TODO policy for multiple entered items (before, after, fail, replace)
              return NULL;
            }
        }
    }
  ++self->elem_cnt;
  if (self->elem_cnt >= 1UL<<self->log2) ++self->log2;
  if (splayfactor && trail.depth > 2)
    psplay_splay (self, &trail, splayfactor);
  return node;
}



PSplaynode
psplay_find (PSplay self, const void* key, int splayfactor)
{
  TRACE (psplay_dbg);
  PSplaynode node = self->tree;
  struct psplaytrail trail;
  trail.dir = 0;
  trail.depth = 0;
  trail.trail [0] = &self->tree;

  while (node)
    {
      int c;
      c = self->cmp (key, self->key (node));
      ++trail.depth;

      if (c < 0)
        {
          --trail.dir;
          trail.trail [trailidx (trail.depth)] = &node->left;
          node = node->left;
        }
      else if (c > 0)
        {
          ++trail.dir;
          trail.trail [trailidx (trail.depth)] = &node->right;
          node = node->right;
        }
      else
        {
          self->found_parent = trail.trail [trailidx (--trail.depth)];
          break;
        }
    }
  if (node && splayfactor && trail.depth > 2)
    psplay_splay (self, &trail, splayfactor);
  return node;
}


PSplaynode
psplay_remove (PSplay self, PSplaynode node)
{
  TRACE (psplay_dbg);
  if (!node) return NULL;

  PSplaynode* r = self->found_parent;

  while (*r != node)
    {
      if (!psplay_find (self, self->key (node), 0))
        {
          WARN (psplay_dbg, "node %p is not in splay tree %p", node, self);
          return NULL;
        }
      r = self->found_parent;
    }

  if (!node->left)
    *r = node->right;
  else if (!node->right)
    *r = node->left;
  else
    {
      PSplaynode i, iparent = NULL;
      if (psplay_fast_prng()&1) /* 50% probability removing left or right wards */
        {
          for (i = node->left; i->right; iparent = i, i = i->right);
          if (iparent)
            iparent->right = i->left;
          if (node->left != i)
            i->left = node->left;
          i->right = node->right;
        }
      else
        {
          for (i = node->right; i->left; iparent = i, i = i->left);
          if (iparent)
            iparent->left = i->right;
          if (node->right != i)
            i->right = node->right;
          i->left = node->left;
        }
      *r = i;
    }
  --self->elem_cnt;
  if (self->elem_cnt < 1UL<<self->log2) --self->log2;
  return node;
}


PSplaynode
psplay_remove_key (PSplay self, void* key)
{
  return psplay_remove (self, psplay_find (self, key, 0));
}


void
psplay_delete_node (PSplay self, PSplaynode node)
{
  if (node)
    self->del (psplay_remove (self, node));
}


void
psplay_delete_key (PSplay self, void* key)
{
  PSplaynode node = psplay_find (self, key, 0);
  psplay_delete_node (self, node);
}



const psplay_delete_fn PSPLAY_CONT = (psplay_delete_fn)0x0;
const psplay_delete_fn PSPLAY_STOP = (psplay_delete_fn)0x1;
const psplay_delete_fn PSPLAY_REMOVE = (psplay_delete_fn)0x2;

static int
psplay_handle (PSplay self, PSplaynode node, psplay_delete_fn res)
{
  if (res == PSPLAY_CONT)
    return 1;

  if (res == PSPLAY_STOP)
    ;
  else if (res == PSPLAY_REMOVE)
    {
      psplay_remove (self, node);
      if (self->del)
        self->del (node);
    }
  else
    {
      psplay_remove (self, node);
      res (node);
    }
  return 0;
}


int
psplay_walk (PSplay self, PSplaynode node, psplay_action_fn action, int level, void* data)
{
  if (!self->tree)
    return 1;

  if (!node)
    node = self->tree;

  psplay_delete_fn res;

  res = action (node, PSPLAY_PREORDER, level, data);
  if (!psplay_handle (self, node, res))
    return 0;

  if (node->left)
    if (!psplay_walk (self, node->left, action, level+1, data))
      return 0;

  res = action (node, PSPLAY_INORDER, level, data);
  if (!psplay_handle (self, node, res))
    return 0;

  if (node->right)
    if (!psplay_walk (self, node->right, action, level+1, data))
      return 0;

  res = action (node, PSPLAY_POSTORDER, level, data);
  if (!psplay_handle (self, node, res))
    return 0;

  return 1;
}


static psplay_delete_fn
psplay_print_node (PSplaynode node, const enum psplay_order_enum which, int level, void* data)
{
  FILE* fh = data;
  static char* sp = "                                        ";
  if (level>40)
    {
      if (which == PSPLAY_PREORDER)
        fprintf (fh, "%s ...\n", sp);
      return PSPLAY_CONT;
    }

  switch (which)
    {
    case PSPLAY_PREORDER:
      fprintf (fh, "%s%p\n", sp+40-level, node);
      if (node->left)
        fprintf (fh, "%sleft %p\n", sp+40-level, node->left);
      break;
    case PSPLAY_INORDER:
      if (node->right)
        fprintf (fh, "%sright %p\n", sp+40-level, node->right);
      break;
    case PSPLAY_POSTORDER:
      break;
    }

  return PSPLAY_CONT;
}

void
psplay_dump (PSplay self, FILE* dest)
{
  fprintf (dest, "root %p\n", self->tree);
  psplay_walk (self, NULL, psplay_print_node, 0, dest);
}


/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
