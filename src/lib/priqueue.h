/*
    priqueue - priority queue

  Copyright (C)
    2011                        Christian Thaeter <ct@pipapo.org>

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

#ifndef PRIQUEUE_H
#define PRIQUEUE_H

/*
  This is a very minimalistic priority queue implementation based on a binary heap. Only 'insert' 'remove' and 'peek' operations are supported
  Memory is dynamically managed through an optionally user supplied 'resize' function.
  Elements in the queue have a user-define size but should kept as small as possible. This is only intended to associate lightweight data such as
  a key and a pointer, storing the key in the element can save dereferencing cost and thus improve cache locality. It must be noted
  that elements in the queue get moved in memory, so referencing them is invalid.

  There are API's (yet) to change the priority of an arbitary element or remove any but the topmost element. The idea is to let expired elements
  sink to the top and detect that and then remove them.
*/


#include <stdlib.h>

typedef struct priqueue_struct priqueue;
typedef priqueue* PriQueue;

/* function to compare 2 keys, mandatory */
typedef int (*priqueue_cmp_fn)(void*, void*);

/* function to copy elements, optional. Has the same prototype as memcpy which is used by default */
typedef void *(*priqueue_copy_fn)(void *dest, const void *src, size_t n);

/* called when used hits the high or low water marks and initially by priqueue_init() (with queue==NULL)
   or at priqueue_destroy (with queue != NULL, used elements == 0),
   optional.

   Must be aware of resizes by more than just incrementing the queue by one
 */
typedef PriQueue (*priqueue_resize_fn) (PriQueue);


/*
  this structure is not opaque to make it possible to implement a low level resize operation which has to reallocate
  the queue and update the high and low water marks.
 */
struct priqueue_struct
{
  void* queue;
  size_t element_size;
  unsigned used;
  unsigned high_water;                          // elements in the queue
  unsigned low_water;                           // size for shrinking the queue

  priqueue_cmp_fn cmpfn;
  priqueue_copy_fn copyfn;

  priqueue_resize_fn resizefn;
};






PriQueue
priqueue_init (PriQueue self,
               size_t element_size,
               priqueue_cmp_fn cmpfn,
               priqueue_copy_fn copyfn,
               priqueue_resize_fn resizefn);



PriQueue
priqueue_destroy (PriQueue self);


/*
  calls resize to match for at least 'elements' in the queue and then sets low_water to 0, disabling shrinking
  note that on overflow resize will re-enable low_water if it is not aware of this
*/
PriQueue
priqueue_reserve (PriQueue self, unsigned elements);


/*
  supplied/default resize function based on realloc
  initially allocates an array for 64 elements,
  doubles this when the high water mark is hit,
  shrinks at high_water/8-8 (that is, 64 is the minimum size)
*/
PriQueue
priqueue_clib_resize (PriQueue self);




/*
  insert a new element into the priority queue
  the element will be copied
  returns NULL on error
*/
PriQueue
priqueue_insert (PriQueue self, void* element);


/*
  returns a pointer to the topmost element
  note that this pointer is only valid as long no insert or remove is called
  returns NULL when the queue is empty
*/
static inline void*
priqueue_peek (PriQueue self)
{
  if (self && self->queue)
    return self->queue;

  return NULL;
}


/*
  removes the topmost element
  returns NULL on error (queue emtpy, resize failure)
 */
PriQueue
priqueue_remove (PriQueue self);



#endif

