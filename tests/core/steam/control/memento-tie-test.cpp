/*
  MementoTie(Test)  -  check the mechanism for capturing and providing undo-state

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file memento-tie-test.cpp
 ** unit test \ref MementoTie_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/memento-tie.hpp"

#include <functional>
#include <cstdlib>

using std::function;
using std::bind;
using std::rand;


namespace steam   {
namespace control {
namespace test    {
  
  namespace {
    
    int testVal=0;                   ///< used to verify the effect of testFunc
    
    void
    undo (short param, int memento)  ///< simulates "Undo" of the command operation
    {
      testVal += param-memento;
    }
    
    int
    capture (short param)            ///< simulates capturing the undo state
    {
      return testVal+param;
    }
    
    
    /** maximum additional storage maybe wasted due
     *  to alignment of the memento value within MementoTie
     */
    const size_t ALIGNMENT = sizeof(size_t);
  }
  
  using LERR_(MISSING_MEMENTO);
  
  
  
  /*************************************************************************************//**
   * @test Verify the state capturing mechanism (memento), which is used to implement
   * the Undo() functionality for Steam-Layer commands. Bind an undo function and a state
   * capturing function and wire up both to store and retrieve a memento value. 
   * Verify that after closing the functions, actually state is captured by each invocation.
   *       
   * @see  control::Command
   * @see  control::CmdClosure
   * @see  control::UndoMutation
   * @see  command-mutation-test.hpp
   */
  class MementoTie_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          checkStateCapturingMechanism();
        }
      
      
      /// assumed signature of the Command "Operation"
      typedef void OpSIG(short);
      typedef MementoTie<OpSIG,int> MemHolder;
      
      
      void
      checkStateCapturingMechanism ()
        {
          function<void(short,int)> undo_func  = undo;
          function< int(short)>     cap_func   = capture;
          
          MemHolder mementoHolder (undo_func,cap_func);
          
          CHECK (sizeof(MemHolder) <= sizeof(int)                    // storage for the memento
                                     + 2 * sizeof(function<void()>)  // storage for the 2 undecorated functors
                                     + ALIGNMENT);
          
          function<OpSIG> bound_undo_func = mementoHolder.tieUndoFunc();
          function<OpSIG> bound_cap_func  = mementoHolder.tieCaptureFunc();
          
          VERIFY_ERROR (MISSING_MEMENTO, bound_undo_func(123) );
          VERIFY_ERROR (MISSING_MEMENTO, mementoHolder.getState() );
          
          short rr (rand() %100);
          testVal = 0;
          bound_cap_func(rr);     // invoke state capturing 
          
          CHECK (rr == mementoHolder.getState());
          
          testVal = 10;           // meanwhile "somehow" mutate the state
          bound_undo_func(0);     // invoking the undo() feeds back the memento
          CHECK (testVal == 10-rr);
          
          // this cycle can be repeated with different state values
          rr = (rand() %100);
          testVal = rr;
          bound_cap_func(5);      // capture new state
          CHECK (5+rr == mementoHolder.getState());
          
          testVal = -20;
          bound_undo_func(3*rr);
          CHECK (testVal == -20 + 3*rr - (5+rr));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MementoTie_test, "unit controller");
  
  
}}} // namespace steam::control::test
