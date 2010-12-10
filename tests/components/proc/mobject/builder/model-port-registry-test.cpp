/*
  ModelPortRegistry(Test)  -  verify handling of model ports
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/builder/model-port-registry.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/timeline.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"

//#include <boost/format.hpp>
//#include <boost/scoped_ptr.hpp>
#include <string>


namespace mobject {
namespace builder {
namespace test  {
  
  //using boost::format;
  //using boost::scoped_ptr;
  using util::isSameObject;
  using util::isnil;
  using std::string;
  
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using asset::Timeline;
  using asset::PTimeline;
  using lumiera::Query;
  
  typedef asset::ID<Pipe> PID;
  typedef asset::ID<Struct> TID;
  
  
  namespace { // test environment
    
    inline PID
    getPipe (string id)
    {
      return Pipe::query("id("+id+")");
    }
    
    inline TID
    getTimeline (string id)
    {
      return asset::Struct::retrieve (Query<Timeline> ("id("+id+")"))->getID();
    }
    
    typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
    
    struct TestContext
      {
        ModelPortRegistry registry_;
        ModelPortRegistry& previous_;
        
        /** setup */
        TestContext()
          : registry_()
          , previous_(ModelPortRegistry::setActiveInstance (registry_))
          { }
        
        /** tear-down */
       ~TestContext()
          {
            ModelPortRegistry::setActiveInstance (previous_);
          }
      };
    
  }
  
  
  /*********************************************************************************
   * @test create a standalone model port registry to verify the behaviour of
   *       model ports, accessed through reference handles. This test provides
   *       an example setup detached from the real usage situation within the builder.
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
          PID pipeWC       = getPipe ("WCpipe");
          TID someTimeline = getTimeline ("some_test_Timeline");
          
          // start out with defining some new model ports......
          MPDescriptor p1 = registry.definePort (pipeA, someTimeline);
          MPDescriptor p2 = registry.definePort (pipeB, someTimeline);
          
          CHECK (registry.contains (pipeA));
          CHECK (registry.contains (pipeB));
          
          VERIFY_ERROR (DUPLICATE_MODEL_PORT, registry.definePort(pipeB, someTimeline) );
          CHECK (registry.contains (pipeB));
          
          CHECK (p1.id == pipeA);
          CHECK (p2.id == pipeB);
          CHECK (p1.holder == someTimeline);
          CHECK (p2.holder == someTimeline);
          
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
          
          VERIFY_ERROR (INVALID_MODEL_PORT, ModelPort(pipeWC));
          
          ModelPort mp1x(pipeA);
          ModelPort mpNull;
          
          CHECK (mp1);
          CHECK (mp2);
          CHECK (mp1x);
          CHECK (!mpNull);
          
          CHECK ( ModelPort::exists (pipeA));
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
          VERIFY_ERROR (UNCONNECTED_MODEL_PORT, mpNull.pipe());
          
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
          CHECK ( ModelPort::exists (pipeA));
          CHECK (!registry.contains (pipeA));
          
          // now create a new and differing definition of port A 
          TID anotherTimeline = getTimeline ("another_test_Timeline");
          MPDescriptor p1 = registry.definePort (pipeA, anotherTimeline);
          CHECK (registry.contains (pipeA));
          CHECK (p1.holder == anotherTimeline);
          CHECK (ModelPort(pipeA).holder() != anotherTimeline);
          
          registry.remove (pipeB);
          registry.definePort (pipeWC,anotherTimeline);
          CHECK (!registry.contains (pipeB));
          CHECK ( registry.contains (pipeWC));
          CHECK ( ModelPort::exists (pipeB));
          CHECK (!ModelPort::exists (pipeWC));

          ModelPort portA(pipeA);
          ModelPort portB(pipeB);
          VERIFY_ERROR (INVALID_MODEL_PORT, ModelPort(pipeWC));
          CHECK (portA);
          CHECK (portB);
          CHECK (portA.pipe() == pipeA);
          CHECK (portB.pipe() == pipeB);
          CHECK (portA.holder() != anotherTimeline);
          
          registry.commit();
          CHECK ( ModelPort::exists (pipeA));
          CHECK (!ModelPort::exists (pipeB));
          CHECK ( ModelPort::exists (pipeWC));
          CHECK ( portA);
          CHECK (!portB);
          CHECK (portA.holder() == anotherTimeline);
          CHECK (portA.pipe() == pipeA);
          VERIFY_ERROR (UNCONNECTED_MODEL_PORT, portB.pipe());
          
          ModelPort pwc(pipeWC);
          CHECK (pwc);
          CHECK (pwc.pipe() == pipeWC);
          CHECK (pwc.holder() == anotherTimeline);
          
          registry.remove (pipeA);
          registry.clear();
          CHECK (!registry.contains (pipeA));
          CHECK (!registry.contains (pipeB));
          CHECK (!registry.contains (pipeWC));
          
          registry.rollback();
          CHECK ( registry.contains (pipeA));
          CHECK ( registry.contains (pipeB));
          CHECK ( registry.contains (pipeWC));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ModelPortRegistry_test, "unit session builder");
  
  
  
}}} // namespace mobject::builder::test
