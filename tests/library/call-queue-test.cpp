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

#include "lib/call-queue.hpp"

#include <functional>
//#include <vector>
#include <string>



namespace lib {
namespace test{
  
  using util::isnil;
  using std::bind;
  using std::string;
  
  
  
  namespace { // test data
  }
  
  
  
  
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
      
      
      void
      verify_Consistency ()
        {
          UNIMPLEMENTED("verify_Consistency");
        }
      
      
      void
      verify_ThreadSafety()
        {
          UNIMPLEMENTED("verify_ThreadSafety");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CallQueue_test, "unit common");
  
  
}} // namespace lib::test
