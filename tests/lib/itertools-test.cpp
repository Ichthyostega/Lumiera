/*
  IterTools(Test)  -  building combined and filtering iterators based on the Iterator tools

   Copyright (C)
     2009,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file itertools-test.cpp
 ** unit test \ref IterTools_test
 */


#include "test/run.hpp"
#include "lib/itertools.hpp"
#include "test/test-coll.hpp"
#include "test/test-helper.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"


using test::Test;
using util::isnil;
using util::join;

using std::vector;
using test::VecI;
using test::getTestSeq_int;
using test::showType;

namespace lib {
namespace test{
  
  using LERR_(ITER_EXHAUST);
  
  
  namespace { // a test iterator
    
    const uint NUM_ELMS{10};
    const auto TEST_NUMZ = getTestSeq_int<VecI> (NUM_ELMS);
    
    inline auto
    iterNum()
      {
        return RangeIter{TEST_NUMZ.begin(), TEST_NUMZ.end()};
      }
    
  } // (END) Test data
  
  
  
  
  
  
  
  /***************************************************************************//**
   *  @test build simple filter and transforming iterators using the »itertools«.
   *        Check correct behaviour of the resulting iterators and
   *        verify they behave like a **Lumiera Forward Iterator**.
   */
  class IterTools_test : public Test
    {
      
      using Iter = decltype(iterNum());
      
      
      virtual void
      run (Arg)
        {
          verify_testSetup();

          verify_singleIter();
          verify_filterIter();
          verify_filterRepeat();
          verify_transformIter();
          verify_pullLast();
        }
      
      
      /** @test fundamental iterator behaviour. */
      void
      verify_testSetup()
        {
          Iter ii = iterNum();
          
          CHECK (ii);
          CHECK (not isnil(ii));
          CHECK (0 == *ii);
          ++++ii;
          CHECK (2 == *ii);
          
          verifyComparisons(ii);
          CHECK (join (ii) == "2, 3, 4, 5, 6, 7, 8, 9"_expect );
          CHECK (join (ii, "-") == "2-3-4-5-6-7-8-9"_expect );
          CHECK (ii);
          CHECK (2 == *ii);
          
          while (ii)
            ++ii;
          CHECK (not ii);
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii);
        }
      
      
      /** @test verify equality handling and NIL detection
       *        for the given iterator/wrapper handed in */
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
      
      
      /** @test wrap an arbitrary single element as pseudo-iterator */
      void
      verify_singleIter()
        {
          uint i{12};
          
          auto i1 = singletonIter (12);
          auto i2 = singletonIter ( i);
          auto i3 = singletonIter (&i);
          
          CHECK (not isnil(i1));
          CHECK (not isnil(i2));
          CHECK (not isnil(i3));
          CHECK (12 == *i1);
          CHECK (12 == *i2);
          CHECK (12 == **i3);
          
          i = 23;
          CHECK (12 == *i1);
          CHECK (23 == *i2);
          CHECK (23 == **i3);
          
          ++i1;
          ++i2;
          ++i3;
          CHECK (isnil(i1));
          CHECK (isnil(i2));
          CHECK (isnil(i3));
          VERIFY_ERROR (ITER_EXHAUST, *i1 );
          VERIFY_ERROR (ITER_EXHAUST, *i2 );
          VERIFY_ERROR (ITER_EXHAUST, *i3 );
          
          // assignable as any iterator...
          auto i0 = nilIter<int>();
          i0 = singletonIter (13);
          CHECK (13 == *i0);
          CHECK (isnil (i1));
          std::swap (i0,i1);
          CHECK (isnil (i0));
          CHECK (13 == *i1);
          
          i1 = iter::Single<int>{};
          CHECK (isnil(i1));
        }
      
      
      static bool takeAll (int)   { return true; }
      static bool takeOdd (int i) { return 0 != i % 2; }
      static bool takeEve (int i) { return 0 == i % 2; }
      static bool takeTrd (int i) { return 0 == i % 3; }
      
      /** @test build a filter pipeline based on a predicate */
      void
      verify_filterIter()
        {
          Iter ii = iterNum();
          
          CHECK (join (filterIter (ii, takeAll)) == "0, 1, 2, 3, 4, 5, 6, 7, 8, 9"_expect );
          CHECK (join (filterIter (ii, takeEve)) == "0, 2, 4, 6, 8"_expect );
          CHECK (join (filterIter (ii, takeOdd)) == "1, 3, 5, 7, 9"_expect );
          
          auto all = filterIter (ii, takeAll);
          auto odd = filterIter (ii, takeOdd);
          verifyComparisons (all);
          verifyComparisons (odd);
          
          while (++all and ++odd)
            CHECK (all != odd);
          
          while (++all) { }
          CHECK (isnil (odd));
          CHECK (all == odd);
        }
      
      
      /** @test verify the helper to filter duplicate elements
       *        emitted by an source iterator. This test creates
       *        a sequence of numbers with random repetitions.
       */
      void
      verify_filterRepeat()
        {
          vector<uint> numberz;
          for (uint i=0; i<NUM_ELMS; ++i)
            {
              uint n = 1 + rani (100);
              do numberz.push_back(i);
              while (--n);
            }
          CHECK (NUM_ELMS < numberz.size(), "no repetition in test data??");
          
          auto completeSequence = iter_stl::eachElm (numberz);
          auto filtered = filterRepeat (completeSequence);
          
          uint num=0;
          for (; num < NUM_ELMS and not isnil(filtered);
               ++num,
               ++filtered
              )
            CHECK (num == *filtered);
          
          CHECK (num == NUM_ELMS and isnil(filtered));
        }
      
      
      
      static uint addTwo (int i) { return i+2; }
      static int  negate (int i) { return -i; }
      static int  idFunc (int i) { return i; }
      
      /** @test build a pipeline that maps a function onto the iterator results */
      void
      verify_transformIter()
        {
          Iter ii = iterNum();
          
          CHECK (join (transformIter (ii, idFunc)) == "0, 1, 2, 3, 4, 5, 6, 7, 8, 9"_expect );
          CHECK (join (transformIter (ii, negate)) == "0, -1, -2, -3, -4, -5, -6, -7, -8, -9"_expect );
          CHECK (join (transformIter (ii, addTwo)) == "2, 3, 4, 5, 6, 7, 8, 9, 10, 11"_expect );
          
          // note: output type changed to unsigned
          using TransIT = decltype(transformIter (ii, addTwo));
          CHECK (showType<TransIT::value_type>() == "uint"_expect);
          
          auto idi = transformIter (ii, idFunc);
          auto neg = transformIter (ii, negate);
          verifyComparisons (idi);
          verifyComparisons (neg);
          
          CHECK (idi == neg);  // Note twist: same type, different functor, yet same result (≡0)
          CHECK (++idi);
          CHECK (++neg);
          for ( ; idi and neg;
              ++idi,
              ++neg)
            {
              CHECK ( idi !=  neg);
              CHECK (*idi != *neg);
            }
          
          CHECK (!idi and !neg);
          CHECK (idi == neg);
        }
      
      
      
      /** @test helper to pull an iterator until its last element */
      void
      verify_pullLast()
        {
          Iter ii = iterNum();
          Iter::value_type lastElm = pull_last (ii);
          CHECK (NUM_ELMS-1 == lastElm);
          
          Iter emptyIterator;
          CHECK (isnil (emptyIterator));
          
          VERIFY_ERROR (ITER_EXHAUST, pull_last(emptyIterator) );
        }
    };
  
  LAUNCHER (IterTools_test, "unit common");
  
  
}} // namespace lib::test
