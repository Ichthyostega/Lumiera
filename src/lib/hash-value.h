/*
  HASH-VALUE.hpp  -  collection of tools and definitions for working with hashes

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file hash-value.h
 ** Hash value types and utilities.
 ** This header defines the basic hash value types and provides some simple
 ** utilities to support working with hash values.
 **
 ** @see HashIndexed
 **
 */



#ifndef LIB_HASH_VALUE_H
#define LIB_HASH_VALUE_H

#include <stdlib.h>


/** 
 * storage for a Lumiera unique ID,
 * based on a 128bit random number
 */
typedef unsigned char lumiera_uid[16];
typedef lumiera_uid* LumieraUid;



#ifdef __cplusplus  /* =========== C++ definitions ====================== */


namespace lib {
  
  /** a STL compatible hash value */
  typedef size_t HashVal;
  
  /** a Lumiera UID */
  typedef lumiera_uid* LUID;
  
  
  
  namespace hash {
    
    /** meld the additional hash value into the given
     *  base hash value. This is the standard formula
     *  used by the STL and Boost to combine the
     *  hash values of parts into a composite.
     */
    inline void
    combine (size_t & combinedHash, size_t additionalHash)
    {
      combinedHash ^= additionalHash
                    + 0x9e3779b9
                    + (combinedHash<<6)
                    + (combinedHash>>2);
    }
    
    // WIP more utils to come here....
  }
  
  
  
} // namespace lib
#endif /* C++ */
#endif /*LIB_HASH_UTIL_H*/
