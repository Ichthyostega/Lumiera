/*
  CommandRegistry(Test)  -  verify command registration and allocation

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

/** @file command-registry-test.cpp
 ** unit test \ref CommandRegistry_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command-def.hpp"
#include "steam/control/command-registry.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include "steam/control/test-dummy-commands.hpp"

#include <functional>


namespace steam {
namespace control {
namespace test    {
  
  
  using std::function;
  using util::isSameObject;
  using lib::Symbol;
  
  
  namespace { // test data and helpers...
  
    Symbol TEST_CMD  = "test.command1.1";
    Symbol TEST_CMD2 = "test.command1.2";
  }
  
  
  
  /***************************************************************************//**
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
          CHECK (&registry);
          
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
          CHECK (++cnt_inst == registry.instance_count());
          CHECK (++cnt_defs == registry.index_size());
          
          checkRegistration (registry);
          checkAllocation(registry);
          
          CHECK (cnt_inst == registry.instance_count());
          CHECK (cnt_defs == registry.index_size());
          
          Command::remove (TEST_CMD);
          CHECK (--cnt_inst == registry.instance_count());
        }
      
      
      
      /** @test verify the index operation.
       *        Add, search, remove, store copy.
       */
      void
      checkRegistration (CommandRegistry& registry)
        {
          CHECK (cnt_inst == registry.instance_count());
          
          Command cmd1 = registry.queryIndex (TEST_CMD);
          CHECK (cmd1);
          CHECK (TEST_CMD == registry.findDefinition(cmd1));
          
          Command nonexistant = registry.queryIndex("miraculous");
          CHECK (!nonexistant);
          
          // now create a clone, registered under a different ID
          Command cmd2 = cmd1.storeDef(TEST_CMD2);
          CHECK (cmd2 != cmd1);     // note: while they are equivalent, they are not identical
          Command cm2x = cmd2.bind(54321);
          CHECK (cm2x != cmd1);
          CHECK (cm2x == cmd2);
          
          // this created exactly one additional instance allocation:
          CHECK (1+cnt_inst == registry.instance_count());
          CHECK (1+cnt_defs == registry.index_size());
          // ...and another index entry
          
          
          Command cmdX = registry.queryIndex(TEST_CMD2);
          CHECK (cmdX == cmd2);
          CHECK (cmdX != cmd1);
          
          CHECK (registry.remove(TEST_CMD2));
          CHECK (!registry.queryIndex(TEST_CMD2));
          CHECK (cnt_defs == registry.index_size());        //  removed from index
          CHECK (1+cnt_inst == registry.instance_count()); //...but still alive
          
          CHECK (cmdX.isAnonymous());
          CHECK (cmd2.isAnonymous());                   //......they got deached
          CHECK (!cmd1.isAnonymous());
          
          // create a new registration..
          registry.track(TEST_CMD2, cmd2);
          CHECK (registry.queryIndex(TEST_CMD2));
          CHECK (1+cnt_defs == registry.index_size()); // again holding two distinct entries
          CHECK (cmdX == cmd2);
          CHECK (cmdX != cmd1);
          
          CHECK (TEST_CMD  == registry.findDefinition(cmd1));
          CHECK (TEST_CMD2 == registry.findDefinition(cmd2));
          CHECK (TEST_CMD2 == registry.findDefinition(cmdX));
          
          CHECK ( registry.remove(TEST_CMD2));
          CHECK (!registry.remove("miraculous"));
          
          CHECK (!registry.queryIndex(TEST_CMD2));
          CHECK ( registry.queryIndex(TEST_CMD));
          CHECK (cnt_defs == registry.index_size());       // the index entry is gone,
          
          CHECK (1+cnt_inst == registry.instance_count()); // but the allocation still lives
          cmdX.close();
          CHECK (1+cnt_inst == registry.instance_count());
          cmd2.close();
          cm2x.close();
          CHECK (0+cnt_inst == registry.instance_count()); // ...as long as it's still referred
        }
      
      
      
      /** @test verify the allocation/de-allocation handling as
       *        embedded into the CommandRegistry operation.
       *        Simulates on low level what normally happens
       *        during command lifecycle.
       */
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
          
          CHECK (o_Fun && c_Fun && u_Fun);
          CHECK (cnt_inst == registry.instance_count());
          
          // when the CommandDef is complete, it issues the
          // allocation call to the registry behind the scenes....
          
          typedef shared_ptr<CommandImpl> PImpl;
          
          PImpl pImpl = registry.newCommandImpl(o_Fun,c_Fun,u_Fun);
          CHECK (1+cnt_inst == registry.instance_count());
          
          CHECK (pImpl);
          CHECK (pImpl->isValid());
          CHECK (!pImpl->canExec());
          CHECK (1 == pImpl.use_count());   // no magic involved, we hold the only instance
          
          PImpl clone = registry.createCloneImpl(*pImpl);
          CHECK (clone->isValid());
          CHECK (!clone->canExec());
          CHECK (1 == clone.use_count());
          CHECK (1 == pImpl.use_count());
          CHECK (2+cnt_inst == registry.instance_count());
          
          CHECK (!isSameObject (*pImpl, *clone));
          CHECK (*pImpl == *clone);
          
          CHECK (!pImpl->canExec());
          typedef Types<int> ArgType;
          TypedArguments<Tuple<ArgType>> arg{Tuple<ArgType>(98765)};
          pImpl->setArguments(arg);
          CHECK (pImpl->canExec());
          
          CHECK (!clone->canExec()); // this proves the clone has indeed a separate identity
          CHECK (*pImpl != *clone);
          
          // discard the first clone and overwrite with a new one
          clone = registry.createCloneImpl(*pImpl);
          CHECK (2+cnt_inst == registry.instance_count());
          CHECK (*pImpl == *clone);
          CHECK (clone->canExec());
          
          clone.reset();
          pImpl.reset();
          // corresponding allocation slots cleared automatically
          CHECK (cnt_inst == registry.instance_count());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandRegistry_test, "function controller");
  
  
}}} // namespace steam::control::test
