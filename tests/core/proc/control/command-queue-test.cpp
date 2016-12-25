/*
  CommandQueue(Test)  -  verify functionality of ProcDispatcher queue component

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "proc/control/command-queue.hpp"
#include "proc/control/command-def.hpp"
//#include "proc/control/command-registry.hpp"
//#include "lib/test/event-log.hpp"
#include "lib/symbol.hpp"

#include "proc/control/test-dummy-commands.hpp"

//#include <cstdlib>


namespace proc {
namespace control {
namespace test    {
  
  
//  using std::function;
//  using std::rand;
  using lib::Symbol;
  
  
  namespace { // test fixture...
    
    const Symbol COMMAND_1{"test.queue.command1"};
    const Symbol COMMAND_3{"test.queue.command3"};
    
  }//(End) test fixture
  
//  typedef shared_ptr<CommandImpl> PCommandImpl;
//  typedef HandlingPattern const& HaPatt;
  
  
  
  
  
  
  /******************************************************************************//**
   * @test verify proper working of queue management used within ProcDispatcher.
   *       - can enqueue and dequeue command messages
   *       - handling of priority messages
   * 
   * @see CommandQueue
   * @see DispatcherLoop
   * @see ProcDispatcher
   * @see DispatcherLooper_test
   */
  class CommandQueue_test : public Test
    {
      
      //------------------FIXTURE
    public:
      CommandQueue_test()
        {
          CommandDef (COMMAND_1)
               .operation (command1::operate)
               .captureUndo (command1::capture)
               .undoOperation (command1::undoIt)
               ;
          CommandDef (COMMAND_3)
               .operation (command3::operate)
               .captureUndo (command3::capture)
               .undoOperation (command3::undoIt)
               ;
        }
     ~CommandQueue_test()
        {
          Command::remove (COMMAND_1);
          Command::remove (COMMAND_3);
        }
      //-------------(End)FIXTURE
      
     
      virtual void
      run (Arg)
        {
          verifyBasics();
        }
      
      
      void
      verifyBasics()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandQueue_test, "unit controller");
  
  
}}} // namespace proc::control::test
