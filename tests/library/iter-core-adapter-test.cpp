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

#include "lib/iter-adapter.hpp"

#include <string>



namespace lib {
namespace test{
  
  using LERR_(ITER_EXHAUST);
  using util::isSameObject;
  using std::string;
  
  namespace { // Test fixture
    
    /**
     * A test »*State Core*« which steps down a number to zero.
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
  }// (END) test dummy
  
  
  
  
  
  
  
  /*****************************************************************//**
   * @test cover the concept of a »state core«, which is used in Lumiera
   *       for various aspects of data generation and iteration.
   * @see IterStateWrapper
   * @see iter-adapter.hpp
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
          verify_TypeReconciliation();
        }
      
      
      
      
      /** @test build a »Lumiera Forward Iterator«
       * to transition a State-Core towards it final state.
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
       *      - demonstrate how cores can be augmented by decoration...
       *      - the decorated core here yields by-value, not by-ref.
       *      - Both CheckedCore and IterableDecorator can cope with that
       *      - the result is then also delivered by-value from the iterator.
       * @remark the »Lumiera Forward Iterator« concept does not exactly specify
       *   what to expect when dereferencing an iterator; yet for obvious reasons,
       *   most iterators in practice expose a reference to some underlying container
       *   or internal engine state, since this is more or less the whole point of using
       *   an iterator: we want to expose something for manipulation, without revealing
       *   what it actually is (even while in most cases the implementation is visible
       *   for the compiler, the code using the iterator is not tightly coupled). This
       *   scheme has ramifications for the way any iterator pipeline works; notably
       *   any _transformation_ will have to capture a function result. However,
       *   sometimes an iterator can only return a computed value; such an usage
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
      
      
      /** @test construction of a common result type.
       *      - based on `std::common_type`
       *      - so there must be some common ground
       *      - if any of the types is by-value, the result is
       *      - if any of the types is const, the result is const
       */
      void
      verify_TypeReconciliation()
        {
          using C1 = Common<int,string>;
          CHECK (not C1());
          CHECK (not C1::value);
          
          using C2 = Common<int,long*>;
          CHECK (not C2());                                              // can not be reconciled
          CHECK (not C2::value);
//        using X = C2::ResType;                                         // does not (and should not) compile
          
          using C3 = Common<string,string>;
          CHECK (C3());
          CHECK (showType<C3::ResType>() == "string"_expect );
          
          using C4 = Common<string&,string>;
          CHECK (showType<C4::ResType>() == "string"_expect );
          
          using C5 = Common<string&,string&>;
          CHECK (showType<C5::ResType>() == "string&"_expect );          // ref access to both is possible
          
          using C6 = Common<string&,string&&>;
          CHECK (showType<C6::ResType>() == "string"_expect );           // caution, RValue might be a temporary
          
          using C7 = Common<string&&,string&&>;
          CHECK (showType<C7::ResType>() == "string"_expect );
          
          using C8 = Common<string const&, string const&>;
          CHECK (showType<C8::ResType>() == "string const&"_expect );
          
          using C9 = Common<string const&, string&>;
          CHECK (showType<C9::ResType>() == "string const&"_expect );    // reconcile to const&
          
          using C10 = Common<string const&, string>;
          CHECK (showType<C10::ResType>() == "const string"_expect );
          
          using C11 = Common<string&&, string const&>;
          CHECK (showType<C11::ResType>() == "const string"_expect );    // reconciled to value type
          
          using C12 = Common<long const&, int>;
          CHECK (showType<C12::ResType>() == "const long"_expect );      // reconciled to the larger number type
          
          using C13 = Common<double&, long const&>;
          CHECK (showType<C13::ResType>() == "double const&"_expect );   // usual in C++ (loss of precision possible)
        }
      
      template<typename T1, typename T2>
      using Common = meta::CommonResultYield<T1,T2>;
    };
  
  LAUNCHER (IterCoreAdapter_test, "unit common");
  
  
}} // namespace lib::test

