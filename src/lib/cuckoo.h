/*
    A cuckoo hash implementation

  Copyright (C)
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef CUCKOO_H
#define CUCKOO_H

/**
 * @file
 * Cuckoo hashing
 * This hashing gives guaranteed O(1) lookup complexity and amortized O(1) insert and remove complexity.
 * Hash tables by default grow and shrink automatically. It is posible to preallocate entries and turn
 * automatic shrinking off taking out the memory management factors for insert and remove operations.
 * This implementation uses 3 Tables which exponential growing sizes.
 */

#include <stdlib.h>
#include <stdint.h>

struct cuckoo_struct;
typedef struct cuckoo_struct * Cuckoo;

/**
 * Hash function.
 * User supplied universal hash function
 * @param item address of the item to be hashed
 * @param r pseudo random number, 31 significant bits, regenerated on each rehash
 * @return hash value
 */
typedef size_t (*cuckoo_hashfunc)(const void* item, const uint32_t r);

/**
 * Compare function.
 * User supplied compare function
 * @param item1 address of the first item
 * @param item2 address of the second item
 * @return 1 when the items are identical, 0 otherwise
 */
typedef int (*cuckoo_cmpfunc)(const void* item1, const void* item2);

/**
 * Item destructor function.
 * User supplied destructor function. This function is called when items are removed
 * from the hash or at hash detroy/delete time. Must be safe to be called on a zeroed element.
 * @param item address of the item to be destroyed
 */
typedef void (*cuckoo_dtorfunc)(void* item);

/**
 * Initialize a cuckoo hash.
 * @param self pointer to a uninitialized cuckoo datastructure
 * @param h1 hash function for the first table
 * @param h2 hash function for the second table
 * @param h3 hash function for the third table
 * @param cmp function which compares two keys
 * @param startsize initial size of table t3, as 2's exponent
 * @param dtor a function used to clean up hash entries, might be NULL if nothing required
 * @return The initialized hashtable or NULL at allocation failure
 */
Cuckoo
cuckoo_init (Cuckoo self,
             cuckoo_hashfunc h1,
             cuckoo_hashfunc h2,
             cuckoo_hashfunc h3,
             cuckoo_cmpfunc cmp,
             size_t itemsize,
             unsigned startsize,
             cuckoo_dtorfunc dtor);

/**
 * Allocate a new cuckoo hash.
 * @param h1 hash function for the first table
 * @param h2 hash function for the second table
 * @param h3 hash function for the third table
 * @param cmp function which compares two keys
 * @param startsize initial size of table t3, as 2's exponent
 * @param dtor a function used to clean up hash entries, might be NULL if nothing required
 * @return The initialized hashtable or NULL at allocation failure
 */
Cuckoo
cuckoo_new (cuckoo_hashfunc h1,
            cuckoo_hashfunc h2,
            cuckoo_hashfunc h3,
            cuckoo_cmpfunc cmp,
            size_t itemsize,
            unsigned startsize,
            cuckoo_dtorfunc dtor);

/**
 * Destroy a cuckoo hash.
 * Frees internal used resources.
 * @param self cuckoo hash to destroy
 * @return The now uninitialized hashtable
 */
Cuckoo
cuckoo_destroy (Cuckoo self);

/**
 * Deallocate a cuckoo hash.
 * @param self handle of the hash table to be freed
 */
void
cuckoo_delete (Cuckoo self);

/**
 * Get the number of elements stored in a hash.
 * @return number of elements, 0 when empty
 */
size_t
cuckoo_nelements (Cuckoo self);

/**
 * Insert an element into a hash.
 * amortized O(1) complexity because it may rarely rehash the tables or even grow them.
 * see cuckoo_reserve() about how to preallocate entries to prevent the growing costs.
 * @param self handle to the hash table
 * @param item pointer to a item to be inserted
 * @return 1 on successful insert, 0 on allocation failure
 * Note: at failure there is one arbitary hash cell lost!
 */
int
cuckoo_insert (Cuckoo self, void* item);

/**
 * Find a value by key in a hash.
 * @param self handle to the hash table
 * @param item pointer to an item to be looked up
 * @return found object or NULL if not present
 */
void*
cuckoo_find (Cuckoo self, void* item);

/**
 * Remove a item from a hash.
 * amortized O(1) complexity when it automatically shrink its tables.
 * guaranteed O(1) complexity when automatic shrinking is turned off.
 * see cuckoo_reserve() about how turn automatic shrinking off.
 * @param self handle to the hash table
 * @param item pointer to the item to be removed
 */
void
cuckoo_remove (Cuckoo self, void* item);

/**
 * Shrink the hash sizes when possible and turn autocompacting on.
 * only useful when autmatic shrinking is turned off.
 * see cuckoo_reserve() about how turn shrinking off.
 * @param self handle to the hash table
 */
int
cuckoo_compact (Cuckoo self);

/**
 * Prereserve hash entries and turn autocompacting off.
 * @param self handle to the hash table
 * @param more number of entries to be additionally reserved
 * In rare circumstances inserting into a hash which has reserved some entries
 * it may still need to rehash (or even rarer) to grow the table.
 * While autocompacting is turned off, removing is completely O(1).
 */
int
cuckoo_reserve (Cuckoo self, size_t more);

#endif
/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
