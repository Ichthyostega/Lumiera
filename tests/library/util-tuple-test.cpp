/*
  UtilTuple(Test)  -  helpers and shortcuts for working with tuples

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file util-tuple-test.cpp
 ** unit test \ref UtilTuple_test
 */


#include "lib/test/run.hpp"
#include "lib/util-tuple.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/util.hpp"

#include <vector>


using ::Test;
using util::isnil;
using std::tuple_size_v;


namespace util {
namespace test {
  
  using VecI   = std::vector<uint>;
  using RangeI = lib::RangeIter<VecI::iterator>;
  
  
  namespace{ // Test data and operations
    
    VecI
    someNumbz (uint count)
    {
      VecI numbers;
      numbers.reserve(count);
      while (count)
        numbers.push_back(count--);
      
      return numbers;
    }
    
  } // (End) test data and operations
  
  
  
  /*****************************************************************//**
   * @test verify some convenience shortcuts and helpers for interplay
   * of tuples and iterable sequences:.
   * - unpack a sequence into a tuple of references
   */
  class UtilTuple_test : public Test
    {
      
      void
      run (Arg)
        {
          verify_unpackIterator();
        }
      
      
      /**
       * @test unpack a sequence into a tuple of references,
       *       usable for structural binding.
       */
      void
      verify_unpackIterator()
        {
          VecI container = someNumbz (5);
          RangeI iterator(container.begin(), container.end());
          
          CHECK (not isnil (iterator));
          auto tup = seqTuple<5> (iterator);
          CHECK (    isnil (iterator));          // iterator was exhausted on unpacking...
          CHECK (5 == tuple_size_v<decltype(tup)>);
          
          auto& [g,f,e,d,c] = tup;
          CHECK (c == 1);
          CHECK (d == 2);
          CHECK (e == 3);
          CHECK (f == 4);
          CHECK (g == 5);
          
          g = 55;                       // we indeed got references...
          CHECK (55 == std::get<0> (tup));
          CHECK (55 == container.front());
        }
    };
  
  
  
  
  LAUNCHER (UtilTuple_test, "unit common");
  
  
}} // namespace util::test

