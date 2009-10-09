/*
  CommandCloneBuilder(Test)  -  verify building an implementation clone
 
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
//#include "lib/test/test-helper.hpp"
//#include "proc/control/command-def.hpp"
#include "proc/control/command-registry.hpp"
#include "proc/control/command-impl.hpp"
#include "proc/control/command-impl-clone-builder.hpp"
#include "proc/control/argument-erasure.hpp"
#include "proc/control/handling-pattern.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "lib/p.hpp"

#include "proc/control/test-dummy-commands.hpp"

//#include <tr1/functional>


namespace control {
namespace test    {
  
  
//  using std::tr1::function;
//  using util::isSameObject;
//  using lib::Symbol;
  using namespace lumiera::typelist;
  
  
  namespace { // test data and helpers...
    
    HandlingPattern::ID TEST_HANDLING_PATTERN = HandlingPattern::DUMMY;
  }
  
  typedef lumiera::P<CommandImpl> PCmdImpl;
  
  
  /********************************************************************************
   * @test check creation of a command implementation clone from top level,
   *       without disclosing specific type information about the involved closure.
   *       This includes verifying sane allocation management.
   * @note this test covers a very specific low-level perspective, but on an
   *       integration level, including TypedAllocationManager, CommandRegistry,
   *       CommandImpl, CmdClosure, ArgumentHolder, UndoMutation, MementoTie.
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
          TypedAllocationManager allo; /////////////////////////////////////////////TODO
          ASSERT (&registry);
          uint cnt_inst = registry.instance_count();
          
          {
            PCmdImpl source = buildTestImplFrame (registry);
////////////////////////////////////////////////////////////////////////////////////TODO            
//          PCmdImpl clone  = registry.createCloneImpl (*source);
            CommandImplCloneBuilder cloneBuilder(allo);
            cloneBuilder.visit (*source);
            PCmdImpl clone  = allo.create<CommandImpl> (*source, cloneBuilder.clonedUndoMutation() 
                                                               , cloneBuilder.clonedClosuere());
////////////////////////////////////////////////////////////////////////////////////TODO        
            
            verifySeparation (source, clone);
          }
          
          ASSERT ( 0 == allo.numSlots<CommandImpl>()); /////////////////////////////TODO
          ASSERT (cnt_inst == registry.instance_count());
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
          
          ASSERT (o_Fun && c_Fun && u_Fun);
          PCmdImpl cmd = registry.newCommandImpl(o_Fun,c_Fun,u_Fun);
          
          // make ready for execution
          bindRandArgument (*cmd);
          ASSERT (cmd->canExec());
          return cmd;
        }
      
      
      /** Helper: create random command parameter binding */
      void
      bindRandArgument (CommandImpl& cmd)
        {
          typedef Types<int> ArgType;
          TypedArguments<Tuple<ArgType> > arg (tuple::make (rand() % 10000));
          cmd.setArguments (arg);
          ASSERT (cmd.canExec());
        }
      
      
      /** @test verify the two command implementation frames are
       *        indeed separate objects without interconnection.
       *        Performing an simulated command execution-undo
       *        cycle on both instances and verify difference.
       */
      void
      verifySeparation (PCmdImpl orig, PCmdImpl copy)
        {
          ASSERT (orig && copy);
          ASSERT (orig->canExec());
          ASSERT (copy->canExec());
          ASSERT (orig == copy);
          
          
          // prepare for command invocation on implementation level....
          HandlingPattern const& testExec = HandlingPattern::get(TEST_HANDLING_PATTERN);
          HandlingPattern const& testUndo = testExec.howtoUNDO();
          command1::check_ = 0;
          
          bindRandArgument (*orig);
          ASSERT ( orig->canExec());
          ASSERT (!orig->canUndo());
          testExec.invoke (*orig, "Execute original");     // EXEC 1
          long state_after_exec1 = command1::check_;
          ASSERT (command1::check_ > 0);
          ASSERT (orig->canUndo());
          ASSERT (orig != copy);
          
          ASSERT (!copy->canUndo());
          testExec.invoke (*copy, "Execute clone");        // EXEC 2
          ASSERT (command1::check_ != state_after_exec1);
//          ASSERT (copy->canUndo());
          ASSERT (copy != orig);
          
          // invoke UNDO on the clone
//          testUndo.invoke (*copy, "Undo clone");           // UNDO 2
//          ASSERT (command1::check_ == state_after_exec1);
          
          // invoke UNDO on original
          testUndo.invoke (*orig, "Undo original");        // UNDO 1
//          ASSERT (command1::check_ ==0);
          
          ASSERT (copy != orig);
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandCloneBuilder_test, "function controller");
  
  
}} // namespace control::test
