/*
  CommandCloneBuilder(Test)  -  verify building an implementation clone

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

/** @file command-clone-builder-test.cpp
 ** unit test \ref CommandCloneBuilder_test
 */


#include "lib/test/run.hpp"
#include "steam/control/command-impl.hpp"
#include "steam/control/command-registry.hpp"
#include "steam/control/argument-erasure.hpp"
#include "steam/control/handling-pattern.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "lib/p.hpp"

#include "steam/control/test-dummy-commands.hpp"


namespace steam {
namespace control {
namespace test    {
  
  using namespace lib::meta;
  
  typedef lib::P<CommandImpl> PCmdImpl;
  
  
  namespace { // test config...
    HandlingPattern::ID TEST_HANDLING_PATTERN = HandlingPattern::DUMMY;
  }
  
  
  
  
  
  
  /****************************************************************************//**
   * @test check creation of a command implementation clone from top level,
   *       without disclosing specific type information about the involved closure.
   *       This includes verifying sane allocation management.
   * @note this test covers a very specific low-level perspective, but on an
   *       integration level, involving TypedAllocationManager, CommandRegistry,
   *       CommandImpl, CmdClosure, StorageHolder, UndoMutation, MementoTie.
   *       Closes: Ticket #298
   * 
   * @see Command
   * @see CommandRegistry
   * @see command.cpp
   * @see command-use1-test.cpp
   */
  class CommandCloneBuilder_test : public Test
    {
      
      
      virtual void
      run (Arg) 
        {
          CommandRegistry& registry = CommandRegistry::instance();
          CHECK (&registry);
          uint cnt_inst = registry.instance_count();
          
          {
            PCmdImpl source = buildTestImplFrame (registry);
            PCmdImpl clone  = registry.createCloneImpl (*source);
            
            verifySeparation (source, clone);
          }
          
          CHECK (cnt_inst == registry.instance_count());
        }
      
      
      
      PCmdImpl
      buildTestImplFrame (CommandRegistry& registry)
        {
          // simulate what normally happens within a CommandDef
          typedef void Sig_oper(int);
          typedef long Sig_capt(int);
          typedef void Sig_undo(int,long);
          
          function<Sig_oper> o_Fun (command1::operate);
          function<Sig_capt> c_Fun (command1::capture);
          function<Sig_undo> u_Fun (command1::undoIt);
          
          CHECK (o_Fun && c_Fun && u_Fun);
          PCmdImpl cmd = registry.newCommandImpl(o_Fun,c_Fun,u_Fun);
          
          // make ready for execution
          bindRandArgument (*cmd);
          CHECK (cmd->canExec());
          return cmd;
        }
      
      
      /** Helper: create random command parameter binding */
      void
      bindRandArgument (CommandImpl& cmd)
        {
          typedef Types<int> ArgType;
          TypedArguments<Tuple<ArgType>> arg (std::make_tuple (rand() % 10000));
          cmd.setArguments (arg);
          CHECK (cmd.canExec());
        }
      
      
      /** @test verify the two command implementation frames are
       *        indeed separate objects without interconnection.
       *        Performing an simulated command execution-undo
       *        cycle on both instances and verify difference.
       */
      void
      verifySeparation (PCmdImpl orig, PCmdImpl copy)
        {
          CHECK (orig && copy);
          CHECK (orig->canExec());
          CHECK (copy->canExec());
          
          
          // prepare for command invocation on implementation level....
          HandlingPattern const& testExec = HandlingPattern::get(TEST_HANDLING_PATTERN);
          command1::check_ = 0;
          
          bindRandArgument (*orig);
          CHECK ( orig->canExec());
          CHECK (!orig->canUndo());
          testExec.exec (*orig, "Execute original");     // EXEC 1
          long state_after_exec1 = command1::check_;
          CHECK (command1::check_ > 0);
          CHECK (orig->canUndo());
          
          CHECK (!copy->canUndo());
          testExec.exec (*copy, "Execute clone");        // EXEC 2
          CHECK (command1::check_ != state_after_exec1);
          CHECK (copy->canUndo());
          
          // invoke UNDO on the clone
          testExec.undo (*copy, "Undo clone");           // UNDO 2
          CHECK (command1::check_ == state_after_exec1);
          
          // invoke UNDO on original
          testExec.undo (*orig, "Undo original");        // UNDO 1
          CHECK (command1::check_ ==0);
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandCloneBuilder_test, "function controller");
  
  
}}} // namespace steam::control::test
