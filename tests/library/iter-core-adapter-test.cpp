/*
  IterCoreAdapter(Test)  -  iterating over a »state core«

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-core-adapter-test.cpp
 ** unit test \ref IterCoreAdapter_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/test/diagnostic-output.hpp"////////////TODO

#include "lib/iter-adapter.hpp"

//#include <boost/lexical_cast.hpp>
//#include <vector>



namespace lib {
  using util::unConst;
namespace test{
  
  using LERR_(ITER_EXHAUST);
//  using boost::lexical_cast;
//  using util::for_each;
//  using util::isnil;
  using util::isSameObject;
//  using std::vector;
  
  
  namespace {
    
    /**
     * A »*State Core*« to step down numbers to zero.
     * @note this is a minimal description of a state progression towards a goal
     *     - default constructed is equivalent to _goal was reached_
     *     - can be copied, manipulated and compared
     *     - yields reference to internal state
     *     - no safety checks whatsoever
     */
    struct StepDown
      {
        uint n;
        
        StepDown(uint start =0)
          : n(start)
          { }
        
        bool
        checkPoint ()  const
          {
            return n != 0;
          }
        
        uint&
        yield ()  const
          {
            return util::unConst(n);
          }
        
        void
        iterNext ()
          {
            --n;
          }
        
        bool
        operator== (StepDown const& o)  const
          {
            return n == o.n;
          }
      };
  } // (END) impl test dummy container
  
  
  
  
  
  
  
  /*****************************************************************//**
   * @test cover the concept of a »state core«, which is used in Lumiera
   *       for various aspects of data generation and iteration.
   * @see IterStateWrapper
   * @see iter-explorer.hpp
   */
  class IterCoreAdapter_test : public Test
    {
      virtual void
      run (Arg)
        {
          simpleUsage();
          stateManipulation();
          checked_and_protected();
          value_and_reference_yield();
        }
      
      
      
      
      
      /** @test build a »Lumiera Forward Iterator«
       *   to transition a State-Core towards it final state
       */
      void
      simpleUsage()
        {
          auto it = IterableDecorator<int, StepDown>{3};
          CHECK (it);
          CHECK (*it == 3);
          ++it;
          CHECK (it);
          CHECK (*it == 2);
          ++it;
          CHECK (it);
          CHECK (*it == 1);
          ++it;
          CHECK (not it);
        }
      
      
      /** @test state of a decorated un-checked core can be manipulated */
      void
      stateManipulation()
        {
          auto it = IterableDecorator<int, StepDown>{};
          CHECK (not it);
          CHECK (*it == 0);
          ++it;
          CHECK (*it == uint(-1));
          CHECK (it);
          *it = 5;
          CHECK (*it == 5);
          ++it;
          CHECK (*it == 4);
          it.n = 1;
          CHECK (*it == 1);
          CHECK (it);
          ++it;
          CHECK (not it);
          CHECK (it.n == 0);
          CHECK (isSameObject(*it, it.n));
        }
      
      
      /** @test additional wrappers to add safety checks
       *        or to encapsulate the state core altogether */
      void
      checked_and_protected()
        {
          auto cc = CheckedCore<StepDown>{2};
          CHECK (cc.checkPoint());
          CHECK (cc.yield() == 2u);
          cc.n = 1;
          CHECK (cc.yield() == 1u);
          cc.iterNext();
          CHECK (not cc.checkPoint());
          CHECK (cc.n == 0u);
          VERIFY_ERROR (ITER_EXHAUST, cc.yield() );
          VERIFY_ERROR (ITER_EXHAUST, cc.iterNext() );
          
          auto it = IterStateWrapper<uint,StepDown>{StepDown{2}};
          CHECK (it);
          CHECK (*it == 2u);
          ++it;
          CHECK (*it == 1u);
          ++it;
          CHECK (not it);
          VERIFY_ERROR (ITER_EXHAUST, *it  );
          VERIFY_ERROR (ITER_EXHAUST, ++it );
        }
      
      
      
      /** @test adapters can (transparently) handle a core which yields values
       *      - demonstrate how cores can be augmented by decoration
       *      - the decorated core here yields by-value
       *      - both CheckedCore and IterableDecorator can cope with that
       *      - the result is then also delivered by-value from the iterator
       * @remark the »Lumiera Forward Iterator« concept does not exactly specify
       *   what to expect when dereferencing an iterator; yet for obvious reasons,
       *   most iterators in practice expose a reference to some underlying container
       *   or internal engine state, since this is more or less the whole point of using
       *   an iterator: we want to expose something for manipulation, without revealing
       *   what it actually is (even while in most cases the implementation is visible
       *   for the compiler, the code using the iterator is not tightly coupled). This
       *   scheme has ramifications for the way any iterator pipeline works; notably
       *   any _transformation_ will have to capture a function result. However,
       *   sometimes an iterator can only return a computed value; such a usage
       *   can be valid and acceptable and is supported to the degree possible.
       */
      void
      value_and_reference_yield ()
        {
          struct ValueStep
            : StepDown
            {
              using StepDown::StepDown;
              int yield()  const { return StepDown::yield(); }
            };
          
          auto it = IterableDecorator<int, CheckedCore<ValueStep>>{2};
          CHECK (it);
          CHECK (*it == 2);
          CHECK (it.n == 2);
          CHECK (not isSameObject(*it, it.n));
          CHECK (showType<decltype(*it)>() == "int"_expect);
          
          StepDown& ix{it};
          CHECK (ix.yield() == 2u);
          CHECK (    isSameObject(ix.yield(), ix.n));
          CHECK (    isSameObject(ix.yield(), it.n));
          CHECK (showType<decltype(ix.yield())>() == "uint&"_expect);
          
          ++it;
          CHECK (*it == 1);
          ++it;
          VERIFY_ERROR (ITER_EXHAUST, *it  );
          VERIFY_ERROR (ITER_EXHAUST, ++it );
        }
      
      
    };
  
  LAUNCHER (IterCoreAdapter_test, "unit common");
  
  
}} // namespace lib::test

