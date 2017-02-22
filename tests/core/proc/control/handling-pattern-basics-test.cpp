/*
  HandlingPatternBasics(Test)  -  verify elementary operation of a command handling pattern

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

/** @file handling-pattern-basics-test.cpp
 ** unit test §§TODO§§
 */


#include "lib/test/run.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-impl.hpp"
#include "proc/control/command-registry.hpp"
#include "proc/control/argument-erasure.hpp"
#include "proc/control/handling-pattern.hpp"
#include "lib/test/event-log.hpp"

#include "proc/control/test-dummy-commands.hpp"

#include <cstdlib>


namespace proc {
namespace control {
namespace test    {
  
  
  using std::function;
  using std::rand;
  
  
  namespace { // test fixture...
    
    string TEST_CMD  = "test.command1.handling";
    HandlingPattern::ID TEST_PATTERN = HandlingPattern::DUMMY;
    
    
    class CustomHandler
      : public HandlingPattern
      {
        mutable
        lib::test::EventLog log_{"custom command handler"};
        
        
        /* ==== HandlingPattern - Interface ==== */
        
        void
        performExec (CommandImpl& command)  const override
          {
            log_.call (TEST_CMD, "exec");
            command.invokeCapture();
            command.invokeOperation();
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            log_.call (TEST_CMD, "undo");
            command.invokeUndo();
          }
        
        bool
        isValid()  const override
          {
            return true;
          }
        
        
      public:
        bool
        invokedExec()
          {
            return log_.verifyCall("exec").on(TEST_CMD);
          }
        
        bool
        invokedUndo()
          {
            return log_.verifyCall("undo").on(TEST_CMD)
                       .afterCall("exec");
          }
      };
    
  }//(End) test fixture
  
  typedef shared_ptr<CommandImpl> PCommandImpl;
  typedef HandlingPattern const& HaPatt;
  
  
  
  
  
  
  /******************************************************************************//**
   * @test operate and verify a simple dummy command handling pattern.
   * @note this test covers mainly the behaviour of a handling pattern as a concept,
   *       not so much the behaviour of the (standard) handling pattern implementations.
   * 
   * @see HandlingPattern
   * @see BasicHandlingPattern
   * @see command.hpp
   * @see command-basic-test.cpp
   */
  class HandlingPatternBasics_test : public Test
    {
      
      uint cnt_inst;
      
      
      virtual void
      run (Arg)
        {
          CommandRegistry& registry = CommandRegistry::instance();
          CHECK (&registry);
          
          cnt_inst = registry.instance_count();
          
          {
            PCommandImpl pCom = buildTestCommand(registry);
            checkExec (pCom);
            checkUndo (pCom);
            
            useCustomHandler (pCom);
          }
          
          CHECK (cnt_inst == registry.instance_count());
        }
      
      
      /** create a command implementation frame usable for tests.
       *  This simulates what normally happens within a CommandDef.
       *  The created CommandImpl isn't registered, and thus will
       *  just go away when the smart-ptr leaves scope.
       */
      PCommandImpl
      buildTestCommand (CommandRegistry& registry)
        {
          
          typedef void Sig_oper(int);
          typedef long Sig_capt(int);
          typedef void Sig_undo(int,long);
          
          function<Sig_oper> o_Fun (command1::operate);
          function<Sig_capt> c_Fun (command1::capture);
          function<Sig_undo> u_Fun (command1::undoIt);
          
          CHECK (o_Fun && c_Fun && u_Fun);
          
          // when the CommandDef is complete, it issues the
          // allocation call to the registry behind the scenes....
          
          PCommandImpl pImpl = registry.newCommandImpl(o_Fun,c_Fun,u_Fun);
          CHECK (pImpl);
          CHECK (*pImpl);
          return pImpl;
        }
      
      
      void
      checkExec (PCommandImpl com)
        {
          CHECK (com);
          CHECK (!com->canExec());
          
          typedef Types<int> ArgType;
          const int ARGU (1 + rand() % 1000);
          Tuple<ArgType> tuple(ARGU);
          TypedArguments<Tuple<ArgType> > arg(tuple);
          com->setArguments(arg);
          
          CHECK (com->canExec());
          CHECK (!com->canUndo());
          command1::check_ = 0;
          
          HaPatt patt = HandlingPattern::get(TEST_PATTERN);
          ExecResult res = patt.exec (*com, TEST_CMD);
          
          CHECK (res);
          CHECK (ARGU == command1::check_);
          CHECK (com->canUndo());
        }
      
      
      void
      checkUndo (PCommandImpl com)
        {
          CHECK (com);
          CHECK (com->canExec());
          CHECK (com->canUndo());
          
          CHECK (command1::check_ > 0);
          
          HaPatt ePatt = HandlingPattern::get(TEST_PATTERN);
          ExecResult res = ePatt.undo (*com, TEST_CMD);
          
          CHECK (res);
          CHECK (command1::check_ == 0);
        }
      
      
      void
      useCustomHandler (PCommandImpl com)
        {
          CustomHandler specialHandler;
          
          CHECK (com->canExec());
          CHECK (not specialHandler.invokedExec());
          
          specialHandler.exec (*com, TEST_CMD);
          CHECK (    specialHandler.invokedExec());
          CHECK (not specialHandler.invokedUndo());
          
          specialHandler.undo (*com, TEST_CMD);
          CHECK (    specialHandler.invokedExec());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HandlingPatternBasics_test, "function controller");
  
  
}}} // namespace proc::control::test
