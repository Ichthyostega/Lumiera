/*
  PRIQUEUE.h  -  simple heap based priority queue

  Copyright (C)         Lumiera.org
    2011,               Christian Thaeter <ct@pipapo.org>

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

/** @file priqueue.h
 ** Simple priority queue implementation based on a binary heap.
 ** Only 'insert' 'remove' and 'peek' operations are supported.
 ** Memory is dynamically managed through an optionally user supplied 'resize' function.
 ** Elements in the queue have a user-define size but should kept as small as possible.
 ** This is only intended to associate lightweight data such as a key and a pointer,
 ** storing the key in the element can save dereferencing cost and thus improve
 ** cache locality.
 ** 
 ** @warning elements in the queue get moved in memory, so referencing them is not allowed.
 ** 
 ** @todo we might add operations to change the priority of an arbitrary element or remove
 **       any but the topmost element. The idea is to let expired elements sink to the top
 **       and just detect and remove them on next access.
 ** 
 ** @see vault::engine::SchedulerFrontend
 ** 
 */


#ifndef LIB_PRIQUEUE_H
#define LIB_PRIQUEUE_H


#include "lib/error.h"


typedef struct lumiera_priqueue_struct lumiera_priqueue;
typedef lumiera_priqueue* LumieraPriQueue;

/** function to compare 2 keys, mandatory */
typedef int (*lumiera_priqueue_cmp_fn)(void*, void*);

/** function to copy elements, optional.
 *  Has the same prototype as memcpy which is used by default */
typedef void *(*lumiera_priqueue_copy_fn)(void *dest, const void *src, size_t n);

/** called when used hits the high or low water marks and initially by priqueue_init() (with queue==NULL)
 *  or at priqueue_destroy (with queue != NULL, used elements == 0), optional.
 *  
 * @note must be aware of resizes by more than just incrementing the queue by one
 */
typedef LumieraPriQueue (*lumiera_priqueue_resize_fn) (LumieraPriQueue);


/**
 * @remarks this structure is not opaque to make it possible
 *          to implement a low level resize operation
 *          which has to reallocate the queue and update
 *          the high and low water marks.
 */
struct lumiera_priqueue_struct
{
  void* queue;
  size_t element_size;
  unsigned used;
  unsigned high_water;                          ///< elements in the queue
  unsigned low_water;                           ///< size for shrinking the queue

  lumiera_priqueue_cmp_fn cmpfn;
  lumiera_priqueue_copy_fn copyfn;

  lumiera_priqueue_resize_fn resizefn;
};






LumieraPriQueue
lumiera_priqueue_init (LumieraPriQueue self,
                       size_t element_size,
                       lumiera_priqueue_cmp_fn cmpfn,
                       lumiera_priqueue_copy_fn copyfn,
                       lumiera_priqueue_resize_fn resizefn);



LumieraPriQueue
lumiera_priqueue_destroy (LumieraPriQueue self);


/**
 * calls resize to match for at least 'elements' in the queue
 * and then sets low_water to 0, disabling shrinking
 * @note on overflow resize will re-enable low_water if it is not aware of this
 */
LumieraPriQueue
lumiera_priqueue_reserve (LumieraPriQueue self, unsigned elements);


/**
 * supplied/default resize function based on realloc
 * initially allocates an array for 64 elements,
 * doubles this when the high water mark is hit,
 * shrinks at high_water/8-8 (that is, 64 is the minimum size)
 */
LumieraPriQueue
lumiera_priqueue_clib_resize (LumieraPriQueue self);




/**
 * insert a new element into the priority queue
 * the element will be copied
 * @return \c NULL on error
 */
LumieraPriQueue
lumiera_priqueue_insert (LumieraPriQueue self, void* element);


/**
 * @return pointer to the topmost element, `NULL` on empty queue
 * @note returned pointer is only valid as long
 *       as no insert or remove is called
 */
static inline void*
lumiera_priqueue_peek (LumieraPriQueue self)
{
  if (self && self->queue)
    return self->queue;

  return NULL;
}


/**
 * removes the topmost element
 * @return \c NULL on error (empty queue, resize failure)
 */
LumieraPriQueue
lumiera_priqueue_remove (LumieraPriQueue self);



#endif/*LIB_PRIQUEUE_H*/
