/*
  LocalBufferStage(Test)  -  verify thread-local buffer state management

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file local-buffer-stage-test.cpp
 ** unit test \ref LocalBufferStage_test
 */


#include "lib/error.hpp"
#include "test/run.hpp"
//#include "test/test-frame.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/local-buffer-stage.hpp"
//#include "lib/uninitialised-storage.hpp"
#include "lib/depend-inject.hpp"
#include "lib/thread.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"////////////////TODO

#include <memory>


//using std::strncpy;
//using std::unique_ptr;
//using test::TestFrame;
//using test::testData;
//using util::isSameObject;
//using util::isnil;
//using lib::randStr;
//using lib::Literal;
using std::this_thread::yield;
using lib::Thread;

namespace vault {
namespace mem   {
namespace test  {
  
  using LERR_(LOGIC);
//  using LERR_(FATAL);
//  using LERR_(INVALID);
  using LERR_(LIFECYCLE);
  
  
  namespace { // Test fixture
    
    template<typename X>
    Buff*
    mark_as_Buffer(X& something)
      {
        return reinterpret_cast<Buff*> (std::addressof(something));
      }
    
  }//(End) Test fixture and helpers
  
  
  
  
  
  
  /***************************************************************//**
   * @test verify mechanism for a thread-local BufferStage implementation
   *     - verify data synchronisation between several BufferMetadata registries
   *     - verify typical usage situations, where a thread-local registry
   *       has to synchronise itself with a central metadata hub.
   */
  class LocalBufferStage_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          verify_DataTransfer();
          verify_publishNewKey();
          verify_fetchNewKeyOnUse();
        }
      
      
      
      /** @test key-chains can be exported and imported */
      void
      verify_DataTransfer()
        {
          BufferMetadata meta1, meta2;
          
          const HashVal FAM = ranHash();
          const size_t SIZ_D = sizeof(double);
          const double RANDD = 1.0 + defaultGen.uni();
          
          // create one type-key in Registry-1
          metadata::Key keyD = meta1.key (FAM,SIZ_D);
          CHECK (meta1.isKnown (keyD));
          CHECK (not meta2.isKnown (keyD));
          CHECK (meta1.get(keyD).isTypeKey());
          
          // create another type-key in Registry-2, this time with a constructor
          metadata::Key keyDD = meta2.key (FAM,SIZ_D, TypeHandler::create<double> (RANDD));
          CHECK (meta2.isKnown (keyDD));
          CHECK (not meta1.isKnown (keyDD));
          CHECK (meta2.get(keyDD).isTypeKey());
          
          // Keys are systematic => KeyD happens to be parent of KeyDD
          CHECK (keyD.storageSize() == keyDD.storageSize());
          CHECK (keyDD.parentKey() == HashVal(keyD));
          
          // synchronise a sub-type
          meta1.import (keyDD, meta2);
          CHECK (meta1.isKnown (keyDD));
          
          // now Registry-1 is able to use this subtype as well
          double testBuffer{0};
          metadata::Entry& eDD = meta1.markLocked (keyDD, mark_as_Buffer(testBuffer));
          CHECK (eDD.state() == LOCKED);
          CHECK (eDD.parentKey() == HashVal(keyDD));
          // and the »constructor« was indeed invoked...
          CHECK (testBuffer == RANDD);
          testBuffer += 1.1;   // add marker...
          
          // within the confines of allowed state transitions,
          // it is even possible to sync the state of active buffer entries
          CHECK (meta1.isKnown (eDD));
          CHECK (not meta2.isKnown (eDD));
          meta2.import (eDD, meta1);
          CHECK (meta2.isKnown (eDD));
          CHECK (meta2.get(eDD).state() == LOCKED);
          
          // importing the state record did not re-invoke the constructor
          CHECK (testBuffer == RANDD + 1.1);
          
          meta2.get(eDD).mark(EMITTED);
          CHECK (meta2.get(eDD).state() == EMITTED);
          CHECK (meta1.get(eDD).state() == LOCKED);
          meta1.import (eDD, meta2);
          CHECK (meta1.get(eDD).state() == EMITTED);
          
          // now mark the buffer as released in Registry-1
          CHECK (eDD.state() == EMITTED);
          eDD.mark(FREE);
          CHECK (eDD.state() == FREE);
          CHECK (meta2.get(eDD).state() == EMITTED);
          
          // it is not allowed to mark as de-allocated through synchronisation
          VERIFY_ERROR (LOGIC, meta2.import (eDD, meta1) );
          
          VERIFY_ERROR (LIFECYCLE, meta2.release(HashVal(eDD)) );
          CHECK (meta2.get(eDD).state() == EMITTED);
          meta2.get(eDD).invalidate (false);
          CHECK (meta2.get(eDD).state() == FREE);
          meta2.release(eDD);
          meta1.release(eDD);
        }
      
      
      
