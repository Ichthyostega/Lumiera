/*
  CommandMutation(Test)  -  checking the functor and undo-functor used within Proc-commands

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "proc/control/memento-tie.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/tuple.hpp"

#include <functional>
#include <iostream>
#include <cstdlib>
#include <string>

using std::function;
using std::string;
using std::rand;
using std::cout;
using std::endl;


namespace proc {
namespace control {
namespace test    {
  
  using namespace lib::meta;
  using control::CmdClosure;
  
  
  namespace { /* ======= test functions to bind ========= */
    
    int testVal=0;  ///< used to verify the effect of testFunc
    
    void
    testFunc (int val)
    {
      testVal += val;
    }
    
    int
    capture ()
    {
      return testVal;
    }
  
  }
  
  
  
  /***********************************************************************//**
   * Verify the behaviour of the type erased closure, which is used
   * by Proc-Layer commands to implement the capturing and later
   * re-invocation of a function.
   *       
   * @see  control::Command
   * @see  control::CommandDef
   * @see  control::Mutation
   * @see  control::UndoMutation
   * @see  command-basic-test.hpp
   */
  class CommandMutation_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          checkMutation();
          checkUndoMutation();
          checkStateCapturingMechanism();
        }
      
      
      /** @test check the Mutation functor which is bound to our \c testFunc(int) .
       *        Then create a argument closure and use this to invoke the Mutation
       *        and verify actually \c testFunc(param) is executed.
       */
      void
      checkMutation ()
        {
          typedef void SIG_fun(int);
          function<SIG_fun> funky  = testFunc;
          
          Mutation functor (funky);
          
          MissingArguments<SIG_fun> nullClosure;
          CHECK (!nullClosure);
          cout << "empty placeholder closure: " << nullClosure << endl;
          VERIFY_ERROR (UNBOUND_ARGUMENTS, functor(nullClosure) );
          
          // now create a real closure....
          Tuple<Types<int> > param = tuple::make(23);
          Closure<void(int)> close_over (param);
          
          CmdClosure& closure (close_over);
          CHECK (closure);
          
          cout << "param values: " << closure << endl;
          
          testVal = 0;
          functor(closure);
          CHECK (testVal == 23);
          functor(closure);
          CHECK (testVal == 2*23);
        }
      
      
      /** @test check the special Mutation which is used to \em undo a command.
       *        This time, we use our \c testFunc(int) as implementation of the
       *        "undo" function; thus its parameter has now the meaning of an
       *        captured state value. Consequently this time the \em operation
       *        which is to be undone would have the signature \c void(void) .
       *        Obviously this is a rather silly "undo" function, but it is
       *        easy to check for unit testing. To carry out this test, we
       *        first have to trigger the state capturing mechanism; after that,
       *        invoking the UndoMutation will call the testFunc with the
       *        previously captured state.
       *  @note Mutation and UndoMutation are value objects, but they refer
       *        to a common command state, which for this test is modelled
       *        by local variables and which for the real commands is 
       *        contained in a CommandArgumentHolder
       */
      void
      checkUndoMutation ()
        {
          function<void(int)> undo_func  = testFunc;
          function<int(void)> cap_func   = capture;
          
          typedef MementoTie<void(),int> MemHolder;
          
          MemHolder mementoHolder (undo_func,cap_func);
          UndoMutation undoFunctor (mementoHolder);
          CHECK (!mementoHolder);
          
          MissingArguments<void(void)> nullClosure;
          VERIFY_ERROR (UNBOUND_ARGUMENTS, undoFunctor(nullClosure) );
          VERIFY_ERROR (UNBOUND_ARGUMENTS, undoFunctor.captureState(nullClosure) );
          
          Tuple<Types<> > param;
          Closure<void()> clo (param);
          
          CHECK (!mementoHolder);
          VERIFY_ERROR (MISSING_MEMENTO, undoFunctor (clo) );
          VERIFY_ERROR (MISSING_MEMENTO, mementoHolder.getState() );
          
          testVal = 11;
          undoFunctor.captureState(clo);
          CHECK (mementoHolder);
          CHECK (testVal == 11);
          
          int mem = mementoHolder.getState();
          cout << "saved state: " << mem << endl;
          
          undoFunctor(clo);
          CHECK (testVal == 11 + 11);
          undoFunctor(clo);
          CHECK (testVal == 11 + 11 + 11);
          undoFunctor.captureState(clo);
          CHECK (33 == mementoHolder.getState());
          undoFunctor(clo);
          CHECK (testVal == 33 + 33);
          testVal = 9;
          undoFunctor(clo);
          CHECK (testVal == 42);
        }
      
      
      /** @test check the undo memento capturing mechanism in isolation 
       *  @see memento-tie-test.cpp more in-depth coverage */
      void
      checkStateCapturingMechanism ()
        {
          typedef MementoTie<void(),int> MemHolder;
          
          MemHolder mementoHolder (testFunc, capture);
          
          function<void()> bound_undo_func = mementoHolder.tieUndoFunc();
          function<void()> bound_cap_func  = mementoHolder.tieCaptureFunc();
          
          
          int rr (rand() % 100);
          testVal = rr;
          bound_cap_func();       // invoke state capturing 
          CHECK (rr == mementoHolder.getState());
          
          testVal = 10;        // meanwhile "somehow" mutate the state
          bound_undo_func();  // invoking the undo() feeds back the memento
          CHECK (testVal == 10+rr);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandMutation_test, "unit controller");
  
  
}}} // namespace proc::control::test
