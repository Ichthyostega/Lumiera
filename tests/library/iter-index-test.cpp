/*
  IterIndex(Test)  -  verify index access packaged as iterator handle

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-index-test.cpp
 ** unit test \ref IterIndex_test
 */



#include "lib/test/run.hpp"
#include "lib/iter-index.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <vector>
#include <memory>



namespace lib {
namespace test{
  
  using ::Test;
  using util::join;
  using util::isnil;
  using std::vector;
  using std::shared_ptr;
  using std::make_shared;
  
  using LERR_(ITER_EXHAUST);
  using LERR_(INDEX_BOUNDS);
  
  
  namespace { // test fixture
    
    const uint NUM_ELMS = 10;
    
    using Numz = vector<uint>;
    using Iter = IterIndex<Numz>;
    using CIter = IterIndex<const Numz>;
    using SMIter = IterIndex<Numz, shared_ptr<Numz>>;
    
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
  class IterIndex_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleIteration();
          verify_randomAccess();
          iterTypeVariations();
        }
      
      
      /** @test just iterate in various ways. */
      void
      simpleIteration ()
        {
          Numz numz{makeNumz()};
          Iter i1{numz};
          
          CHECK (not isnil(i1));
          CHECK (0 == *i1);
          ++++++i1;
          CHECK (3 == *i1);
          for (uint i=*i1 ; i1; ++i1, ++i)
            CHECK (i == *i1);
          
          CHECK (isnil(i1));
          
          auto sum = explore(Iter{numz}).resultSum();
          uint n = numz.size() - 1;
          CHECK (sum == n*(n+1)/2);
          
          for (auto & i : Iter{numz})
            ++i; // note: manipulate the contents...
          CHECK (join(numz,"◇") == "1◇2◇3◇4◇5◇6◇7◇8◇9◇10"_expect);
          
          verifyComparisons (Iter{numz});
        }
      
      
      
      /** @test verify the ability of IterIndex to access and manipulate
       * the current index position, which can be done any time, while in
       * the middle of iteration, and even after iteration end. That means,
       * even an exhausted iterator can be „reanimated“. This manipulation
       * is not allowed on a default constructed IterIndex, though.
       */
      void
      verify_randomAccess ()
        {
          Numz numz{makeNumz()};
          Iter iter{numz};
          
          CHECK (0 == *iter);
          ++++++++iter;
          CHECK (4 == *iter);
          CHECK (not isnil(iter));
          CHECK (join(iter) == "4, 5, 6, 7, 8, 9"_expect);
          verifyComparisons (iter);
          
          CHECK (4 == *iter);
          CHECK (4 == iter.getIDX());
          iter.setIDX(7);
          CHECK (7 == iter.getIDX());
          CHECK (not isnil(iter));
          CHECK (7 == *iter);
          ++iter;
          CHECK (8 == *iter);
          iter.setIDX(6);
          CHECK (join(iter) == "6, 7, 8, 9"_expect);
          verifyComparisons (iter);

          ++++++++iter;
          CHECK (isnil(iter));
          VERIFY_ERROR (ITER_EXHAUST, *iter);
          VERIFY_ERROR (ITER_EXHAUST, ++iter);
          VERIFY_ERROR (ITER_EXHAUST, iter.getIDX());
          
          iter.setIDX(9);
          CHECK (not isnil(iter));
          CHECK (9 == *iter);
          
          VERIFY_ERROR (INDEX_BOUNDS, iter.setIDX(10));
          CHECK (9 == iter.getIDX());
          VERIFY_ERROR (INDEX_BOUNDS, iter.setIDX(-1));
          CHECK (9 == iter.getIDX());
          
          Iter empty;
          CHECK (isnil (empty));
          VERIFY_ERROR (INDEX_BOUNDS, empty.setIDX(0));
          VERIFY_ERROR (ITER_EXHAUST, empty.getIDX());
        }
      
      
      
      /** @test verify possible variations of this iterator template,
       *  using a const container or maintaining shared ownership.
       */
      void
      iterTypeVariations ()
        {
          auto smartNumz = make_shared<Numz> (makeNumz());
          Numz      & numz{*smartNumz};
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
                           // Note: the preceding loop has indeed modified the contents
//            ++(*iter);  //  but this doesn't compile, because the CIter yields a _const_
            }
          
          verifyComparisons (CIter{numz});
          
          CHECK (1 == smartNumz.use_count());
          {
            SMIter smIter{smartNumz};
            CIter cIter{*smartNumz};
            CHECK (*cIter == uint(-1));
            for (i=0; smIter; ++smIter, ++i)
              {
                CHECK (smIter);
                CHECK (smIter != SMIter());
                CHECK (*smIter == i-1);
                ++(*smIter);
                CHECK (*smIter == i);
              }
            CHECK (isnil (smIter));
            CHECK (smIter == SMIter());
            cIter.setIDX(5);
            smIter.setIDX(5);
            CHECK (*smIter == *cIter);
            
            verifyComparisons (smIter);
            
            CHECK (5 == *cIter);   // shared data modified
            CHECK (2 == smartNumz.use_count());
          }
          CHECK (1 == smartNumz.use_count());
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
  
  LAUNCHER (IterIndex_test, "unit common");
  
  
}} // namespace lib::test

