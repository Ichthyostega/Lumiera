/*
  HASH-VALUE.hpp  -  collection of tools and definitions for working with hashes

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file hash-value.h
 ** Hash value types and utilities.
 ** This header defines the basic hash value types and provides some simple
 ** utilities to support working with hash values.
 **
 ** @todo 11/2024 : to ensure a strong and reproducible implementation of hash-chaining,
 **                 the implementation of LibBoost is used directly. This breaks portability.
 **                 ///////////////////////////////////////////////////////////////////////////////TICKET #722
 ** @see hash-combine.hpp
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

#include <climits>

namespace lib {
  
  /** a STL compatible hash value */
  typedef size_t HashVal;
  
  /** a Lumiera UID */
  typedef lumiera_uid* LUID;
  
  
  //////////////////////////////////////TICKET #722 : hash_combine utility extracted into separate header 11/2024
  ///
} // namespace lib
#endif /* C++ */
#endif /*LIB_HASH_UTIL_H*/