      /** @test simulate a common buffer provider usage situation,
       *        whereby a local instance in some worker thread provides
       *        a new (or refined) buffer type.
       *      - create empty new EngineBufferMetadata end inject it as mock
       *      - use a free standing LocalBufferStage instance (instead of a BufferProvider)
       *      - launch a worker thread to conduct the test invocation of this subject
       *      - notably register a totally new buffer type, from within the worker
       *      - then also perform the steps necessary to lock and release a buffer
       *      - check that the new type was migrated into the central metadata hub
       *      - verify that also the complete paretn-chain was published
       */
      void
      verify_publishNewKey()
        {
          // provide a »central Metadata hub« (as a transient instance for this test)
          lib::DependInject<EngineBufferMetadata>::Local<EngineBufferMetadata> metaHub;
          metaHub.triggerCreate();
          CHECK (0 == metaHub->cntEntries()); // no buffer metadata yet...
          
          // provide the *test subject*,
          // which implements the BufferStage API
          // and uses a thread-local metadata table internally
          LocalBufferStage localStage{"verify_publishNewKey"};
          BufferProviderSetup::Stage& stageAPI = localStage;
          
          // setup information used for buffer type registration
          const size_t SIZ_D = sizeof(double);
          const double RANDD = 1.0 + defaultGen.uni();
          const auto HANDLER = TypeHandler::create<double>(RANDD);
          const LocalTag TAG{defaultGen.u64()};
          
          HashVal metaID{0};
          
          // this is our »allocated buffer«
          double testBuffer{0};
          Buff* alloc = mark_as_Buffer (testBuffer);
          
          Thread testWorker{[&] /* === Use Case : a worker defines a new buffer type === */
                              {
                                auto& typeKey = stageAPI.defineBufferType (SIZ_D, HANDLER, TAG); //  ◁─────────────────┨ data up-sync happens here
                                metaID = HashVal(typeKey); // sneak out to verify later
                                
                                // emulate allocation and usage of a buffer
                                BuffAlloc allocRecord{alloc, typeKey.storageSize(),typeKey.localTag()};
                                auto& stateKey = stageAPI.mark_locked (typeKey, allocRecord);
                                
                                // verify the buffer constructor was applied
                                CHECK (testBuffer = RANDD);
                                
                                // release the buffer...
                                stageAPI.mark_released (stateKey);
                                stageAPI.discard (stateKey);    //  ◁──────────────────────────────────────────────────┨ buffer state entry never leaves the worker thread
                              }};
          while (testWorker)
            yield();     // wait for worker to finish
          
          
          CHECK (0 != metaID);
          CHECK (3 == metaHub->cntEntries());
          CHECK (metaHub->isKnown (metaID));
          
          // complete registration chain has been published to the central registry
          HashVal FAM_ID  = localStage.getFamilyID();
          HashVal parent1 = metaHub->lookup(metaID).parentKey();
          CHECK (metaHub->isKnown (parent1));
          CHECK (parent1 == metaHub->defineBufferType (FAM_ID, SIZ_D, HANDLER));
          
          HashVal parent0 = metaHub->lookup(parent1).parentKey();
          CHECK (metaHub->isKnown (parent0));
          CHECK (parent0 == metaHub->defineBufferType (FAM_ID, SIZ_D));
          
          CHECK (FAM_ID == metaHub->lookup(parent0).parentKey());
        }
      
      
      void
      verify_fetchNewKeyOnUse()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalBufferStage_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
