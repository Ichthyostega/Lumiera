/*
  NodeBase(Test)  -  unit test to cover the render node base elements

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-base-test.cpp
 ** Unit test \ref NodeBase_test covers elementary components of render nodes.
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/meta/function.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"
//#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////////TODO
#include "lib/util.hpp"


//using std::string;
using std::tuple;/////////////TODO
 using std::array;
using util::isSameAdr;


namespace steam {
namespace engine{
namespace test  {
  
  
  namespace { // Test fixture
    /**
     */
  }
  
  
  /***************************************************************//**
   * @test basic render node properties and behaviour.
   */
  class NodeBase_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          verify_TurnoutSystem();
          verify_FeedManifold();
          UNIMPLEMENTED ("build a simple render node and then activate it");
        }
      
      /** @test the TurnoutSystem as transient coordinator for node invocation
       */
      void
      verify_TurnoutSystem()
        {
          Time nomTime{rani(10'000),0};                    // drive test with a random »nominal Time« <10s with ms granularity
          TurnoutSystem invoker{nomTime};                  // a time spec is mandatory, all further parameters are optional
        }
      
      
      /** @test the FeedManifold as adapter between Engine and processing library
       */
      void
      verify_FeedManifold()
        {
          // some random numbers to test...
          long r1 = rani(100);
          
          // Prepare setup to build a suitable FeedManifold
          using Buffer = long;
/////////////////////////////////////////////////////////////////////////////////TODO
  using T1 = tuple<int,double>;
  using T2 = array<int*,3>;
  using T3 = int;
  using T4 = int*;
  using T5 = lib::HeteroData<int*,long,double*>;
/////////////////////////////////////////////////////////////////////////////////TODO
    using lib::meta::ElmTypes;
    using S1 = ElmTypes<T5>;
SHOW_TYPE(S1)
SHOW_TYPE(S1::Seq)
SHOW_TYPE(S1::Tup)
SHOW_TYPE(S1::Idx)
    using S1A = S1::Apply<is_Buffer>;
SHOW_TYPE(S1A)
    using S1AR = ElmTypes<S1A>::Rebind<std::__and_>;
SHOW_TYPE(S1AR)
SHOW_EXPR(S1AR::value)
    using S1AA = S1::AndAll<is_Buffer>;
SHOW_TYPE(S1AA)
SHOW_EXPR(bool(S1AA()))
    using S1OA = S1::OrAll<is_Buffer>;
SHOW_TYPE(S1OA)
SHOW_EXPR(S1OA::value)
 
          auto fun_singleOut = [&](Buffer* buff) { *buff = r1; };
          
          // Example-1: a FeedManifold to adapt a simple generator function
          using M1 = FeedManifold<decltype(fun_singleOut)>;
          CHECK (not M1::hasInput());
          CHECK (not M1::hasParam());
          // instantiate...
          M1 m1{fun_singleOut};
          CHECK (1 == m1.outBuff.array().size());
          CHECK (nullptr == m1.outArgs );
//        CHECK (m1.inArgs );                              // does not compile because storage field is not provided
//        CHECK (m1.param );
          
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<long> (-55);
          CHECK (buff.isValid());
          CHECK (buff.accessAs<long>() == -55);
          
          m1.outBuff.createAt (0, buff);                   // plant a copy of the BuffHandle into the output slot
          CHECK (m1.outBuff[0].isValid());
          CHECK (m1.outBuff[0].accessAs<long>() == -55);
          
          m1.connect();                                    // instruct the manifold to connect buffers to arguments
          CHECK (isSameAdr (m1.outArgs, *buff));
          CHECK (*m1.outArgs == -55);
          
          m1.invoke();                                     // invoke the adapted processing function (fun_singleOut)
          CHECK (buff.accessAs<long>() == r1);             // result: the random number r1 was written into the buffer.
          
          // Example-2: adapt a function to process input -> output buffer
          auto fun_singleInOut = [](Buffer* in, Buffer* out) { *out = *in; };
          using M2 = FeedManifold<decltype(fun_singleInOut)>;
          CHECK (    M2::hasInput());///////////////////////////TODO broken due to overly convoluted logic!!!!
          CHECK (not M2::hasParam());
          CHECK (1 == M2::FAN_I);
          CHECK (1 == M2::FAN_O);
          // instantiate...
          M2 m2{fun_singleInOut};
//          CHECK (1 == m2.inBuff.array().size());   ///////////TODO : need to »lift« plain types for simplfied logic!!!!
          CHECK (1 == m2.outBuff.array().size());
//          CHECK (nullptr == m2.inArgs );
          CHECK (nullptr == m2.outArgs );

        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBase_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
