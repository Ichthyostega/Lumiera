/*
    llist.h - simple intrusive cyclic double linked list

   Copyright (C)
     2003, 2005       Christian Thaeter <chth@gmx.net>
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>

/**
 * @file
 * Intrusive cyclic double linked list
 * There is only one node type which contains a forward and a backward pointer. In a empty initialised node,
 * this pointers point to the node itself. Note that these pointers can never ever become NULL.
 * This lists are used by using one node as 'root' node where its both pointers are the head/tail pointer to the actual list.
 * Care needs to be taken to ensure not to apply any operations meant to be applied to data nodes to the root node.
 * This way is the preferred way to use this lists.
 * Alternatively one can store only a chain of data nodes and use a LList pointer to point to the first item
 * (which might be NULL in case no data is stored). When using the 2nd approach care must be taken since most functions
 * below expect lists to have a root node.
 *
 * This header can be used in 2 different ways:
 * 1) (preferred) just including it provides all functions as static inlined functions. This is the default
 * 2) `#define LLIST_INTERFACE` before including this header gives only the declarations
 *    `#define LLIST_IMPLEMENTATION` before including this header yields in definitions
 *    this can be used to generate a library. This is currently untested and not recommended.
 * The rationale for using inlined functions is that most functions are very small and likely to be used in performance critical parts.
 * Inlining can give a huge performance and optimisation improvement here.
 * The few functions which are slightly larger are expected to be the less common used ones, so inlining them too shouldn't be a problem either
 */


/* TODO __STDC_VERSION__ 199901L
150) This macro was not specified in ISO/IEC 9899:1990 and was specified as 199409L in
ISO/IEC 9899/AMD1:1995. The intention is that this will remain an integer constant of type long
int that is increased with each revision of this International Standard.
*/
#ifdef HAVE_INLINE
#       define LLIST_MACRO static inline
#else
#       ifdef __GNUC__
#               define LLIST_MACRO static __inline__
#       else
#               define LLIST_MACRO static
#       endif
#endif

#if defined(LLIST_INTERFACE)
/* only the interface is generated */
#define LLIST_FUNC(proto, ...) proto
#elif defined(LLIST_IMPLEMENTATION)
/* generate a non inlined implementation */
#define LLIST_FUNC(proto, ...) proto { __VA_ARGS__ }
#else
/* all functions are macro-like inlined */
#define LLIST_FUNC(proto, ...) LLIST_MACRO proto { __VA_ARGS__ }
#endif


/*
 * Type of a llist node.
 */
#ifndef LLIST_DEFINED
#define LLIST_DEFINED
struct llist_struct
{
  struct llist_struct *next;
  struct llist_struct *prev;
};
#endif
typedef struct llist_struct llist;
typedef llist * LList;
typedef const llist * const_LList;
typedef llist ** LList_ref;

/**
 * Macro to instantiate a local llist.
 * @param name of the llist node
 */
#define LLIST_AUTO(name) llist name = {&name,&name}


/*
  some macros for convenience
*/
#define llist_insert_head(list, element) llist_insert_next (list, element)
#define llist_insert_tail(list, element) llist_insert_prev (list, element)
#define llist_head llist_next
#define llist_tail llist_prev

/**
 * cast back from a member of a structure to a pointer of the structure
 */
/* example:
   struct foo
   {
     int bar;
     llist l;
   } x;
   LLIST_TO_STRUCTP (&x.l, foo, l)->bar
*/
#define LLIST_TO_STRUCTP(llist, type, member) \
  ((type*)(((char*)(llist)) - offsetof(type, member)))

/**
 * Iterate forward over a list.
 * @param list the root node of the list to be iterated
 * @param node pointer to the iterated node
 */
#define LLIST_FOREACH(list, node)               \
    for (LList node = llist_head (list);        \
         ! llist_is_end (node, list);           \
         llist_forward (&node))

/**
 * Iterate backward over a list.
 * @param list the root node of the list to be iterated
 * @param node pointer to the iterated node
 */
#define LLIST_FOREACH_REV(list, node)           \
    for (LList node = llist_tail (list);        \
         ! llist_is_end (node, list);           \
         llist_backward (&node))


/**
 * Iterate forward over a range.
 * @param start first node to be iterated
 * @param end node after the last node be iterated
 * @param node pointer to the iterated node
 */
#define LLIST_FORRANGE(start, end, node)        \
    for (LList node = start;                    \
         node != end;                           \
         llist_forward (&node))

