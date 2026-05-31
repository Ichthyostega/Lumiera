/*
  LiveMark(Test)  -  verify safeguard against invoking a deceased partner
  
   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>
  
  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file live-mark-test.cpp
 ** unit test \ref LiveMark_test
 */

#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/thread.hpp"
#include "lib/util.hpp"

#include "lib/live-mark.hpp"

#include <boost/lockfree/queue.hpp>
#include <atomic>


using util::isSameAdr;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""us;


namespace lib {
namespace test {
  
  using LERR_(BOTTOM_VALUE);
  
  
  /****************************************************************//**
   * @test validate a safety tag to prevent segfault when accessing
   *       a deceased object through a previously valid pointer.
   *       - demonstrate how to embody and use the LiveMark tag.
   *       - verify that access is only valid for the unique single
   *         object that generated the access-handle.
   *       - demonstrate that the protection mechanism can be used
   *         to safeguard a multithreaded sender/receiver scenario.
   * @see live-mark.hpp
   * @see LocalBufferStore_test
   */
  class LiveMark_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_distinctInstances();
          verify_multithreadAccess();
        }
      
      
        /**
         * Test target structure
         * protected with a LiveMark tag
         */
        struct Sidekick
          {
            LiveMark mark{*this};
            uint64_t feat{0};
            
            LiveMark const& getLiveMark() const { return mark; }
          };
      
      using Handle = LiveMark::SafeHandle<Sidekick>;
      
      
      /** @test attach a LiveMark to a simple object
       *        and create a safe access handle, allowing
       *        to detect when the target went out of scope */
      void simpleUsage ()
        {
          Handle handle;
          CHECK (not handle);
          {
            Sidekick kick;
            handle = kick.mark.makeHandle (kick);
            CHECK (handle);
            handle->feat = 55;
            CHECK (55 == kick.feat);
          }//   ◁──────────────────┨ Sidekick vanishes
          
          CHECK (not handle);
          VERIFY_ERROR (BOTTOM_VALUE, *handle );
        }
      
      
      
      /** @test re-use of the same memory location for another instance
       *        is indeed properly detected, preventing unintended access
       *        through the handle obtained from a preceding instance */
      void verify_distinctInstances()
        {
          UninitialisedStorage<Sidekick, 1> dodgyPlace;
          
          Sidekick* pos = new(&dodgyPlace) Sidekick;
          pos->feat = 23;
          CHECK (23 == dodgyPlace[0].feat);
          
          Handle handle = pos->mark.makeHandle (*pos);
          CHECK (handle);
          
          // destroy the first instance
          pos->~Sidekick();
          
          // memory content is still there...
          CHECK (23 == pos->feat);

          // insidiously plant a new object
          // into the same memory location on the stack
          new(&dodgyPlace) Sidekick;
          
          // the constructor initialised the new object,
          // but we can manipulate its content through the old pointer;
          CHECK (isSameAdr (pos, &dodgyPlace));
          CHECK (0 == pos->feat);
          
          pos->feat = 55;
          CHECK (55 == dodgyPlace[0].feat);
          
          // but the safe handle prevents unintended aliasing
          CHECK (not handle);
        }
      
      
      
      /** @test assess the liveness state from another thread
       *      - setup a producer ⟶ consumer scenario
       *      - the consumer embodies a LiveMark
       *      - the producer checks the liveness state before sending
       *      - after some delay, the consumer disengages the LiveMark
       *        and then empties the queue...
       *      - the producer should notice that the receiver is no longer
       *        accessible and should stop sending.
       *      - after some further delay, the receiver verifies that
       *        no further messages were received and that the last
       *        number received is the last number produced
       */
      void verify_multithreadAccess()
        {
            struct Receiver
              {
                using InQueue = boost::lockfree::queue<uint>;
                
                InQueue queue{1000};
                LiveMark mark{*this};
                LiveMark const& getLiveMark() const { return mark; }
              };
            using RecHandle = LiveMark::SafeHandle<Receiver>;

          
          Receiver receiver;
          RecHandle handle = receiver.mark.makeHandle (receiver);
          CHECK (handle);
          
          std::atomic_uint source{0};
          
          Thread testWorker{[&] /* === Receiver of messages === */
                             {
                                sleep_for (100us);
                                
                                // signal to accept no further messages
                                receiver.mark.disengage();
                                sleep_for (100us);
                                
                                // accept all messages sent thus far
                                uint lastNumber{0};
                                receiver.queue.consume_all ([&](uint val){ lastNumber = val; });
                                
                                // verify that the sender indeed stopped...
                                sleep_for (200us);
                                CHECK (receiver.queue.empty());
                                CHECK (lastNumber == source);
                             }};
          
          /* === Producer of messages === */
          
          while (handle)
            {
              ++source;
              handle->queue.push (source);
              yield();
            }
          
          // receiver thread is still running...
          CHECK (testWorker);
          while (testWorker)
            yield();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER(LiveMark_test, "unit common");
  
  
}} // namespace lib::test
