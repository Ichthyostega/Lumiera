/*
  UtilCollection(Test)  -  helpers and shortcuts for working with collections

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file util-collection-test.cpp
 ** unit test \ref UtilCollection_test
 */


#include "lib/test/run.hpp"
#include "lib/itertools.hpp"
#include "lib/util-coll.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/meta/trait.hpp"


#include <boost/lexical_cast.hpp>
#include <vector>


using ::Test;

using util::first;
using util::last;

using lib::meta::can_STL_ForEach;
using lib::meta::can_IterForEach;
using lib::meta::can_STL_backIteration;

using boost::lexical_cast;



namespace util {
namespace test {
  
  typedef std::vector<uint> VecI;
  typedef lib::RangeIter<VecI::iterator> RangeI;
  
  
  
  namespace{ // Test data and operations
    
    VecI
    someNumberz (uint count)
    {
      VecI numbers;
      numbers.reserve(count);
      while (count)
        numbers.push_back(count--);
      
      return numbers;
    }
    
  } // (End) test data and operations
  
  
  
  /*****************************************************************//**
   * @test verify some convenience shortcuts and helpers dealing
   * with Collections and sequences (Iterators).
   * - metafunctions to distinguish STL containers and Lumiera Iterators
   * - get the first element
   * - get the last element
   */
  class UtilCollection_test : public Test
    {
      virtual void
      run (Arg arg)
        {
          verify_typeDetectors();
          
          uint NUM_ELMS = firstVal (arg, 20);
          VecI container = someNumberz (NUM_ELMS);
          RangeI iterator(container.begin(), container.end());
          
          verify_accessFirstLast (container, NUM_ELMS);
          verify_accessFirstLast (iterator, NUM_ELMS);
          
          verify_Min_Max (container, NUM_ELMS);
          verify_Min_Max (iterator, NUM_ELMS);
        }
      
      
      template<class COL>
      void
      verify_accessFirstLast (COL const& col, uint lim)
        {
          uint theFirst = lim;
          uint theLast  = 1;
          
          CHECK (first(col) == theFirst);
          CHECK (last(col) == theLast);
        }
      
      
      template<class COL>
      void
      verify_Min_Max (COL const& col, uint lim)
        {
          uint expectedMax = lim;
          uint expectedMin = 1;
          
          CHECK (max (col) == expectedMax);
          CHECK (min (col) == expectedMin);
          
          COL empty;
          
          using Val = typename COL::value_type;
          
          CHECK (max (empty) == std::numeric_limits<Val>::min());
          CHECK (min (empty) == std::numeric_limits<Val>::max());
        }
      
      
      void
      verify_typeDetectors()
        {
          CHECK ( can_STL_ForEach<VecI>::value);
          CHECK ( can_STL_backIteration<VecI>::value);
          
          CHECK (!can_STL_ForEach<RangeI>::value);
          CHECK (!can_STL_backIteration<RangeI>::value);
          
          CHECK (!can_IterForEach<VecI>::value);
          CHECK ( can_IterForEach<RangeI>::value);
        }
    };
  
  
  
  
  LAUNCHER (UtilCollection_test, "unit common");
  
  
}} // namespace util::test