/**
 * Iterate backward over a range.
 * @param rstart first node to be iterated
 * @param rend node before the last node be iterated
 * @param node pointer to the iterated node
 */
#define LLIST_FORRANGE_REV(rstart, rend, node)  \
    for (LList node = rstart;                   \
         node != rend;                          \
         llist_backward (&node))


/**
 * Consume a list from head.
 * The body of this statement should remove the head from the list, else it would be a infinite loop
 * @param list the root node of the list to be consumed
 * @param head pointer to the head node
 */
#define LLIST_WHILE_HEAD(list, head)            \
    for (LList head = llist_head (list);        \
         !llist_is_empty (list);                \
         head = llist_head (list))

/**
 * Consume a list from tail.
 * The body of this statement should remove the tail from the list, else it would be a infinite loop
 * @param list the root node of the list to be consumed
 * @param tail pointer to the tail node
 */
#define LLIST_WHILE_TAIL(list, tail)            \
    for (LList tail = llist_tail (list);        \
         !llist_is_empty (list);                \
         tail = llist_tail (list))

/**
 * Initialise a new llist.
 * Must not be applied to a list node which is not empty! Lists need to be initialised
 * before any other operation on them is called.
 * @param self node to be initialised
 */
LLIST_FUNC (LList llist_init (LList self),
            return self->next = self->prev = self;
);

/**
 * Check if a node is not linked with some other node.
 */
LLIST_FUNC (int llist_is_empty (const_LList self),
            return self->next == self;
);

/**
 * Check if self is the only node in a list or self is not in a list.
 * @param self node to be checked
 */
LLIST_FUNC (int llist_is_single (const_LList self),
            return self->next->next == self;
);

/**
 * Check for the head of a list.
 * @param self root of the list
 * @param head expected head of the list
 */
LLIST_FUNC (int llist_is_head (const_LList self, const_LList head),
            return self->next == head;
);

/**
 * Check for the tail of a list.
 * @param self root of the list
 * @param tail expected tail of the list
 */
LLIST_FUNC (int llist_is_tail (const_LList self, const_LList tail),
            return self->prev == tail;
);

/**
 * Check for the end of a list.
 * The end is by definition one past the tail of a list, which is the root node itself.
 * @param self root node of the list
 * @param end expected end of the list
 */
LLIST_FUNC (int llist_is_end (const_LList self, const_LList end),
            return self == end;
);

/**
 * Check if a node is a member of a list.
 * @param self root node of the list
 * @param member node to be searched
 */
LLIST_FUNC (int llist_is_member (const_LList self, const_LList member),
            for (const_LList i = member->next; i != member; i = i->next)
              {
                if (i == self)
                  return 1;
              }
            return 0;
);

/**
 * Check the order of elements in a list.
 * @param self root node of the list
 * @param before expected to be before after
 * @param after expected to be after before
 */
LLIST_FUNC (int llist_is_before_after (const_LList self, const_LList before, const_LList after),
            for (const_LList i = before->next; i != self; i = i->next)
              {
                if (i == after)
                  return 1;
              }
            return 0;
);

/**
 * Count the nodes of a list.
 * @param self root node of the list
 * @return number of nodes in self
 */
LLIST_FUNC (unsigned llist_count (const_LList self),
            unsigned cnt = 0;
            const_LList i = self;
            for (; i->next != self; ++cnt, i = i->next);
            return cnt;
);

/* private, unlink self some any list but leaves self in a uninitialised state */
LLIST_FUNC (void llist_unlink_fast_ (LList self),
            LList nxt = self->next, pre = self->prev;
            nxt->prev = pre;
            pre->next = nxt;
);

/**
 * Remove a node from a list.
 * @param self node to be removed
 * @return self
 */
LLIST_FUNC (LList llist_unlink (LList self),
            llist_unlink_fast_ (self);
            return self->next = self->prev = self;
);

/**
 * Fix a node which got relocated in memory.
 * It is supported to realloc/move list nodes in memory but one must call 'list_relocate' after doing so.
 * IMPORTANT: it is not possible to relocate nodes which are empty this way, nor can this be determined
 * after the relocation, so either llist_init them afterwards or insert a bogus node before moving the node
 * and relocating it and remove it afterwards.
 * @param self node which got relocated
 * @return self
 */
LLIST_FUNC (LList llist_relocate (LList self),
            return self->next->prev = self->prev->next = self;
);


