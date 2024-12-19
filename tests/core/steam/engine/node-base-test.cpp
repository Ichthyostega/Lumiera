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
#include "lib/iter-zip.hpp"
#include "lib/meta/function.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"
#include "lib/test/test-helper.hpp"
//#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////////TODO
#include "lib/format-util.hpp"///////////////////////////////TODO
#include "lib/util.hpp"


//using std::string;
using std::tuple;
using std::array;
using util::isSameAdr;
using lib::test::showType;


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
          // Prepare setup to build a suitable FeedManifold...
          long r1 = rani(100);
          using Buffer = long;
          
          
          // Example-1: a FeedManifold to adapt a simple generator function
          auto fun_singleOut = [&](Buffer* buff) { *buff = r1; };
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
          BuffHandle buff = provider.lockBufferFor<Buffer> (-55);
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
          auto fun_singleInOut = [](Buffer* in, Buffer* out) { *out = *in + 1; };
          using M2 = FeedManifold<decltype(fun_singleInOut)>;
          CHECK (    M2::hasInput());
          CHECK (not M2::hasParam());
          CHECK (1 == M2::FAN_I);
          CHECK (1 == M2::FAN_O);
          // instantiate...
          M2 m2{fun_singleInOut};
          CHECK (1 == m2.inBuff.array().size());
          CHECK (1 == m2.outBuff.array().size());
          CHECK (nullptr == m2.inArgs );
          CHECK (nullptr == m2.outArgs );
          
          // use the result of the preceding Example-1 as input
          // and get a new buffer to capture the output
          BuffHandle buffOut = provider.lockBufferFor<Buffer> (-99);
          CHECK (buff.accessAs<long>()    ==  r1);
          CHECK (buffOut.accessAs<long>() == -55);  ///////////////////////////////////////OOO should be -99 --> aliasing of buffer meta records due to bug with hash generation
          
          // configure the Manifold-2 with this input and output buffer
          m2.inBuff.createAt (0, buff);
          m2.outBuff.createAt(0, buffOut);
          CHECK (m2.inBuff[0].isValid());
          CHECK (m2.inBuff[0].accessAs<long>() == r1 );
          CHECK (m2.outBuff[0].isValid());
          CHECK (m2.outBuff[0].accessAs<long>() == -55);   ////////////////////////////////OOO should be -99
          
          // connect arguments to buffers
          m2.connect();
          CHECK (isSameAdr (m2.inArgs,  *buff));
          CHECK (isSameAdr (m2.outArgs, *buffOut));
          CHECK (*m2.outArgs == -55);                      ////////////////////////////////OOO should be -99
          
          m2.invoke();
          CHECK (buffOut.accessAs<long>() == r1+1);
          
          
          // Example-3: accept complex buffer setup
          using Sequence = array<Buffer,3>;
          using Channels = array<Buffer*,3>;
          using Compound = tuple<Sequence*, Buffer*>;
          auto fun_complexInOut = [](Channels in, Compound out)
                                    {
                                      auto [seq,extra] = out;
                                      for (auto [i,b] : lib::izip(in))
                                        {
                                          (*seq)[i] = *b + 1;
                                          *extra += *b;
                                        }
                                    };
          using M3 = FeedManifold<decltype(fun_complexInOut)>;
          CHECK (    M3::hasInput());
          CHECK (not M3::hasParam());
          CHECK (3 == M3::FAN_I);
          CHECK (2 == M3::FAN_O);
          CHECK (showType<M3::ArgI>() == "array<long*, 3ul>"_expect);
          CHECK (showType<M3::ArgO>() == "tuple<array<long, 3ul>*, long*>"_expect);
          // instantiate...
          M3 m3{fun_complexInOut};
          CHECK (3 == m3.inBuff.array().size());
          CHECK (2 == m3.outBuff.array().size());
          
          // use existing buffers and one additional buffer for input
          BuffHandle buffI0 = buff;
          BuffHandle buffI1 = buffOut;
          BuffHandle buffI2 = provider.lockBufferFor<Buffer> (-22);
          CHECK (buffI0.accessAs<long>() == r1  );         // (result from Example-1)
          CHECK (buffI1.accessAs<long>() == r1+1);         // (result from Example-2)
          CHECK (buffI2.accessAs<long>() == -55 );  ///////////////////////////////////////OOO should be -22
          // prepare a compound buffer and an extra buffer for output...
          BuffHandle buffO0 = provider.lockBufferFor<Sequence> (Sequence{-111,-222,-333});
          BuffHandle buffO1 = provider.lockBufferFor<Buffer> (-33);
          CHECK ((buffO0.accessAs<Sequence>() == Sequence{-111,-222,-333}));
          CHECK (buffO1.accessAs<long>() == -55 );  ///////////////////////////////////////OOO should be -33
           
          // configure the Manifold-3 with these input and output buffers
          m3.inBuff.createAt (0, buffI0);
          m3.inBuff.createAt (1, buffI1);
          m3.inBuff.createAt (2, buffI2);
          m3.outBuff.createAt(0, buffO0);
          m3.outBuff.createAt(1, buffO1);
          m3.connect();
          // Verify data exposed prior to invocation....
          auto& [ia0,ia1,ia2] = m3.inArgs;
          auto& [oa0,oa1]     = m3.outArgs;
          auto& [o00,o01,o02] = *oa0;
          CHECK (*ia0 == r1  );
          CHECK (*ia1 == r1+1);
          CHECK (*ia2 == -55 );       /////////////////////////////////////////////////////OOO should be -22
          CHECK ( o00 == -111);
          CHECK ( o01 == -222);
          CHECK ( o02 == -333);
          CHECK (*oa1 == -55 );       /////////////////////////////////////////////////////OOO should be -33
          
          m3.invoke();
          CHECK (*ia0 == r1  );                            // Input buffers unchanged
          CHECK (*ia1 == r1+1);
          CHECK (*ia2 == -55 );       /////////////////////////////////////////////////////OOO should be -22
          CHECK ( o00 == *ia0+1);                          // Output buffers as processed by the function
          CHECK ( o01 == *ia1+1);
          CHECK ( o02 == *ia2+1);
          CHECK (*oa1 == -55 + *ia0+*ia1+*ia2); ///////////////////////////////////////////OOO should be -33
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBase_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
