/*
  ModelPortRegistry(Test)  -  verify handling of model ports

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file model-port-registry-test.cpp
 ** unit test \ref ModelPortRegistry_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/fixture/model-port-registry.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/pipe.hpp"
#include "common/query.hpp"
#include "lib/util.hpp"


namespace steam {
namespace fixture {
namespace test  {
  
  using util::isSameObject;
  using util::isnil;
  
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using asset::Timeline;
  using asset::PTimeline;
  using mobject::ModelPort;
  using lumiera::Query;
  
  using PID = asset::ID<Pipe>;
  using TID = asset::ID<Struct>;
    
  using MPDescriptor = ModelPortRegistry::ModelPortDescriptor const&;
  
  using mobject::LUMIERA_ERROR_INVALID_MODEL_PORT;
  using mobject::LUMIERA_ERROR_UNCONNECTED_MODEL_PORT;
  
  
  namespace { // test environment
    
    inline PID
    getPipe (string id)
    {
      return Pipe::query ("id("+id+")");
    }
    
    inline TID
    getTimeline (string id)
    {
      return asset::Struct::retrieve (Query<Timeline> ("id("+id+")"))->getID();
    }
    
    struct TestContext
      {
        ModelPortRegistry registry_;
        ModelPortRegistry* previous_;
        
        /** setup */
        TestContext()
          : registry_()
          , previous_(ModelPortRegistry::setActiveInstance (registry_))
          { }
        
        /** tear-down */
       ~TestContext()
          {
            if (previous_)
              ModelPortRegistry::setActiveInstance (*previous_);
            else
              ModelPortRegistry::shutdown();
          }
      };
  }
  
  
  
  
  /*****************************************************************************//**
   * @test create a standalone model port registry to verify the behaviour of
   *       model ports, accessed through reference handles. This test provides an
   *       example setup detached from the real usage situation within the builder.
   *       The ModelPortRegistry management interface is used to create and track a
   *       set of model ports, to be made visible by an atomic, transactional switch.
   *       The access for client code through the ModelPort frontend is then verified.
   *       
   * @see  mobject::ModelPort
   * @see  mobject::builder::ModelPortRegistry
   */
  class ModelPortRegistry_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestContext ctx;
          
          fabricating_ModelPorts (ctx.registry_);
          accessing_ModelPorts();
          transactionalSwitch (ctx.registry_);
        }
      
      
      void
      fabricating_ModelPorts (ModelPortRegistry& registry)
        {
          /* == some Assets to play with == */
          PID pipeA        = getPipe ("pipeA");
          PID pipeB        = getPipe ("pipeB");
          TID someTimeline = getTimeline ("some_test_Timeline");
          
          // start out with defining some new model ports......
          MPDescriptor p1 = registry.definePort (pipeA, someTimeline);
          MPDescriptor p2 = registry.definePort (pipeB, someTimeline);
          
          CHECK (registry.contains (pipeA));
          CHECK (registry.contains (pipeB));
          
          VERIFY_ERROR (DUPLICATE_MODEL_PORT, registry.definePort(pipeB, someTimeline) );
          CHECK (registry.contains (pipeB));
          
          CHECK (pipeA        == p1.id());
          CHECK (pipeB        == p2.id());
          CHECK (someTimeline == p1.holder());
          CHECK (someTimeline == p2.holder());
          
          registry.commit();
        }
      
      
      void
      accessing_ModelPorts ()
        {
          PID pipeA  = getPipe ("pipeA");
          PID pipeB  = getPipe ("pipeB");
          PID pipeWC = getPipe ("WCpipe");
          
          ModelPort mp1(pipeA);
          ModelPort mp2(pipeB);
          
          VERIFY_ERROR (INVALID_MODEL_PORT, ModelPort unbefitting(pipeWC) );
          
          ModelPort mp1x(pipeA);                 // can be created multiple times
          ModelPort mp2x(mp1x);                  // can be copied at will
          ModelPort mpNull;                      // can be default constructed (->unconnected)
          
          CHECK (mp1);
          CHECK (mp2);
          CHECK (mp1x);
          CHECK (mp2x);
          CHECK (!mpNull);                       // bool check verifies setup and connected state
          
          CHECK ( ModelPort::exists (pipeA));    // this is the same check, but invoked just with an pipe-ID
          CHECK ( ModelPort::exists (pipeB));
          CHECK (!ModelPort::exists (pipeWC));
          
          CHECK (mp1 == mp1x);
          CHECK (!isSameObject (mp1, mp1x));
          CHECK (mp1 != mp2);
          CHECK (mp2 != mp1);
          CHECK (mp1 != mpNull);
          CHECK (mp2 != mpNull);
          
          CHECK (mp1.pipe() == pipeA);
          CHECK (mp2.pipe() == pipeB);
          CHECK (mp1x.pipe() == pipeA);
          VERIFY_ERROR (UNCONNECTED_MODEL_PORT, mpNull.pipe()); // any further operations on an unconnected port will throw
          VERIFY_ERROR (UNCONNECTED_MODEL_PORT, mpNull.holder());
          
          CHECK (mp1.streamType() == pipeA.streamType());
        }
      
      
      void
      transactionalSwitch (ModelPortRegistry& registry)
        {
          PID pipeA  = getPipe ("pipeA");
          PID pipeB  = getPipe ("pipeB");
          PID pipeWC = getPipe ("WCpipe");
          
          CHECK ( ModelPort::exists (pipeB));
          CHECK (!ModelPort::exists (pipeWC));
          
          CHECK (ModelPort::exists (pipeA));
          CHECK (registry.contains (pipeA));
          registry.remove (pipeA);
          CHECK (!registry.contains (pipeA));    // removed from the current (pending) transaction
          CHECK ( ModelPort::exists (pipeA));    // but not yet publicly visible
          
          // now create a new and differing definition of port A 
          TID anotherTimeline = getTimeline ("another_test_Timeline");
          MPDescriptor p1 = registry.definePort (pipeA, anotherTimeline);
          CHECK (registry.contains (pipeA));
          CHECK (anotherTimeline == p1.holder());
          CHECK (ModelPort(pipeA).holder() != anotherTimeline);
          
          registry.remove (pipeB);               // some more wired definitions 
          registry.definePort (pipeWC, anotherTimeline);
          CHECK (!registry.contains (pipeB));
          CHECK ( registry.contains (pipeWC));
          CHECK ( ModelPort::exists (pipeB));
          CHECK (!ModelPort::exists (pipeWC));
          CHECK ( registry.isRegistered (pipeB));   // this is the same as ModelPort::exists
          CHECK (!registry.isRegistered (pipeWC)); // 
                                                  //  Note: pending transaction not yet committed
          ModelPort portA(pipeA);                // ...... thus the changes aren't reflected to client code
          ModelPort portB(pipeB);
          VERIFY_ERROR (INVALID_MODEL_PORT, ModelPort ineptly(pipeWC));
          CHECK (portA);
          CHECK (portB);
          CHECK (portA.pipe() == pipeA);
          CHECK (portB.pipe() == pipeB);
          CHECK (portA.holder() != anotherTimeline);
          
          registry.commit();
          CHECK ( ModelPort::exists (pipeA));    // now all our changes got publicly visible
          CHECK (!ModelPort::exists (pipeB));
          CHECK ( ModelPort::exists (pipeWC));
          CHECK ( portA);
          CHECK (!portB);
          CHECK (portA.holder() == anotherTimeline);
          CHECK (portA.pipe() == pipeA);
          VERIFY_ERROR (INVALID_MODEL_PORT, portB.pipe());
          
          ModelPort pwc(pipeWC);                 // now clients may also use the now officially promoted new port
          CHECK (pwc);
          CHECK (pwc.pipe() == pipeWC);
          CHECK (pwc.holder() == anotherTimeline);
          
          // Next: doing several changes,
          //       but finally *not* committing them...
          CHECK ( registry.contains (pipeA));
          CHECK (!registry.contains (pipeB));
          CHECK ( registry.contains (pipeWC));
          registry.remove (pipeA);
          registry.clear();                      // remove everything from the pending transaction
          CHECK (!registry.contains (pipeA));
          CHECK (!registry.contains (pipeB));
          CHECK (!registry.contains (pipeWC));
          
          registry.definePort (pipeB, anotherTimeline);
          CHECK ( registry.contains (pipeB));
          CHECK (!portB);                        // not committed and thus not visible
          CHECK (portA);
          CHECK (pwc);
          
          registry.rollback();
          CHECK ( registry.contains (pipeA));    // no effect to the officially visible state
          CHECK (!registry.contains (pipeB));
          CHECK ( registry.contains (pipeWC));
          
          VERIFY_ERROR(INVALID_MODEL_PORT, registry.get(pipeB) );
          CHECK (!portB);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ModelPortRegistry_test, "unit fixture session builder");
  
  
  
}}} // namespace steam::fixture::test
