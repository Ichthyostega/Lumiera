/*
  FunInline(Test)  -  verify implementation of a functor with local delegate

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file fun-inline-test.cpp
 ** unit test \ref FunInline_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "test/tracking-dummy.hpp"
#include "lib/meta/fun-inline.hpp"
#include "test/diagnostic-output.hpp"///////////TODO jaleck

using test::showType;
using test::Dummy;


namespace lib  {
namespace meta {
namespace test {
  
  namespace { // test fixture...
    
    class TrackingFun
      : public Dummy
      {
      public:
        using Dummy::Dummy;
            
        long
        operator() (int val)
          {
            return Dummy::calc(val);
          }
      };
    
  }//(End) test fixture
  
  

  
  
  
  
  /*************************************************************//**
   * @test Documentation of the defining structure for a functor
   *       that manages a delegate within an opaque inline buffer.
   *       - setup of the invocation delegation
   * @remark this design complements std::function, insofar the
   *       wrapped functor implementation is stored within the
   *       FunInline itself, never using any heap memory.
   * @todo 4/2026 unfinished design draft.
   * @see FunctionSignature_test
   */
  class FunInline_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleUsage();
          design_Invoker();
          design_BufferHandling();
        }
      

	  /** @test example usage of an functor wrapper with local storage. */
      void
      simpleUsage()
        {
          TODO ("show simple usage of the front-end builder");
        }
      
      
      /** @test document the design of the Invoker
       */
      void
      design_Invoker()
        {
          int tap{-1};
          auto funny = [&](int i){ return -(tap = i); };
          CHECK (-1 == tap);
          CHECK (-2 == funny(2));
          CHECK ( 2 == tap);
          
          using Sig = long(int);
          using FunIn = FunInline<Sig, sizeof(TrackingFun)>;
          using Invoker = FunIn::Invoker<decltype(funny)>;
          
          CHECK (showType<Invoker::Sig>() == "long (int)"_expect );
          CHECK (showType<Invoker::ISig>() == "long (void*, int&&)"_expect );
          
          // The Invoker(Builder) actually defines a static function with that signature...
          using InvoSig = _Fun<decltype(Invoker::dispatch)>::Sig;
          CHECK (showType<InvoSig>() == "long (void*, int&&)"_expect );
          
          // and, through magic machinations of the C++ platform,
          // this dispatcher function actually inlines the target functor...
          InvoSig* invoker = Invoker::dispatch;
          void* delegate = &funny;
          
          CHECK (-55 == (*invoker) (delegate, 55) );
          CHECK ( 55 == tap);
          
          // since this delegation through the invoker is an abstraction,
          // other functors with compatible signature can be invoked as well,
          // once a suitable Invoker was instantiated to generate dispatch code...
          TrackingFun dummy;
          invoker = FunIn::Invoker<TrackingFun>::dispatch;
          delegate = &dummy;
          
          CHECK ((*invoker) (delegate, -55) == dummy.getID() - 55 );
          // Remark: Dummy::calc(arg) ≔ val + arg
        }
      
      
      /** @test verify proper instance handling
       *        for the embedded functor delegate
       */
      void
      design_BufferHandling()
        {
          using Sig = long(int);
          using FunIn = FunInline<Sig, sizeof(TrackingFun)>;
          
          CHECK (0 == Dummy::checksum());
          {
            TrackingFun dummy;
            int id = dummy.getID();
            CHECK (id == Dummy::checksum());
            {
              FunIn inFun{dummy};
              CHECK (inFun);
              
              CHECK (2*id == Dummy::checksum());
              
              long res = inFun(42);
              CHECK (res == id+42);
            }
            CHECK (id == Dummy::checksum());
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunInline_test, "unit common");
  
  
}}}// namespace lib::meta::test
