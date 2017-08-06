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
#include "lib/util.hpp"
#include "lib/format-cout.hpp"
#include "lib/scoped-collection.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/sync.hpp"

#include "lib/call-queue.hpp"

#include <functional>
#include <string>



namespace lib {
namespace test{
  
  using lib::Sync;
  using backend::Thread;
  using backend::ThreadJoinable;
  
  using util::isnil;
  using std::string;
  using std::bind;
  
  
  
  namespace { // test fixture
    
    // --------random-stress-test------
    uint const NUM_OF_THREADS = 50;
    uint const MAX_RAND_INCMT = 200;
    uint const MAX_RAND_STEPS = 500;
    uint const MAX_RAND_DELAY = 10000;
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
   *       - multithreaded stress test
   *       
   * @see lib::CallQueue
   * @see gui::NotificationService usage example
   * @see [DemoGuiRoundtrip]: http://issues.lumiera.org/ticket/1099 "Ticket #1099"
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
      
      
      
      using Step = std::function<uint()>;
      
      struct Worker
        : ThreadJoinable
        {
          uint64_t localSum = 0;
          
          Worker(uint cnt, Step workStep)
            : ThreadJoinable{"CallQueue_test: concurrent dispatch"
                            , [&]() {
                                for (uint i=0; i<cnt; ++i)
                                  localSum += workStep();
                            }}
            { }
        };
      
      using Workers = lib::ScopedCollection<Worker>;
      
      
      void
      verify_ThreadSafety()
        {
          CallQueue queue;
          uint64_t globalSum = 0;
          uint64_t checkSum  = 0;
          
          Step step =[&]() -> uint
                        {
                          uint increment = rand() % MAX_RAND_INCMT;
                          uint delay     = rand() % MAX_RAND_DELAY;
                          
                          queue.feed ([&]() { globalSum += increment; });
                          usleep (delay);
                          queue.invoke();  // NOTE: typically this dequeues some other entry added during our sleep
                          return increment;
                        };
          
          uint const cntSteps = rand() % MAX_RAND_STEPS;
          
          // Start a bunch of threads with random access pattern
          Workers workers{NUM_OF_THREADS,
                          [&](Workers::ElementHolder& storage)
                            {
                              storage.create<Worker>(cntSteps, step);
                            }
                         };
          
          // collect the results of all worker threads
          for (auto& worker : workers)
            {
              worker.join();
              checkSum += worker.localSum;
            }
          
          // VERIFY: locally recorded partial sums match total sum
          CHECK (globalSum == checkSum);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CallQueue_test, "unit common");
  
  
}} // namespace lib::test
