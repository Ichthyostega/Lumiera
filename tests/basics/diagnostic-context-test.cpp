/*
  DiagnosticContext(Test)  -  verify thread local stack for collecting diagnostics 

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

/** @file diagnostic-context-test.cpp
 ** unit test \ref DiagnosticContext_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "vault/thread-wrapper.hpp"
#include "lib/diagnostic-context.hpp"

#include <cstdlib>
#include <vector>



namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  namespace { // private test setup...
    
                             /* WARNING: memory hungry */
    const uint NUM_THREADS = 75;
    const uint MAX_RAND    = 100*1000;
    
    inline bool
    isOdd (uint val)
    {
      return bool (val % 2);
    }
    
  } // (End) test setup....
  
  using backend::ThreadJoinable;
  using error::LERR_(LOGIC);
  using std::rand;
  
  
  /** 
   * Subject of this test:
   * a thread-local stack of int values
   */
  typedef DiagnosticContext<uint> Marker;
  
  typedef std::vector<uint> VecI;
  
  
  
  
  
  /******************************************************************************//**
   * @test verify a diagnostic facility to collect and access contextual information.
   *       DiagnosticContext frames are placed into automatic storage (as local
   *       variable within some function scope). Based on thread-local storage,
   *       the next reachable frame can be accessed from anywhere within
   *       the callstack. This feature is useful for collecting 
   *       information regarding features cross-cutting
   *       the usual dependency hierarchy.
   * 
   * @see lib::DiagnosticContext
   * @see lib::ThreadLocal
   * @see backend::Thread
   */
  class DiagnosticContext_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_simpleAccess();
          verify_heavilyParallelUsage();
        }
      
      
      
      /** @test create nested scopes and place a
       *  DiagnosticContext frame into each.
       *  Access the next reachable frame.
       */
      void
      verify_simpleAccess()
        {
          VERIFY_ERROR (LOGIC, Marker::access());
          
          VecI loggedValues;
          
          Marker zero(0);
          CHECK (0 == zero);
          CHECK (0 == Marker::access());
          
          { // nested scope 
            CHECK (0 == Marker::access());
            
            Marker one(1);
            CHECK (1 == Marker::access());
            CHECK (1 == one);
            CHECK (0 == zero);
            
            { // nested scope
              CHECK (1 == Marker::access());
              
              Marker two(2);
              CHECK (2 == Marker::access());
              CHECK (2 == two);
              CHECK (1 == one);
              CHECK (0 == zero);
              
              loggedValues = Marker::extractStack();
            }
            CHECK (1 == Marker::access());
          }
          CHECK (0 == Marker::access());
          
          CHECK (3 == loggedValues.size());
          CHECK (2 == loggedValues[0]);
          CHECK (1 == loggedValues[1]);
          CHECK (0 == loggedValues[2]);
        }
      
      
      
      
      /** @test verify the thread local property of ContextStack.
       * Create several threads, each of which processes a sequence
       * of numbers starting from a random initial value down to one.
       * Whenever a simple division by two leads to an odd value, this
       * value is placed onto the ContextStack. At recursion end, we
       * take a snapshot of the full ContextStack and then unwind.
       * Thus the captured numbers must from a decreasing sequence
       * of odd values.
       * @warning this test case seems to cause memory pressure.
       *          When running the test suite with VSize limit 500MB,
       *          we frequently got aborts even with 40 threads.
       *          This is surprising, since all of the lists
       *          generated in the individual threads are
       *          of size below 20 elements. 
       */
      void
      verify_heavilyParallelUsage()
        {
          TestThread testcase[NUM_THREADS]    SIDEEFFECT;
          
          for (uint i=0; i < NUM_THREADS; ++i)
            CHECK (testcase[i].join().isValid() );
        }
      
      
      struct TestThread
        : ThreadJoinable
        {
          TestThread()
            : ThreadJoinable("test diagnostic context stack"
                            ,verifyDiagnosticStack)
            { }
        };
      
        
      /** the actual test operation running in a separate thread */
      static void
      verifyDiagnosticStack()
        {
          uint seed (1 + rand() % MAX_RAND);
          
          VecI sequence = descend (seed);
          
          uint prev = 0;
          for (uint i=0; i < sequence.size(); ++i)
            {
              uint val = sequence[i];
              if (! (isOdd(val) && seed >= val && val > prev ))
                throw error::Fatal ("thread-local diagnostic stack");
              prev = val;
            }
        }
      
      static VecI
      descend (uint current)
        {
          if (current < 2)
            return Marker::extractStack();
          
          usleep (500);
          if (isOdd(current))
            {
              Marker remember(current);
              return descend (current+1);
            }
          else
            return descend (current/2);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (DiagnosticContext_test, "function common");
  
  
  
}} // namespace vault::test
