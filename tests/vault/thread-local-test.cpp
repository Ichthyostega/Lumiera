/*
  ThreadLocal(Test)  -  verify wrapper for using thread-local data 

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file thread-local-test.cpp
 ** unit test \ref ThreadLocal_test
 */


#include "lib/test/run.hpp"

#include "vault/thread-wrapper.hpp"
#include "lib/thread-local.hpp"

#include <cstdlib>

using test::Test;
using backend::ThreadJoinable;
using std::rand;


namespace lib {
namespace test{

  namespace { // private test setup...
    
    const uint NUM_THREADS = 50;
    const uint MAX_RAND    = 5*1000*1000;
    
    
    /** Subject of the test */
    ThreadLocalPtr<uint> privateValue;
    
    
    
    
    struct TestThread
      : ThreadJoinable
      {
        TestThread()
          : ThreadJoinable("test Thread-local storage"
                          ,verifyThreadLocal)
          { }
        
        
        /** the actual test operation running in a separate thread */
        static void
        verifyThreadLocal()
          {
            uint secret (1 + rand() % MAX_RAND);
            privateValue.set (&secret);
            
            usleep (secret); // sleep for a random period
            
            if (secret != *privateValue)
              throw error::Fatal ("thread-local value access broken");
          }
      };
    
  } // (End) test setup....
  
  
  
  
  
  
  
  
  
  
  /**********************************************************************//**
   * @test use a wrapper to simplify handling of thread-local data.
   *       Create some threads, each referring to another piece of data
   *       through the "same" wrapper instance.
   * 
   * @see backend::Thread
   * @see lib::ThreadLocal
   */
  class ThreadLocal_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          TestThread testcase[NUM_THREADS]    SIDEEFFECT;
          
          for (uint i=0; i < NUM_THREADS; ++i)
            CHECK (testcase[i].join().isValid() );
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ThreadLocal_test, "function common");
  
  
  
}} // namespace backend::test
