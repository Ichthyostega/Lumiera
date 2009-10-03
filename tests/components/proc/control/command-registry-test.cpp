/*
  CommandRegistry(Test)  -  verify command registration and allocation
 
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
#include "proc/control/command-def.hpp"
#include "proc/control/command-registry.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

#include <tr1/functional>


namespace control {
namespace test    {
  
  
  using std::tr1::function;
  using util::isSameObject;
  using lib::Symbol;
  
  
  namespace { // test data and helpers...
  
    Symbol TEST_CMD  = "test.command1.1";
    Symbol TEST_CMD2 = "test.command1.2";
  }
  
  
  
  /*******************************************************************************
   * @test verify sane behaviour of the relevant operations on the CommandRegistry
   *       interface. Add/remove a command instance to the index, allocate an
   *       CommandImpl frame and verify it is removed properly on ref count zero.
   * @note this test covers the internal bits of functionality,
   *       not the behaviour of the (integrated) command framework 
   * 
   * @see Command
   * @see CommandRegistry
   * @see command.cpp
   * @see command-use1-test.cpp
   */
  class CommandRegistry_test : public Test
    {

      uint cnt_defs;
      uint cnt_inst;

      
      virtual void
      run (Arg) 
        {
          CommandRegistry& registry = CommandRegistry::instance();
          ASSERT (&registry);
          
          cnt_defs = registry.index_size();
          cnt_inst = registry.instance_count();
          
          // prepare a command definition (prototype)
          CommandDef (TEST_CMD)
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind(123);
          
          // this command definition is
          // represented internally by a prototype instance
          ASSERT (++cnt_inst == registry.instance_count());
          ASSERT (++cnt_defs == registry.index_size());
          
          checkRegistration (registry);
          checkAllocation(registry);
          
          ASSERT (cnt_inst == registry.instance_count());
          ASSERT (cnt_defs == registry.index_size());
          
          Command::remove (TEST_CMD);
          ASSERT (--cnt_inst == registry.instance_count());
        }
      
      
      void
      checkRegistration (CommandRegistry& registry)
        {
          ASSERT (cnt_inst == registry.instance_count());
          
          Command cmd1 = registry.queryIndex (TEST_CMD);
          ASSERT (cmd1);
          ASSERT (string(TEST_CMD) == registry.findDefinition(cmd1));
          
          Command nonexistant = registry.queryIndex("miraculous");
          ASSERT (!nonexistant);
          
          // now create a clone, registered under a different ID
          Command cmd2 = cmd1.storeDef(TEST_CMD2);
          ASSERT (cmd2 != cmd1);
          cmd2.bind(54321);
          
          // this created exactly one additional instance allocation:
          ASSERT (1+cnt_inst == registry.instance_count());
          ASSERT (1+cnt_defs == registry.index_size());
          // ...and another index entry
          
          
          Command cmdX = registry.queryIndex(TEST_CMD2);
          ASSERT (cmdX == cmd2);
          ASSERT (cmdX != cmd1);
          
          ASSERT (registry.remove(TEST_CMD2));
          ASSERT (!registry.queryIndex(TEST_CMD2));
          ASSERT (cnt_defs == registry.index_size()); // removed from index
          
          // create a new registration..
          cmdX = registry.track(TEST_CMD, cmd2);  // but "accidentally" use an existing ID
          ASSERT (cmdX == cmd1);                  // Oops, we got the existing registration...
          
          cmdX = registry.track(TEST_CMD2, cmd2);
          ASSERT (registry.queryIndex(TEST_CMD2));
          ASSERT (cmdX == cmd2);
          ASSERT (cmdX != cmd1);
          
          ASSERT (1+cnt_inst == registry.instance_count());
          ASSERT (1+cnt_defs == registry.index_size());
          
          ASSERT (string(TEST_CMD2) == registry.findDefinition(cmdX));
          
          ASSERT ( registry.remove(TEST_CMD2));
          ASSERT (!registry.remove("miraculous"));
          
          ASSERT (!registry.queryIndex(TEST_CMD2));
          ASSERT ( registry.queryIndex(TEST_CMD));
          ASSERT (cnt_defs == registry.index_size());       // the index entry is gone,
          
          ASSERT (1+cnt_inst == registry.instance_count()); // but the allocation still lives
          cmdX.close();
          ASSERT (1+cnt_inst == registry.instance_count());
          cmd2.close();
          ASSERT (0+cnt_inst == registry.instance_count()); // ...as long as it's still referred
        }
      
      
      void
      checkAllocation (CommandRegistry& registry)
        {
          // simulate what normally happens within a CommandDef
          typedef void Sig_oper(int);
          typedef long Sig_capt(int);
          typedef void Sig_undo(int,long);
          
          function<Sig_oper> o_Fun (command1::operate);
          function<Sig_capt> c_Fun (command1::capture);
          function<Sig_undo> u_Fun (command1::undoIt);
          
          ASSERT (o_Fun && c_Fun && u_Fun);
          ASSERT (cnt_inst == registry.instance_count());
          
          // when the CommandDef is complete, it issues the
          // allocation call to the registry behind the scenes....
          
          typedef shared_ptr<CommandImpl> PImpl;
          
          PImpl pImpl = registry.newCommandImpl(o_Fun,c_Fun,u_Fun);
          ASSERT (1+cnt_inst == registry.instance_count());
          
          ASSERT (pImpl);
          ASSERT (pImpl->isValid());
          ASSERT (1 == pImpl.use_count());   // no magic involved, we hold the only instance
          
          PImpl clone = registry.createCloneImpl(*pImpl);
          ASSERT (clone->isValid());
          ASSERT (1 == clone.use_count());
          ASSERT (1 == pImpl.use_count());
          ASSERT (2+cnt_inst == registry.instance_count());
          
          ASSERT (!isSameObject (*pImpl, *clone));
//          ASSERT (*pImpl == *clone);             ///////////////////////////////////////TODO: comparison on CommandImpl ??
          
          ASSERT (!pImpl->canExec());
          typedef Types<int> ArgType;
          TypedArguments<Tuple<ArgType> > arg (Tuple<ArgType>(98765));
          pImpl->setArguments(arg);
          ASSERT (pImpl->canExec());
          
//          ASSERT (*pImpl != *clone);  // this proves the clone has indeed a separate identity
          ASSERT (!clone->canExec());
          
          // discard the first clone and overwrite with a new one
          clone = registry.createCloneImpl(*pImpl);
          ASSERT (2+cnt_inst == registry.instance_count());
//          ASSERT (*pImpl == *clone);
          ASSERT (clone->canExec());
          
          clone.reset();
          pImpl.reset();
          // corresponding allocation slots cleared automatically
          ASSERT (cnt_inst == registry.instance_count());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandRegistry_test, "function controller");
      
      
}} // namespace control::test
