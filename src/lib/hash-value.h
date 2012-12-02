/*
  HASH-UTIL.hpp  -  collection of tools and definitions for working with hashes

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


/** @file hash-util.h
 ** Hash value types and utilities.
 ** This header defines the basic hash value types and provides some simple
 ** utilities to support working with hash valuesWhile the actual storage is assumed to be based on a POD, the type info is crucial
 ** to circumvent the problems with an "object" base class. Frequently, the need to
 ** manage some objects in a central facility drives us to rely on a common base class,
 ** irrespective of an actual common denominator in the semantics of the objects to
 ** be managed within this collection. Typically this results in this common base class
 ** being almost worthless as an API or interface, causing lots of type casts when using
 ** such a common object management facility. Passing additional context or API information
 ** on a metaprogramming level through the management interface helps avoiding these
 ** shortcomings. 
 ** 
 ** Here we build an ID facility with the following properties:
 ** - based on a configurable storage/implementation of the actual hash or index code.
 ** - tied to a specific hierarchy of objects (template parameter "BA")
 ** - providing an additional template parameter to pass the desired type info
 ** - establishing an type hierarchy relation between ID related to the base class
 **   and the IDs denoting specific subclasses, such that the latter can stand-in
 **   for the generic ID.
 ** - providing a Mixin, which allows any hierarchy to use this facility without 
 **   much code duplication, including an adapter for tr1::unordered_map
 ** - equality comparison
 **
 ** @see HashIndexed_test
 ** @see Placement usage example
 **
 */



#ifndef LIB_HASH_UTIL_H
#define LIB_HASH_UTIL_H


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
    // WIP more utils to come here....
  }
  
  
  
} // namespace lib
#endif /* C++ */
#endif /*LIB_HASH_UTIL_H*/
