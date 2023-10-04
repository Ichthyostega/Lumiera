/*
  CallQueue(Test)  -  verify queue based dispatch of bound function objects

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file call-queue-test.cpp
 ** unit test \ref CallQueue_test
 */


#include "lib/test/run.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/sync.hpp"
#include "lib/util.hpp"

#include "lib/call-queue.hpp"

#include <string>



namespace lib {
namespace test{
  
  using lib::Sync;
  using lib::SyncBarrier;
  using lib::ThreadJoinable;
  
  using util::isnil;
  using std::string;
  
  
  
  namespace { // test fixture
    
    // --------random-stress-test------
    uint const NUM_OF_THREADS = 50;
    uint const MAX_RAND_INCMT = 200;
    uint const MAX_RAND_STEPS = 500;
    uint const MAX_RAND_DELAY = 1000;
    // --------random-stress-test------
    
    
    uint calc_sum = 0;
    uint ctor_sum = 0;
    uint dtor_sum = 0;
    
    template<uint i>
    struct Dummy
      {
        uint val_;
        
        Dummy()
          : val_(i)
          {
            ctor_sum += (val_+1);
          }
        
       ~Dummy()
          {
            dtor_sum += val_;
          }
        
        int
        operator++()
          {
            return ++val_;
          }
      };
    
    template<uint i>
    void
    increment (Dummy<i>&& dummy)  //NOTE: dummy is consumed here
    {
      calc_sum += ++dummy;
    }
    
  }//(End) test fixture
  
  
  
  
  /**********************************************************************************//**
   * @test verify a helper component for dispatching functors through a threadsafe queue.
   *       - simple usage
   *       - enqueue and dequeue several functors
   *       - multithreaded load test
   * @see lib::CallQueue
   * @see stage::NotificationService usage example
   * @see [DemoGuiRoundtrip](http://issues.lumiera.org/ticket/1099 "Ticket #1099")
   */
  class CallQueue_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_SimpleUse();
          verify_Consistency();
          verify_ThreadSafety();
        }
      
      
      void
      verify_SimpleUse ()
        {
          CallQueue queue;
          CHECK (isnil (queue));
          
          int val = 2;
          queue.feed ([&]() { val = -1; });
          CHECK (1 == queue.size());
          CHECK (val == 2);
          
          queue.invoke();
          CHECK (val == -1);
          CHECK (0 == queue.size());
          
          queue.invoke();
          CHECK (0 == queue.size());
        }
      
      
      /**
       * @test consistency of queue data handling
       *       - functors of various types can be mixed
       *       - entries are moved in and out of the queue
       *       - no copying whatsoever happens
       *       - each entry gets invoked
       *       - all entries are invoked in order
       *       - enqueuing and dequeuing can be interspersed
       *       - no memory leaks in bound argument data
       */
      void
      verify_Consistency ()
        {
          calc_sum = 0;
          ctor_sum = 0;
          dtor_sum = 0;
          
          CallQueue queue;
          queue.feed ([]() { increment(Dummy<0>{}); });     //NOTE: each lambda binds a different instantiation of the increment template
          queue.feed ([]() { increment(Dummy<1>{}); });     //      and each invocation closes over an anonymous rvalue instance
          queue.feed ([]() { increment(Dummy<2>{}); });
          
          queue.invoke();
          queue.invoke();
          queue.feed ([]() { increment(Dummy<3>{}); });
          queue.feed ([]() { increment(Dummy<4>{}); });
          
          queue.invoke();
          queue.invoke();
          queue.invoke();
          
          uint expected = (5+1)*5/2;
          CHECK (calc_sum = expected);
          CHECK (ctor_sum = expected);
          CHECK (dtor_sum = expected);
        }
      
      
      
      struct Worker
        : ThreadJoinable<>
        , Sync<>
        {
          uint64_t producerSum = 0;
          uint64_t consumerSum = 0;
          
          SyncBarrier& trigger_;
          
          void
          countConsumerCall (uint increment)
            {
              Lock sync(this);          // NOTE: will be invoked from some random other thread
              consumerSum += increment;
            }
          
          Worker(CallQueue& queue, SyncBarrier& commonTrigger)
            : ThreadJoinable{"CallQueue_test: concurrent dispatch"
                            , [&]() {
                                uint cnt    = rand() % MAX_RAND_STEPS;
                                uint delay  = rand() % MAX_RAND_DELAY;
                                
                                trigger_.sync();            // block until all threads are ready
                                for (uint i=0; i<cnt; ++i)
                                  {
                                    uint increment = rand() % MAX_RAND_INCMT;
                                    queue.feed ([=]() { countConsumerCall(increment); });
                                    producerSum += increment;
                                    usleep (delay);
                                    queue.invoke();  // NOTE: dequeue one functor added during our sleep
                                  }                 //        and thus belonging to some random other thread
                            }}
            , trigger_{commonTrigger}
            { }
        };
      
      using Workers = lib::ScopedCollection<Worker>;
      
      
      /**
       * @test torture the CallQueue by massively multithreaded dispatch
       *       - start #NUM_OF_THREADS (e.g. 50) threads in parallel
       *       - each of those has a randomised execution pattern to
       *         add new functors and dispatch other thread's functors
       */
      void
      verify_ThreadSafety()
        {
          CallQueue queue;
          SyncBarrier trigger{NUM_OF_THREADS + 1};
          
          // Start a bunch of threads with random access pattern
          Workers workers{NUM_OF_THREADS,
                          [&](Workers::ElementHolder& storage)
                            {
                              storage.create<Worker> (queue, trigger);
                            }
                         };
          
          // unleash all worker functions
          trigger.sync();
          
          // wait for termination of all threads and detect possible exceptions
          bool allFine{true};
          for (auto& worker : workers)
            allFine &= worker.join().isValid();
          CHECK (allFine);
          
          // collect the results of all worker threads
          uint64_t globalProducerSum = 0;
          uint64_t globalConsumerSum = 0;
          for (auto& worker : workers)
            {
              globalProducerSum += worker.producerSum;
              globalConsumerSum += worker.consumerSum;
            }
          
          // VERIFY: locally recorded partial sums match total sum
          CHECK (globalProducerSum == globalConsumerSum);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CallQueue_test, "unit common");
  
  
}} // namespace lib::test
