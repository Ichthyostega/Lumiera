/*
  HASH-COMBINE.hpp  -  hash chaining function extracted from LibBoost

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

  ======================================================================
  NOTE: this header adapts implementation code from LibBoost 1.67

//  Copyright 2005-2014 Daniel James.
//  Distributed under the Boost Software License, Version 1.0.
//  (See http://www.boost.org/LICENSE_1_0.txt)
//
//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18.
//
//  This also contains public domain code from MurmurHash. From the MurmurHash header:
//
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
  ======================================================================
*/


/** @file hash-combine.h
 ** Hash combine function extracted from LibBoost 1.67
 ** Combine two hash values to form a composite depending on both.
 ** @todo 2024 the Lumiera project has yet to decide how to approach
 **       portability of hash values, and the related performance issues.
 **       This code was directly integrated into the code base to ensure
 **       a stable implementation and reproducible hash values.
 **       ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 uniform uses of hash values
 */



#ifndef LIB_HASH_COMBINE_H
#define LIB_HASH_COMBINE_H

#include "lib/hash-value.h"
#include "lib/integral.hpp"

#include <climits>

namespace lib {
namespace hash{
    
    /** meld the additional hash value into the given base hash value.
     *  This is the standard formula used by Lib-Boost to combine the hash values
     *  of parts into a composite, and is based on the [Murmur-2.64A] hash algorithm.
     * [Murmur-2.64A]: https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
     */
    inline void
    combine (size_t & combinedHash, size_t additionalHash)
    {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 : Decide what stance to take towards portability
      combinedHash ^= additionalHash
                    + 0x9e3779b9
                    + (combinedHash<<6)
                    + (combinedHash>>2);
    }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 : (End) weak but portable fall-back code
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 : Using the stronger Boost-impl for 64bit platforms
   //// see: Boost 1.67 <include>/boost/container_hash/hash.hpp
  ///
 /*
// Don't define 64-bit hash combine on platforms without 64 bit integers,
// and also not for 32-bit gcc as it warns about the 64-bit constant.
#if !defined(BOOST_NO_INT64_T) && \
        !(defined(__GNUC__) && ULONG_MAX == 0xffffffff)

        inline void hash_combine_impl(boost::uint64_t& h,
                boost::uint64_t k)
        {
*/
      static_assert (sizeof (void*) * CHAR_BIT == 64, "TODO 2024 : decide what to do about portability");
      static_assert (sizeof (size_t) == sizeof(uint64_t));
      
      uint64_t& h = combinedHash;
      uint64_t  k = additionalHash;
      
            const uint64_t m{0xc6a4a7935bd1e995};
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

       //////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #722 : (End) Code extracted from Boost 1.67
      //
     //
    //
}} // namespace lib::hash
#endif /*LIB_HASH_COMBINE_H*/
