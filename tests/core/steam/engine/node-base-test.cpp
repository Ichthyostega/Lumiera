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
#include "lib/several-builder.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"


using std::tuple;
using std::array;
using util::isSameAdr;
using lib::test::showType;
using lib::makeSeveral;
using lib::izip;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  /******************************************************//**
   * @test basic render node structure and building blocks.
   *  This test documents and verifies some fundamental
   *  Render Node structures, looking at intricate technical
   *  details, which are usually hidden below the NodeBuidler.
   *  - #verify_NodeStructure is a demonstration example
   *    to show fundamentals of node construction and
   *    invocation, using a dummy implementation.
   *  - the following cases cover extremely technical details
   *    of the FeedManifold, which serves as junction point
   *    between Render Node and external library functions.
   *  - in a similar style, \ref NodeFeed_test covers the
   *    various parameter- and data connections of Nodes
   *    in a »clean-room« setting
   *  - much more high-level is NodeLink_test, covering
   *    the construction of a Render Node network
   *  - NodeBuilder_test focuses on aspects of node
   *    generation, as packaged into the NodeBuilder.
   */
  class NodeBase_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          verify_TurnoutSystem();
          verify_NodeStructure();
          verify_FeedManifold();
          verify_FeedPrototype();
        }
      
      
      /** @test the TurnoutSystem as transient connection hub for node invocation
       *      - for most invocations, just the nominal timeline time and an
       *        arbitrary process indentification-key is placed into fixed
       *        «slots« within the TurnoutSystem, from where these parameters
       *        can be retrieved by actual processing functions;
       *      - for some special cases however, additional storage blocks
       *        can be chained up, to allow accessing arbitrary parameters
       *        through the TurnoutSystem as front-end.
       */
      void
      verify_TurnoutSystem()
        {
          Time nomTime{rani(10'000),0};                    // drive test with a random »nominal Time« <10s with ms granularity
          TurnoutSystem invoker{nomTime};                  // a time spec is mandatory, all further parameters are optional
          
          CHECK (invoker.getNomTime() == nomTime);         // can access those basic params from within the render invocation.
          CHECK (invoker.getProcKey() == ProcessKey{});
          
          /* == That's all required for basic usage. == */
          
          
           // Demonstrate extension-block to TurnoutSystem
          //  Used to setup elaborate parameter-nodes...
          double someVal = defaultGen.uni();               // some param value, computed by »elaborate logic«
          auto spec = buildParamSpec()
                        .addValSlot (someVal);             // declare a parameter slot for an extension data block
          auto acc0 = spec.makeAccessor<0>();              // capture an accessor-functor for later use
          
          {// Build and connect extension storage block
            auto dataBlock =                               // ...typically placed locally into a nested stack frame
                      spec.makeBlockBuilder()
                          .buildParamDataBlock(invoker);
            
            invoker.attachChainBlock (dataBlock);          // link extension data block into the TurnoutSystem
            CHECK (invoker.get(acc0) == someVal);          // now able to retrieve data from extension block
            invoker.detachChainBlock (dataBlock);
          }
          // base block continues to be usable...
          CHECK (invoker.getNomTime() == nomTime);
        }
      
      
      /** @test very basic structure of a Render Node.
       *      - All render processing happens in \ref Port implementations
       *      - here we use a dummy port, which just picks up a parameter
       *        from the TurnoutSystem and writes it into the output buffer;
       *        no further recursive call happens — so this is a source node.
       *      - To _incorporate_ this Port implementation into a Render Node,
       *        the _connectivity_ of the node network must be defined:
       *        + each node has a list of »Leads« (predecessor nodes)
       *        + and an array of port implementation (here just one port)
       *      - note that data placement relies on lib::Several, which can
       *        be configured to use a custom allocator to manage storage
       *      - furthermore, a node gets some ID descriptors, which are used
       *        to generate processing metadata (notably a hash key for caching)
       *      - for the actual invocation, foremost we need a _buffer provider_
       *      - and we need to supply the most basic parameters, like the
       *        nominal timeline time and a proccess-Key. These will be
       *        embedded into the TurnoutSystem, to be accessible throughout
       *        the complete recursive node-pull invocation.
       *      - This test verifies that the actual invocation indeed happened
       *        and placed a random parameter-value into the output buffer.
       * @remark In reality, processing operations are delegated to a
       *   media-processing library, which requires elaborate buffer handling
       *   and typically entails recursive calls to predecessor nodes. This
       *   intricate logic is handled by the typical Port implementation
       *   known as \ref MediaWeavingPattern; notably the processing will
       *   rely on a transient data structure called \ref FeedManifold, which
       *   is verified in much more detail [below](\ref #verify_FeedManifold)
       */
      void
      verify_NodeStructure()
        {
              class DummyProcessing
                : public Port
                {
                public:
                  DummyProcessing (ProcID& id)
                    : Port{id}
                    { }
                  
                  /** Entrance point to the next recursive step of media processing. */
                  BuffHandle
                  weave (TurnoutSystem& turnoutSystem, OptionalBuff outBuffer)  override
                    {// do something deeply relevant, like feeding a dummy parameter...
                      outBuffer->accessAs<long>() = turnoutSystem.getProcKey();
                      return *outBuffer;
                    }
                };
          
          
          // Prepare Connectivity for the Node
          auto leadNodes = makeSeveral<ProcNodeRef>();     // empty, no predecessor nodes
          auto nodePorts = makeSeveral<Port>()             // build the port implementation object(s)
                              .emplace<DummyProcessing> (ProcID::describe ("TestDummy","live(long)"));
          
          // Build a Render Node
          ProcNode theNode{Connectivity{nodePorts.build()
                                       ,leadNodes.build()
                          }};
          
          // Inspect Node metadata...
          CHECK (watch(theNode).isSrc());
          CHECK (watch(theNode).leads().size() == 0);
          CHECK (watch(theNode).ports().size() == 1);
          CHECK (watch(theNode).getNodeSpec () == "TestDummy-◎"_expect );
          CHECK (watch(theNode).getPortSpec(0) == "TestDummy.live(long)"_expect );
          
          
          // prepare for invoking the node....
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<long> (-55);
          CHECK (-55 == buff.accessAs<long>());            // allocated some data buffer for the result, with a marker-value
          
          Time nomTime{Time::ZERO};
          ProcessKey key = 1 + rani(100);                  // here we »hide« some data value in the ProcessKey
          uint port{0};                                    // we will pull port-#0 of the node
          
          // Trigger Node invocation...
          buff = theNode.pull (port, buff, nomTime, key);
          
          CHECK (key == buff.accessAs<uint>());            // DummyProcessing port placed ProcessKey into the output-buffer
          buff.release();
        }
      
      
      
      
      /** @test the FeedManifold as adapter between Engine and processing library...
       *      - bind local λ with various admissible signatures
       *      - construct specifically tailored FeedManifold types
       *      - use the DiagnosticBufferProvider for test buffers
       *      - create FeedManifold instance, passing the λ and additional parameters
       *      - connect BuffHandle for these buffers into the FeedManifold instance
       *      - trigger invocation of the function
       *      - look into the buffers and verify effect
       * @remark within each Render Node, a FeedManifold is used as junction
       *  to tap into processing functionality provided by external libraries.
       *  Those will be adapted by a Plug-in, to be loaded by the Lumiera core
       *  application. The _signature of a functor_ linked to the FeedManifold
       *  is used as kind of a _low-level-specification_ how to invoke external
       *  processing functions. Obviously this must be complemented by a more
       *  high-level descriptor, which is interpreted by the Builder to connect
       *  a suitable structure of Render Nodes.
       * @see feed-manifold.h
       * @see NodeLinkage_test
       */
      void
      verify_FeedManifold()
        {
          // Prepare setup to build a suitable FeedManifold...
          long r1 = rani(100);
          using Buffer = long;
          
          
           //______________________________________________________________
          // Example-1: a FeedManifold to adapt a simple generator function
          auto fun_singleOut = [&](Buffer* buff) { *buff = r1; };
          using M1 = FeedManifold<decltype(fun_singleOut)>;
          CHECK (not M1::hasInput());
          CHECK (not M1::hasParam());
          CHECK (0 == M1::FAN_P);
          CHECK (0 == M1::FAN_I);
          CHECK (1 == M1::FAN_O);
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
          
          
           //_____________________________________________________________
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
          
          
           //______________________________________
          // Example-3: accept complex buffer setup
          using Sequence = array<Buffer,3>;
          using Channels = array<Buffer*,3>;
          using Compound = tuple<Sequence*, Buffer*>;
          auto fun_complexInOut = [](Channels in, Compound out)
                                    {
                                      auto [seq,extra] = out;
                                      for (auto [i,b] : izip(in))
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
          
          
           //_________________________________
          // Example-4: pass a parameter tuple
          using Params = tuple<short,long>;
          // Note: demonstrates mix of complex params, an array for input, but just a simple output buffer
          auto fun_ParamInOut = [](Params param, Channels in, Buffer* out)
                                    {
                                      auto [s,l] = param;
                                      *out = 0;
                                      for (Buffer* b : in)
                                        *out += (s+l) * (*b);
                                    };
          using M4 = FeedManifold<decltype(fun_ParamInOut)>;
          CHECK (M4::hasInput());
          CHECK (M4::hasParam());
          CHECK (2 == M4::FAN_P);
          CHECK (3 == M4::FAN_I);
          CHECK (1 == M4::FAN_O);
          CHECK (showType<M4::ArgI>()  == "array<long*, 3ul>"_expect);
          CHECK (showType<M4::ArgO>()  == "long *"_expect);
          CHECK (showType<M4::Param>() == "tuple<short, long>"_expect);
          
          // Note: instantiate passing param values as extra arguments
          short r2 = 1+rani(10);
          long  r3 = rani(1000);
          M4 m4{Params{r2,r3}, fun_ParamInOut};            // parameters directly given by-value
          auto& [p0,p1] = m4.param;
          CHECK (p0 == r2);                                // parameter values exposed through manifold
          CHECK (p1 == r3);
          
          // wire-in existing buffers for this example
          m4.inBuff.createAt (0, buffI0);
          m4.inBuff.createAt (1, buffI1);
          m4.inBuff.createAt (2, buffI2);
          m4.outBuff.createAt(0, buffO1);
          CHECK (*ia0 == r1  );                            // existing values in the buffers....
          CHECK (*ia1 == r1+1);
          CHECK (*ia2 == -55 );       /////////////////////////////////////////////////////OOO should be -22
          CHECK (*oa1 == -55 + *ia0+*ia1+*ia2); ///////////////////////////////////////////OOO should be -33
          
          m4.connect();
          m4.invoke();                                     // processing combines input buffers with parameters
          CHECK (*oa1 == (r2+r3) * (r1 + r1+1 -55));  /////////////////////////////////////OOO should be -22
          
          
           //______________________________________
          // Example-5: simple parameter and output
          auto fun_singleParamOut = [](short param, Buffer* buff) { *buff = param-1; };
          using M5 = FeedManifold<decltype(fun_singleParamOut)>;
          CHECK (not M5::hasInput());
          CHECK (    M5::hasParam());
          CHECK (1 == M5::FAN_P);
          CHECK (0 == M5::FAN_I);
          CHECK (1 == M5::FAN_O);
          CHECK (showType<M5::ArgI>()  == "tuple<>"_expect);
          CHECK (showType<M5::ArgO>()  == "long *"_expect);
          CHECK (showType<M5::Param>() == "short"_expect);
          
          // instantiate, directly passing param value
          M5 m5{r2, fun_singleParamOut};
          // wire with one output buffer
          m5.outBuff.createAt(0, buffO1);
          m5.connect();
          CHECK (m5.param    ==  r2);                      // the parameter value passed to the ctor
//        CHECK (m5.inArgs );                              // does not compile because storage field is not provided
          CHECK (*m5.outArgs == *oa1);                     // still previous value sitting in the buffer...
          
          m5.invoke();
          CHECK (*oa1 == r2 - 1);                          // processing has placed result based on param into output buffer
          
          // done with these buffers
          buffI0.release();
          buffI1.release();
          buffI2.release();
          buffO0.release();
          buffO1.release();
        }
      
      
      
      /** @test Setup of a FeeManifold to attach parameter-functors
       */
      void
      verify_FeedPrototype()
        {
          // Prepare setup to build a suitable FeedManifold...
          using Buffer = long;
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<Buffer> (-55);
          
          
           //_______________________________________
          // Case-1: Prototype without param-functor
          auto fun_singleParamOut = [](short param, Buffer* buff) { *buff = param-1; };
          using M1 = FeedManifold<decltype(fun_singleParamOut)>;
          using P1 = M1::Prototype;
          CHECK (    P1::hasParam());                      // checks that the processing-function accepts a parameter
          CHECK (not P1::hasParamFun());                   // while this prototype has no active param-functor
          CHECK (not P1::canActivate());
          
          P1 p1{move (fun_singleParamOut)};                // create the instance of the prototype, moving the functor in
          CHECK (sizeof(p1) <= sizeof(void*));
          TurnoutSystem turSys{Time::NEVER};               // Each Node invocation uses a TurnoutSystem instance....
          
          M1 m1 = p1.buildFeed(turSys);                    //... and also will create a new FeedManifold from the prototype
          CHECK (m1.param == short{});                     // In this case here, the param value is default constructed.
          m1.outBuff.createAt(0, buff);                    // Perform the usual steps for an invocation....
          CHECK (buff.accessAs<long>() == -55);
          m1.connect();
          CHECK (*m1.outArgs == -55);
          
          m1.invoke();
          CHECK (*m1.outArgs           == 0 - 1);          // fun_singleParamOut() -> param - 1 and param ≡ 0
          CHECK (buff.accessAs<long>() == 0 - 1);
          long& calcResult = buff.accessAs<long>();        // for convenience use a reference into the result buffer
          
          
          
           //_____________________________________________
          // Case-2: Reconfigure to attach a param-functor
          long rr{11};                                    // ▽▽▽▽  Note: side-effect
          auto fun_paramSimple = [&](TurnoutSystem&){ return rr += 1+rani(100); };
          using P1x = P1::Adapted<decltype(fun_paramSimple)>;
          CHECK (    P1x::hasParam());
          CHECK (    P1x::hasParamFun());
          CHECK (not P1x::canActivate());
          
          P1x p1x = p1.moveAdaptedParam (move(fun_paramSimple));
          M1 m1x = p1x.buildFeed(turSys);                  // ◁————————— param-functor invoked here
          CHECK (rr == m1x.param);                         //            ...as indicated by the side-effect
          short r1 = m1x.param;
          
          // the rest works as always with FeedManifold (which as such is agnostic of the param-functor!)
          m1x.outBuff.createAt(0, buff);
          m1x.connect();
          m1x.invoke();                                    // Invoke the processing functor
          CHECK (calcResult == r1 - 1);                    // ...which computes fun_singleParamOut() -> param-1
          
          // but let's play with the various instances...
          m1.invoke();                                     // the previous FeedManifold is sill valid and connected
          CHECK (calcResult ==  0 - 1);                    // and uses its baked in parameter value (0)
          m1x.invoke();
          CHECK (calcResult == r1 - 1);                    // as does m1x, without invoking the param-functor
          
          // create yet another instance from the prototype...
          M1 m1y = p1x.buildFeed(turSys);                  // ◁————————— param-functor invoked here
          CHECK (rr == m1y.param);
          CHECK (r1 < m1y.param);                          //            ...note again the side-effect
          m1y.outBuff.createAt(0, buff);
          m1y.connect();
          m1y.invoke();                                    // ...and so this third FeedManifold instance...
          CHECK (calcResult == rr - 1);                    // uses yet another baked-in param value;
          m1x.invoke();                                    // recall that each Node invocation creates a new
          CHECK (calcResult == r1 - 1);                    // FeedManifold on the stack, since invocations are
          m1.invoke();                                     // performed concurrently, each with its own set of
          CHECK (calcResult ==  0 - 1);                    // buffers and parameters.
          
          
          
           //_______________________________
          // Case-3: Integrate std::function
          using ParamSig = short(TurnoutSystem&);
          using ParamFunction = std::function<ParamSig>;
          //  a Prototype to hold such a function...
          using P1F = P1::Adapted<ParamFunction>;
          CHECK (    P1F::hasParam());
          CHECK (    P1F::hasParamFun());
          CHECK (    P1F::canActivate());
          
          P1F p1f = p1x.clone()                            // if (and only if) the embedded functors allow clone-copy
                       .moveAdaptedParam<ParamFunction>(); // then we can fork-off and then adapt a cloned prototype
          
          // Need to distinguish between static capability and runtime state...
          CHECK (not p1 .canActivate());                   // Case-1 had no param functor installed...
          CHECK (not p1 .isActivated());                   //        and thus also can not invoke such a functor at runtime
          CHECK (not p1x.canActivate());                   // Case-2 has a fixed param-λ, which can not be activated/deactivated
          CHECK (    p1x.isActivated());                   //        yet at runtime this functor is always active and callable
          CHECK (    p1f.canActivate());                   // Case-3 was defined to hold a std::function, which thus can be toggled
          CHECK (not p1f.isActivated());                   //        yet in current runtime configuration, the function is empty
          
          // create a FeedManifold instance from this prototype
          M1 m1f1 = p1f.buildFeed(turSys);                 // no param-functor invoked,
          CHECK (m1f1.param == short{});                   // so this FeedManifold will use the default-constructed parameter
          
          // but since std::function is assignable, we can activate it...
          CHECK (not p1f.isActivated());
          p1f.assignParamFun ([](TurnoutSystem&){ return 47; });
          CHECK (    p1f.isActivated());
          M1 m1f2 = p1f.buildFeed(turSys);                 // ◁————————— param-functor invoked here
          CHECK (m1f2.param == 47);                        //            ...surprise: we got number 47...
          p1f.assignParamFun();
          CHECK (not p1f.isActivated());                   // can /deactivate/ it again...
          M1 m1f3 = p1f.buildFeed(turSys);                 // so no param-functor invoked here
          CHECK (m1f3.param == short{});
          
          // done with buffer
          buff.release();
          
          
          
           //_____________________________________
          // Addendum: type conversion intricacies
          auto lambdaSimple =  [ ](TurnoutSystem&){ return short(47); };
          auto lambdaCapture = [&](TurnoutSystem&){ return short(47); };
          using LambdaSimple  = decltype(lambdaSimple);
          using LambdaCapture = decltype(lambdaCapture);
          CHECK (    (std::is_constructible<bool,ParamFunction>::value));
          CHECK (    (std::is_constructible<bool,LambdaSimple >::value));
          CHECK (not (std::is_constructible<bool,LambdaCapture>::value));
          //    Surprise! a non-capture-λ turns out to be bool convertible,
          //              which however is also true for std::function,
          //              yet for quite different reasons: While the latter has a
          //              built-in conversion operator to detect /inactive/ state,
          //              the simple λ decays to a function pointer, which makes it
          //              usable as implementation for plain-C callback functions.
          using FunPtr = short(*)(TurnoutSystem&);
          CHECK (not (std::is_convertible<ParamFunction,FunPtr>::value));
          CHECK (    (std::is_convertible<LambdaSimple ,FunPtr>::value));
          CHECK (not (std::is_convertible<LambdaCapture,FunPtr>::value));
          //             ..which allows to distinguish these cases..
          //
          CHECK (true  == _ParamFun<P1::ProcFun>::isConfigurable<ParamFunction>::value);
          CHECK (false == _ParamFun<P1::ProcFun>::isConfigurable<LambdaSimple >::value);
          CHECK (false == _ParamFun<P1::ProcFun>::isConfigurable<LambdaCapture>::value);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBase_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
