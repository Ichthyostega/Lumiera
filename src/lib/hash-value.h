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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 : Boost uses a stronger impl here on 64bit platforms
    /// see: Boost 1.67 <include>/boost/container_has/hash.hpp
    ///
 /*
// Don't define 64-bit hash combine on platforms without 64 bit integers,
// and also not for 32-bit gcc as it warns about the 64-bit constant.
#if !defined(BOOST_NO_INT64_T) && \
        !(defined(__GNUC__) && ULONG_MAX == 0xffffffff)

        inline void hash_combine_impl(boost::uint64_t& h,
                boost::uint64_t k)
        {
            const boost::uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
            const int r = 47;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;

            // Completely arbitrary number, to prevent 0's
            // from hashing to 0.
            h += 0xe6546b64;
        }

#endif // BOOST_NO_INT64_T
      */
     //
    // WIP more utils to come here....
  }
  
  
  
} // namespace lib
#endif /* C++ */
#endif /*LIB_HASH_UTIL_H*/
