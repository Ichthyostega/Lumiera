/*
  IterCursor(Test)  -  verify operation of a iterator based navigation cursor

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-cursor-test.cpp
 ** unit test \ref IterCursor_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-util.hpp"
#include "lib/iter-cursor.hpp"
#include "lib/util.hpp"

#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using util::join;
  using util::isnil;
  using std::vector;
  
  using LERR_(ITER_EXHAUST);
  
  
  namespace { // test fixture
    
    const uint NUM_ELMS = 10;
    
    using Numz = vector<uint>;
    using Iter = IterCursor<Numz::iterator>;
    using CIter = IterCursor<Numz::const_iterator>;
    
    inline Numz
    makeNumz()
    {
      Numz numz;
      for (uint i=0; i<NUM_ELMS; ++i)
        numz.push_back(i);
      return numz;
    }
    
  } // (END)fixture
  
  
  
  
  
  
  
  /*****************************************************************//**
   * @test demonstrate and cover the properties of IterCursor.
   * This wrapper allows to change between iterating forward and backwards.
   *
   * @see iter-cursor.hpp
   * @see iter-adapter.hpp
   * @see [usage example](\ref event-log.hpp)
   */
  class IterCursor_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleIteration();
          switchDirection();
          iterTypeVariations();
        }
      
      
      /** @test just iterate in various ways. */
      void
      simpleIteration ()
        {
          Numz numz{makeNumz()};
          Iter i1{numz.begin(), numz.end()};
          
          CHECK (!isnil(i1));
          CHECK (0 == *i1);
          ++++++i1;
          CHECK (3 == *i1);
          for (uint i=*i1 ; i1; ++i1, ++i)
            CHECK (i == *i1);
          
          CHECK (isnil(i1));
          
          Iter i2{numz};
          uint sum =0;
          while (++i2)
            sum += *i2;
          uint n = numz.size() - 1;
          CHECK (sum == n*(n+1) / 2);
          
          Iter i3{numz};
          for (auto & i : i3)
            ++i; // note: manipulate the contents...
          
          Iter i4{numz};
          CHECK (join(i4,"-+-") == "1-+-2-+-3-+-4-+-5-+-6-+-7-+-8-+-9-+-10");
          verifyComparisons (i4);
        }
      
      
      
      /** @test verify the ability of IterCursor to switch
       * the direction of the iteration. This "gear switch" can be done
       * any time, while in the middle of iteration, and even after
       * iteration end. That means, even an exhausted iterator can be
       * "turned back". This does not work on a default constructed
       * IterCursor, though.
       */
      void
      switchDirection ()
        {
          Numz numz{makeNumz()};
          Iter iter{numz};
          
          CHECK (0 == *iter);
          ++++++++iter;
          CHECK (4 == *iter);
          CHECK (!isnil(iter));
          CHECK (join(iter) == "4, 5, 6, 7, 8, 9");
          verifyComparisons (iter);
          
          iter.switchDir();
          CHECK (4 == *iter);
          CHECK (!isnil(iter));
          ++iter;
          CHECK (3 == *iter);
          CHECK (!isnil(iter));
          CHECK (join(iter) == "3, 2, 1, 0");
          verifyComparisons (iter);
          
          ++iter;
          CHECK (2 == *iter);
          ++++iter;
          CHECK (0 == *iter);
          CHECK (!isnil(iter));
          
          iter.switchDir();
          CHECK (0 == *iter);
          CHECK (!isnil(iter));
          ++iter;
          CHECK (1 == *iter);
          
          iter.switchDir();
          ++iter;
          CHECK (0 == *iter);
          CHECK (!isnil(iter));
          
          ++iter;
          CHECK (isnil(iter));
          VERIFY_ERROR (ITER_EXHAUST, *iter);
          VERIFY_ERROR (ITER_EXHAUST, ++iter);
          
          iter.switchDir();
          CHECK (!isnil(iter));
          CHECK (0 == *iter);
          
          while (++iter);
          CHECK (isnil(iter));
          VERIFY_ERROR (ITER_EXHAUST, *iter);
          VERIFY_ERROR (ITER_EXHAUST, ++iter);
          
          iter.switchDir();
          CHECK (!isnil(iter));
          CHECK (9 == *iter);
          
          Iter nil;
          CHECK (isnil (nil));
          iter.switchDir();
          CHECK (isnil (nil));
          VERIFY_ERROR (ITER_EXHAUST, *nil);
          VERIFY_ERROR (ITER_EXHAUST, ++nil);
        }
      
      
      
      /** @test verify variant created from a const_iterator,
       *  based on the const-ness of the underlying STL iterator
       */
      void
      iterTypeVariations ()
        {
          Numz numz{makeNumz()};
          Numz const& const_numz{numz};
          
          uint i = 0;
          for (Iter iter{numz};
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK (iter != Iter());
              CHECK (*iter == i);
              --(*iter);
              CHECK (*iter == i-1);
            }
          
          i = 0;
          for (CIter iter{const_numz};
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK (iter != CIter());
              CHECK (*iter == i-1);
              
              // note: the previous run indeed modified
              // the elements within the container.
              
            // ++(*iter);   // doesn't compile, because it yields a "* const"
            }
          
          verifyComparisons (CIter{numz});
        }
      
      
      
      
      
      
      /** @test verify equality handling and NIL detection
       *        for the given iterator/wrapper handed in.
       *  @note the argument is not altered; rather we create
       *        several copies, to iterate and compare those
       */
      template<class IT>
      void
      verifyComparisons (IT const& ii)
        {
          IT i1(ii);
          IT i2(ii);
          IT iN;
          CHECK ( isnil (iN));
          CHECK (!isnil (i1));
          CHECK (!isnil (i2));
          
          CHECK (i1 == i2); CHECK (i2 == i1);
          CHECK (i1 != iN); CHECK (iN != i1);
          CHECK (i2 != iN); CHECK (iN != i2);
          
          ++i1;
          CHECK (i1 != i2);
          CHECK (i1 != iN);
          
          ++i2;
          CHECK (i1 == i2);
          CHECK (i1 != iN);
          CHECK (i2 != iN);
          
          while (++i1) { }
          CHECK (isnil(i1));
          CHECK (i1 != i2);
          CHECK (i1 == iN);
          
          while (++i2) { }
          CHECK (isnil(i2));
          CHECK (i2 == i1);
          CHECK (i2 == iN);
        }
    };
  
  LAUNCHER (IterCursor_test, "unit common");
  
  
}} // namespace lib::test