/**
 * Insert a node after another.
 * @param self node after which we want to insert
 * @param next node which shall be inserted after self. Could already linked to a list from where it will be removed.
 * @return self
 */
LLIST_FUNC (LList llist_insert_next (LList self, LList next),
            llist_unlink_fast_ (next);
            self->next->prev = next;
            next->prev = self;
            next->next = self->next;
            self->next = next;
            return self;
);

/**
 * Insert a node before another.
 * @param self node before which we want to insert
 * @param prev node which shall be inserted before self. Could already linked to a list from where it will be removed.
 * @return self
 */
LLIST_FUNC (LList llist_insert_prev (LList self, LList prev),
            llist_unlink_fast_ (prev);
            self->prev->next = prev;
            prev->next = self;
            prev->prev = self->prev;
            self->prev = prev;
            return self;
);


/**
 * Move the content of a list after a node in another list.
 * @param self node after which we want to insert a list
 * @param next rootnode of the list which shall be inserted after self
 * @return self
 * next is a empty list after this call
 */
LLIST_FUNC (LList llist_insertlist_next (LList self, LList next),
            if (!llist_is_empty (next))
              {
                self->next->prev = next->prev;
                next->prev->next = self->next;
                self->next = next->next;
                next->next->prev = self;

                next->prev = next->next = next;
              }
            return self;
);

/**
 * Move the content of a list before a node in another list.
 * @param self node before which we want to insert a list
 * @param prev rootnode of the list which shall be inserted before self
 * @return self
 * prev is a empty list after this call
 */
LLIST_FUNC (LList llist_insertlist_prev (LList self, LList prev),
            if (!llist_is_empty (prev))
              {
                self->prev->next = prev->next;
                prev->next->prev = self->prev;
                self->prev = prev->prev;
                prev->prev->next = self;

                prev->prev = prev->next = prev;
              }
            return self;
);

#if 0             //BUG("needs temporary")
/**
 * Move a range of nodes after a given node.
 * @param self node after which the range shall be inserted
 * @param start first node in range to be moved
 * @param end node after the last node of the range
 */
LLIST_FUNC (LList llist_insertafter_range (LList self, LList start, LList end),
            self->next->prev = end->prev;
            end->prev->next = self->next;
            end->prev = start->prev;
            start->prev->next = end;
            self->next = start;
            start->prev = self;
            return self;
);
#endif

#if 0             //BUG("needs temporary")
/**
 * Move a range of nodes before a given node.
 * @param self node before which the range shall be inserted
 * @param start first node in range to be moved
 * @param end node after the last node of the range
 */
LLIST_FUNC (LList llist_inserbefore_range (LList self, LList start, LList end),
            self->prev->next = start;
            start->prev->next = end;
            end->prev = start->prev;
            start->prev = self->prev;
            self->prev = end->prev;
            end->prev->next = self;
            return self;
);
#endif

/**
 * Swap a node with its next node.
 * @param self node to be advanced
 * @return self
 * advancing will not stop at tail, one has to check that if this is intended
 */
LLIST_FUNC (LList llist_advance (LList self),
            LList tmp = self->next->next;
            tmp->prev = self;
            self->next->prev = self->prev;
            self->prev->next = self->next;
            self->prev = self->next;
            self->next->next = self;
            self->next = tmp;
            return self;
);

/**
 * Swap a node with its previous node.
 * @param self node to be retreated
 * @return self
 * retreating will not stop at head, one has to check that if this is intended
 */
LLIST_FUNC (LList llist_retreat (LList self),
            LList tmp = self->prev->prev;
            tmp->next = self;
            self->prev->next = self->next;
            self->next->prev = self->prev;
            self->next = self->prev;
            self->prev->prev = self;
            self->prev = tmp;
            return self;
);


/**
 * Get next node.
 * @param self current node
 * @return node after self
 * Will not stop at tail
 */
LLIST_FUNC (LList llist_next (const_LList self),
            return self->next;
);

/**
 * Get previous node.
 * @param self current node
 * @return node before self
 * Will not stop at head
 */
LLIST_FUNC (LList llist_prev (const_LList self),
            return self->prev;
);

/**
 * Advance a pointer to a node to its next node.
 * @param self pointer-to-pointer to the current node
 * *self will point to the next node after this call
 */
LLIST_FUNC (void llist_forward (LList_ref self),
            *self = (*self)->next;
);

/**
 * Retreat a pointer to a node to its previous node.
 * @param self pointer-to-pointer to the current node
 * *self will point to the previous node after this call
 */
