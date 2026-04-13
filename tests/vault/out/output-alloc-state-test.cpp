/*
  OutputAllocState(Test)  -  verify generation details

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file output-alloc-state-test.cpp
 ** unit test \ref OutputAllocState_test
 */


#include "test/run.hpp"
#include "test/event-log.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/out/output-slot.hpp"
#include "vault/out/output-slot-connection.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <memory>
#include <string>

using std::string;
using test::EventLog;
using vault::mem::Buff;
using vault::out::FrameID;
using util::isSameAdr;


namespace vault::out::test {
  
  namespace { // test fixture...
    
    uint64_t DUMMY_BUFF{0xAaAaAa};
    
    EventLog log("static OutputAllocState_test");
    
    
    inline std::string
    mark (void const* dummyBuff) noexcept
    {
      return "Mem:" + util::showHashLSB ((size_t)dummyBuff);
    }

    /**
     * Dummy implementation of the OutputSlot::Connection interface.
     * Instead of engaging into actual output operations, this mock
     * implementation protocols all received calls into an EventLog.
     * A dummy memory address and buffer size is returned.
     */
    class MockConnection
      : public OutputSlot::Connection
      {
        
        /* === Connection API === */
        
        size_t
        getBufferSize()  const override
          {
            log.call ("MockConnection", "getBufferSize");
            return sizeof(DUMMY_BUFF);
          }
        
        Buff*
        claimBufferFor (FrameID frame)  override
          {
            log.call ("MockConnection", "claimBufferFor", frame);
            void* dummy = &DUMMY_BUFF;
            return static_cast<Buff*> (dummy);
          }
        
        void
        publish (Buff* buff)  override
          {
            log.call ("MockConnection", "publish", mark(buff));
          }
        
        void
        release (Buff* buff)  override
          {
            log.call ("MockConnection", "release", mark(buff));
          }
        
        void
        shutDown()  override
          {
            log.call ("MockConnection", "shutDown");
          }
        
      public:
        MockConnection() { log.call ("MockConnection", "create"); }
       ~MockConnection() { log.call ("MockConnection", "destroy"); }
      };
    
    
    inline OutputSlot
    makeFakeSlot()
    {
      const uint NUM_CONNECTIONS = 1;
      return OutputSlot::allocate<MockConnection> (NUM_CONNECTIONS
                                                  ,[](auto& storage)
                                                      {
                                                        storage.template create<MockConnection>();
                                                      });
    }
  }//(End) test fixture
  
  
  
  
  
  
  /***********************************************************************//**
   * @test demonstrate how the internal implementation hub within OutputSlot
   *       is connected to some OutputSlot::Connection implementation and
   *       exposes a BuffHandle as front-end.
   *     - use an embedded EventLog to verify sequence of events
   *     - use a rigged mock connection that logs invocations
   *     - perform all operations for a typical output usage
   * @see OutputSlotProtocol_test
   * @see EventLog_test
   */
  class OutputAllocState_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          log.clear ("Start OutputAllocState_test");
          
          const FrameCnt FRA_NO = 55;
          {
              log.event("create FakeSlot");
              auto slot = makeFakeSlot();
              
              log.event("retrieve DataSink");
              auto sinks = slot.getOpenedSinks();
              auto sink = *sinks;
              ++sinks;
              CHECK (not sinks);
              
              log.event("lock buffer");
              BuffHandle han = sink(FRA_NO);
              CHECK (han);
              CHECK (han.size() == sizeof(DUMMY_BUFF));
              CHECK (isSameAdr (han.rawStorage(), &DUMMY_BUFF));
              
              log.event("emit buffer");
              han.emit();
              CHECK (not han);
              
              log.event("release buffer");
              han.release();

              log.event("abandon FakeSlot");
          }// last instance of this slot goes out of scope...
          
          
          cout << "____Event-Log_________________\n"
               << util::join(log,               "\n")
               << "\n───╼━━━━━━━━━━━╾──────────────"<<endl;
          
          CHECK (log.verify ("Start OutputAllocState_test")
                    .beforeEvent("create FakeSlot")
                    .beforeCall ("create")        .on("MockConnection")
                    
                    .beforeEvent("retrieve DataSink")
                    .beforeCall ("getBufferSize") .on("MockConnection")
                    
                    .beforeEvent("lock buffer")
                    .beforeCall ("claimBufferFor").on("MockConnection").arg(FRA_NO)
                    
                    .beforeEvent("emit buffer")
                    .beforeCall ("publish")       .on("MockConnection").arg(mark(&DUMMY_BUFF))
                    
                    .beforeEvent("release buffer")
                    .beforeCall ("release")       .on("MockConnection").arg(mark(&DUMMY_BUFF))
                    
                    .beforeEvent("abandon FakeSlot")
                    .beforeCall ("shutDown")      .on("MockConnection")
                    .beforeCall ("destroy")       .on("MockConnection")
                    );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputAllocState_test, "unit common");
  
  
}// namespace vault::out::test
