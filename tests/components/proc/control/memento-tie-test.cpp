/*
  MementoTie(Test)  -  check the mechanism for capturing and providing undo-state
 
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
#include "proc/control/memento-tie.hpp"
//#include "lib/meta/typelist.hpp"
#include "lib/meta/tuple.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/util.hpp"

#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
#include <cstdlib>
#include <string>

using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
using std::rand;
using std::cout;
using std::endl;


namespace control {
namespace test    {

  using lib::test::showSizeof;

//  using session::test::TestClip;
//  using lumiera::P;
  using namespace lumiera::typelist;
  using lumiera::typelist::Tuple;
  
  using control::CmdClosure;
  
  
  
  
  
  
  namespace {                                                     /////////////////TODO: use a more interesting function here.....
  
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
  
  
  
  /***************************************************************************
   * Verify the state capturing mechanism (memento), which is used
   * to implement the Undo() functionality for Proc-Layer commands
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
          checkStateCapturingClosure();
        }
      
      
      /** @test check the functionality used to implement UndoMutation:
       *        bind an undo function and a state capturing function
       *        and use the latter to define the special CmdClosure
       *        with the ability hold the memento and bind it into
       *        the relevant parameter of the undo function.
       *        Verify that, after closing the functions, actually
       *        state is captured by each invocation.
       */
      void
      checkStateCapturingClosure ()
        {
          function<void(int)> undo_func  = bind (&testFunc,_1);
          function<int(void)> cap_func   = bind (&capture    );
          
#if false ////////////////////////////////////////////////////////////////////////TODO doesn't compile yet, lots of missing stuff          
          MementoClosure memClo (cap_func);
          CmdFunctor closed_cap_func = memClo.bindArguments (cap_func);
          Tuple<Types<> > param;
          Closure<void()> clo (param);
          cout << "plain param values: " << clo << endl;
          
          Closure<void(int)> extendedClo = memClo.decorate (clo);
          cout << "params including memento storage: " << extendedClo << endl;
          
          CmdFunctor closed_undo_func = extendedClo.bindArguments (undo_func);
          
          VERIFY_ERROR (MISSING_MEMENTO, closed_undo_func() );     // invalid, because no state was captured
          
          int rr (rand() %100);
          
          testVal = rr;
          closed_cap_func();      // invoke state capturing 
          
          cout << "params including memento: " << memClo << endl;
          cout << "captured memento state  : " << extendedClo << endl;
          
          testVal = -10;          // meanwhile "somehow" mutate the state
          
          closed_undo_func();     // invoking the undo() feeds back the memento
          ASSERT (rr == testVal); // which is then restored into the state
          
          // this cycle can be repeated with different state values
          rr = (rand() %100);
          testVal = rr;
          closed_cap_func();      // capture new state
          cout << "params including memento: " << memClo << endl;
                                //   ....note the changed memento!
          testVal = -20;
          closed_undo_func();
          ASSERT (rr == testVal);
#endif          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MementoTie_test, "unit controller");
      
      
}} // namespace control::test
