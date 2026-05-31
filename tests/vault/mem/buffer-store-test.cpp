/*
  BufferStore(Test)  -  verify how the BufferStore API is invoked from BufferProvider

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file buffer-store-test.cpp
 ** unit test \ref BufferStore_test
 */


#include "test/run.hpp"
#include "test/event-log.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/simple-buffer-state-registry.hpp"
#include "lib/format-obj.hpp"

#include <memory>
#include <string>

using test::EventLog;
using std::string;


namespace util { // add some rigged string-conversions....
  using vault::mem::Buff;
  using vault::mem::LocalTag;
  
  template<>
  struct StringConv<Buff*>
    {
      static std::string
      invoke (Buff const* dummyBuff) noexcept
      {
        return "Mem:" + util::showHashLSB ((size_t)dummyBuff);
      }
    };
  template<>
  struct StringConv<LocalTag>
    {
      static std::string
      invoke (LocalTag const& localTag) noexcept
      {
        uint64_t dummyMark(localTag);
        return "Tag:" + util::showHashLSB (dummyMark);
      }
    };
}

namespace vault::mem::test {
  
  
  namespace { // test fixture...
    /**
     * Dummy implementation of the BufferProvider interface.
     * Instead of a real storage backend, this test setup uses
     * an embedded EventLog to mark all calls received by the
     * BufferStore API-functions. Furthermore, the value
     * transported in the LocalTag is returned as buffer address.
     */
    class TestProvider
      : public BufferProviderSetup
      {
        
        class LoggingBufferStore
          : public BufferProviderSetup::Store
          {
            /** interpret the LocalTag as buffer address */
            Buff*
            asBuffer (LocalTag targetMarker)
              {
                void* buffMem{targetMarker};
                return static_cast<Buff*> (buffMem);
              }
            
            
            /* =========== BufferStore interface =========== */
            
            uint
            prepareBuffers (HashVal typeID, uint cnt,size_t siz)  override
              {
                log.call ("TestProvider", "prepareBuffers", typeID,cnt,siz);
                return cnt;
              }
            
            BuffAlloc
            provideBuffer (HashVal typeID, size_t siz, LocalTag localTag, int64_t customArg)  override
              {
                log.call ("TestProvider", "provideBuffer", typeID,siz,localTag,customArg);
                
                Buff* storage = asBuffer (localTag);
                log.event ("allocate").addAttrib("mem", util::toString(storage))
                                      .addAttrib("tag", util::toString(localTag))
                                      ;
                // A real BufferStore would perform the allocation task here...
                BuffAlloc storageSlot{storage, siz, localTag};
                return storageSlot;
              }
            
            void
            mark_emitted (HashVal typeID, BuffAlloc storageSlot)  override
              {
                auto& [storage,siz,localTag] = storageSlot;
                log.call ("TestProvider", "mark_emitted", typeID,storage,siz,localTag);
              }
            
            void
            detachBuffer (HashVal typeID, BuffAlloc storageSlot)  override
              {
                auto& [storage,siz,localTag] = storageSlot;
                log.call ("TestProvider", "detachBuffer", typeID,storage,siz,localTag);
              }
          };
        
        
      public:
        TestProvider()
          : BufferProviderSetup{*this}
          {
            log.event("Setup");
          }
          
          auto buildStage() { return std::make_unique<SimpleBufferStateRegistry> ("BufferStore_test"); }
          auto buildStore() { return std::make_unique<LoggingBufferStore>(); }
        
        
        static EventLog log;
        
        
        /** prepare a specifically rigged »buffer type« that passes the given \a mark
         *  as a LocalTag to the BufferStore, which will return it as buffer "address" */
        BuffDescr
        specifyBufferType (size_t siz, uint64_t mark)
          {
            auto& typeKey = bufferStage_->defineBufferType (siz, TypeHandler::RAW, LocalTag(mark));
            return buildDescriptor (typeKey);
          }
      };
    
    EventLog TestProvider::log("static BufferStore_test");
    
    
  }//(End) test fixture
  
  
  
  
  
  
  /***********************************************************************//**
   * @test demonstrate how usage of the BufferProvider interface
   *       translates into calls on the internal BufferStore API.
   *     - use an embedded EventLog to verify sequence of events
   *     - use a rigged BufferStore implementation that logs invocations
   *     - perform a typical buffer usage cycle
   * @see BufferProviderProtocol_test
   * @see EventLog_test
   */
  class BufferStore_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          auto& log = TestProvider::log;
          log.clear ("Start BufferStore_test");
          
          const size_t BUFFSIZ = 55;
          const size_t ANOUNCE = 23;
          const int64_t    ARG = 88;
          const uint64_t  MARK = 42;
          
          // this is how the packaged mark will be rendered...
          const string TAG = util::toString (LocalTag{MARK});
          const string MEM = util::toString ((Buff*)  MARK);
          
          
          TestProvider pro;
          
          BuffDescr descr = pro.specifyBufferType(BUFFSIZ, MARK);
          HashVal typeID{descr};

          log.event ("announce buffers");
          descr.announce (ANOUNCE);
          
          log.event ("lock a buffer");
          BuffHandle han = descr.lockBuffer (ARG);
          CHECK (han.isAllotted());
          
          Buff* mem = han.rawStorage();
          CHECK (MARK == (uint64_t)mem );
          
          log.event ("emit buffer");
          han.emit();
          
          log.event ("release buffer");
          han.release();
          CHECK (not han.isValid());
          
          
          cout << "____Event-Log_________________\n"
               << util::join(log,               "\n")
               << "\n───╼━━━━━━━━━━━╾──────────────"<<endl;
          
          CHECK (log.verify ("Start BufferStore_test")
                    .beforeEvent("Setup")
                    
                    .beforeEvent("announce buffers")
                    .beforeCall ("prepareBuffers").on("TestProvider").arg(typeID, ANOUNCE, BUFFSIZ)
                    
                    .beforeEvent("lock a buffer")
                    .beforeCall ("provideBuffer") .on("TestProvider").arg(typeID, BUFFSIZ, TAG, ARG)
                    .beforeEvent("allocate")      .attrib("mem", MEM)
                                                  .attrib("tag", TAG)
                    
                    .beforeEvent("emit buffer")
                    .beforeCall ("mark_emitted")  .on("TestProvider").arg(typeID, MEM, BUFFSIZ, TAG)
                    
                    .beforeEvent("release buffer")
                    .beforeCall ("detachBuffer")  .on("TestProvider").arg(typeID, MEM, BUFFSIZ, TAG)
                    );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferStore_test, "unit common");
  
  
}// namespace vault::mem::test
