/*
  CommandMutation(Test)  -  checking the functor and undo-functor used within Proc-commands
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-mutation.hpp"
#include "lib/meta/tuple.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/util.hpp"

//#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
#include <string>

//using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
//using std::tr1::function;
//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
using std::cout;
using std::endl;


namespace control {
namespace test    {

  using lib::test::showSizeof;

//  using session::test::TestClip;
//  using lumiera::P;
  using lumiera::typelist::Tuple;
  using control::CommandClosure;
  
  
  
  
  
  
  namespace {
    void
    operate (P<Time> dummyObj, int randVal)
    {
      *dummyObj += Time(randVal);
    }
    
    Time
    capture (P<Time> dummyObj, int)
    {
      return *dummyObj;
    }
    
    void
    undoIt (P<Time> dummyObj, int, Time oldVal)
    {
      *dummyObj = oldVal;
    }
  
  }
  
  
  
  /***************************************************************************
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
        }
      
      
      /** @test check the Mutation functor which is bound to our \c testFunc(int) .
       *        Then close this Mutation by providing a parameter tuple.
       *        Verify finally that by invoking the Mutation functor
       *        actually \c testFunc(param) is executed.
       */
      void
      checkMutation ()
        {
          function<void(int)> funky (bind (&testFunc));
          
          Mutation functor (funky);
          ASSERT (!functor);
          VERIFY_ERROR (UNBOUND_ARGUMENTS, functor() );
          
          Tuple<int> param = tuple::make(23);
          Closure close_over (param);
          
          CmdClosure& clo (close_over);
          functor.close(clo);
          ASSERT (functor);
          
          testVal = 0;
          functor();
          ASSERT (testVal == 23);
          functor();
          ASSERT (testVal == 2*23);
        }
      
      
      /** @test check the special Mutation which is used to \em undo a command.
       *        This time, we use our \c testFunc(int) as implementation of the
       *        "undo" function; thus its parameter has now the meaning of an
       *        captured state value. Consequently this time the \em operation
       *        which is to be undone would have the signature \c void(void) .
       *        To carry out the test, we first have to trigger the state
       *        capturing mechanism; after that, invoking the UndoMutation
       *        will call the testFunc with the previously captured state.
       *  @note Mutation and UndoMutation are value objects.
       */
      void
      checkUndoMutation ()
        {
          function<void(int)> undo_func (bind (&testFunc));
          function<int(void)> cap_func  (bind (&capture));
          
          UndoMutation undoFunctor (undo_func,cap_func);
          ASSERT (!undoFunctor);
          VERIFY_ERROR (UNBOUND_ARGUMENTS, undoFunctor() );
          
          Tuple<> param;
          Closure clo (param);
          
          undoFunctor.close(clo);
          ASSERT (!undoFunctor);
          VERIFY_ERROR (MISSING_MEMENTO, undoFunctor() );
          VERIFY_ERROR (MISSING_MEMENTO, undoFunctor.getMemento() );
          
          testVal = 11;
          undoFunctor.captureState();
          ASSERT (undoFunctor);
          ASSERT (testVal == 11);
          
          CmdClosure& mem = undoFunctor.getMemento();
          cout << showSizeof(mem) << endl;
          cout << "saved state: " << mem << endl;
          
          undoFunctor();
          ASSERT (testVal == 11 + 11);
          undoFunctor();
          ASSERT (testVal == 11 + 11 + 11);
          undoFunctor.captureState();
          undoFunctor();
          ASSERT (testVal == 33 + 33);
          testVal = 9;
          undoFunctor();
          ASSERT (testVal == 42);
          
          UndoMutation clonedFunc (undoFunctor);
          ASSERT (&undoFunctor.getMemento() != &clonedFunc.getMemento());
          
          clonedFunc.captureState();
          testVal = 0;
          undoFunctor();
          ASSERT (testVal == 33);
          testVal = 0;
          clonedFunc();
          ASSERT (testVal == 42);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandMutation_test, "unit controller");
      
      
}} // namespace control::test
