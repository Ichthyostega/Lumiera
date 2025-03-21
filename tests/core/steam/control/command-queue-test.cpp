/*
  CommandQueue(Test)  -  verify functionality of SteamDispatcher queue component

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


#include "lib/test/run.hpp"
#include "steam/control/command-queue.hpp"
#include "steam/control/command-def.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include "steam/control/test-dummy-commands.hpp"


namespace steam  {
namespace control{
namespace test   {
  
  using lib::Symbol;
  using util::isnil;
  using LERR_(UNBOUND_ARGUMENTS);
  
  
  namespace { // test fixture...
    
    const Symbol COMMAND_1{"test.queue.command1"};
    const Symbol COMMAND_3{"test.queue.command3"};
    
  }//(End) test fixture
  
  
  
  
  /******************************************************************************//**
   * @test verify proper working of queue management used within SteamDispatcher.
   *       - can enqueue and dequeue command messages
   *       - handling of priority messages
   * @see CommandQueue
   * @see DispatcherLoop
   * @see SteamDispatcher
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
          verifyExecutabilityCheck();
        }
      
      
      void
      verifyBasics()
        {
          Command com11 = Command(COMMAND_1).newInstance();
          Command com12 = Command(COMMAND_1).newInstance();
          
          com11.bind(42);
          com12.bind(47);
          
          CommandQueue queue;
          CHECK (isnil(queue));
          
          queue.feed (Command{com11});
          queue.feed (Command{com12});
          
          CHECK (2 == queue.size());
          
          Command x = queue.pop();
          CHECK (1 == queue.size());
          CHECK (x == com11);
          
          queue.clear();
          CHECK (0 == queue.size());
          CHECK (queue.empty());
        }
      
      
      void
      verifyExecutabilityCheck()
        {
          Command com11 = Command(COMMAND_1).newInstance();
          Command com12 = Command(COMMAND_1).newInstance();
          
          com11.bind(42);
          // NOT binding the second command...
          
          CommandQueue queue;
          queue.feed (Command{com11});
          CHECK (1 == queue.size());
          
          VERIFY_ERROR (UNBOUND_ARGUMENTS, queue.feed (Command{com12}));
          CHECK (1 == queue.size());
          
          queue.pop().execSync();
          VERIFY_ERROR (UNBOUND_ARGUMENTS, com12.execSync());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandQueue_test, "unit controller");
  
  
}}} // namespace steam::control::test
