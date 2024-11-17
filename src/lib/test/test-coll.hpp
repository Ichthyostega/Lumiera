/*
  TEST-COLL.hpp  -  containers and collections with test data

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file test-coll.hpp
 ** some bits of unit test helper code to fabricate collections with test data
 */


#ifndef LIB_TEST_TEST_COLL_H
#define LIB_TEST_TEST_COLL_H


#include <unordered_map>
#include <iostream>
#include <vector>
#include <map>


namespace lib {
namespace test{
  
  
  typedef std::vector<int> VecI;
  
  template<class VEC>
  inline VEC
  getTestSeq_int (const uint NUM_ELMS)
  {
    VEC vec;
    for (uint i=0; i<NUM_ELMS; ++i)
      vec.push_back (i);
    
    return vec;
  }
  

  typedef std::map<int,int> MapII;
  typedef std::multimap<int,int> MMapII;
  
  typedef std::unordered_map<int,int> HMapII;
  typedef std::unordered_multimap<int,int> HMMapII;
  
  
  template<class MAP>
  inline MAP
  getTestMap_int (const uint NUM_ELMS)
  {
    MAP map;
    for (uint i=0; i<NUM_ELMS; ++i)
      map[i] = 2*i;
    
    return map;
  }
  
  template<class MUMAP>
  inline MUMAP
  getTestMultiMap_int (const uint NUM_ELMS)
  {
    MUMAP map;
    for (uint i=0; i<NUM_ELMS; ++i)
      for (uint j=NUM_ELMS-i; j; --j)
        map.insert (std::make_pair (i, j));
    
    return map;
  }
  
  
        
  template<class ITER>
  inline void
  pullOut (ITER const& i)
  {
    for (ITER ii(i); ii ; ++ii )
      std::cout << "::" << *ii;
  }

  
  
}} // namespace lib::test
#endif /*LIB_TEST_TEST_COLL_H*/