LLIST_FUNC (void llist_backward (LList_ref self),
            *self = (*self)->prev;
);

/**
 * Get the nth element of a list.
 * @param self list to be queried
 * @param n nth element after (positive n) or before (negative n) self
 * this function does not stop at head/tail.
 */
LLIST_FUNC (LList llist_nth (LList self, int n),
            if (n>0)
              while (n--)
                self = llist_next (self);
            else
              while (n++)
                self = llist_prev (self);
            return self;
);

/**
 * Get the nth element of a list with a stop node.
 * @param self list to be queried
 * @param n nth element after (positive n) or before (negative n) self
 * @param stop node which will abort the iteration
 */
LLIST_FUNC (LList llist_get_nth_stop (LList self, int n, const_LList stop),
            if (n>0)
              while (n--)
                {
                  self = llist_next (self);
                  if (self == stop)
                    return NULL;
                }
            else
              while (n++)
                {
                  self = llist_prev (self);
                  if (self == stop)
                    return NULL;
                }
            return self;
);


/**
 * The comparison function function type.
 * certain sort and find functions depend on a user supplied comparison function
 * @param a first operand for the comparison
 * @param b second operand for the comparison
 * @param extra user supplied data which passed through
 * @return shall return a value less than zero, zero, bigger than zero when
 *         a is less than, equal to, bigger than b
 */
typedef int (*llist_cmpfn)(const_LList a, const_LList b, void* extra);


/**
 * Sort a list.
 * recursive mergesort, needs much extra stackspace (crappy implementation yet) but it reasonable fast
 * @param self list to be sorted
 * @param cmp function for comparing 2 nodes
 * @param extra generic data passed to the cmp function
 */
LLIST_FUNC (LList llist_sort (LList self, llist_cmpfn cmp, void* extra),
            llist left;
            llist right;
            llist_init (&left);
            llist_init (&right);
            unsigned n = 0;
            if (!llist_is_single (self))
              {
                LLIST_WHILE_HEAD (self, head)
                  llist_insert_prev (++n & 1 ? &left : &right, head);

                llist_sort (&left, cmp, extra);
                llist_sort (&right, cmp, extra);

                while (!llist_is_empty (&left) && !llist_is_empty (&right))
                  llist_insert_prev (self, cmp (left.next, right.next, extra) < 0 ? left.next : right.next);

                if (!llist_is_empty (&left))
                  llist_insertlist_prev (self, &left);
                if (!llist_is_empty (&right))
                  llist_insertlist_prev (self, &right);
              }
            return self;
)


/**
 * Find a element in list.
 * searches the list for a element.
 * Does not change the list order.
 * @param self list to be searched
 * @param templ template for the element being searched
 * @param cmp function for comparing 2 nodes
 * @return pointer to the found LList element or NULL if nothing found
 */
LLIST_FUNC (LList llist_find (const_LList self, const_LList templ, llist_cmpfn cmp, void* extra),
            LLIST_FOREACH(self, node)
            {
              if (!cmp (node, templ, extra))
                return node;
            }
            return NULL;
)


/**
 * Find a element in a unsorted list.
 * searches the list until it finds the searched element and moves it then to the head.
 * Useful if the order of the list is not required and few elements are frequently searched.
 * @param self list to be searched
 * @param templ template for the element being searched
 * @param cmp function for comparing 2 nodes
 * @return pointer to the found LList element (head) or NULL if nothing found
 */
LLIST_FUNC (LList llist_ufind (LList self, const_LList templ, llist_cmpfn cmp, void* extra),
            LLIST_FOREACH(self, node)
            {
              if (!cmp (node, templ, extra))
                {
                  if (llist_next(self) != node)
                    llist_insert_next (self, node);
                  return node;
                }
            }
            return NULL;
)


/**
 * Find a element in a sorted list.
 * searches the list until it find the searched element, exits searching when found an element
 * bigger than the searched one.
 * @param self list to be searched
 * @param templ template for the element being searched
 * @param cmp function for comparing 2 nodes
 * @return pointer to the found LList element or NULL if nothing found
 */
LLIST_FUNC (LList llist_sfind (const_LList self, const_LList templ, llist_cmpfn cmp, void* extra),
            LLIST_FOREACH(self, node)
            {
              int c = cmp (node, templ, extra);
              if (!c)
                return node;
              else if (c>0)
                break;
            }
            return NULL;
)



#endif /* LLIST_H */
/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
