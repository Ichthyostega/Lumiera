/*
  CommandInstanceManager(Test)  -  verify helper for setup of actual command definitions

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file command-instance-manager-test.cpp
 ** unit test \ref CommandInstanceManager_test
 */


#include "lib/test/run.hpp"
#include "steam/control/test-dummy-commands.hpp"
#include "steam/control/command-instance-manager.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/iter-stack.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <cstdlib>
#include <utility>
#include <string>
#include <deque>


namespace steam {
namespace control {
namespace test {
  
  using lib::Literal;
  using std::string;
  using util::_Fmt;
  using std::move;
  using std::rand;
  
  using lumiera::error::LERR_(LIFECYCLE);
  
  
  
  
  namespace  { // Test fixture....
    
    const Symbol COMMAND_PROTOTYPE  = test_Dummy_command1;
    const string INVOCATION_ID = "CommandInstanceManager_test";
    
    class Fixture
      : public CommandDispatch
      {
        std::deque<Command> queue_;
        
        
        /* == Interface: CommandDispatch == */
        
        void
        clear()  override
          {
            queue_.clear();
          }
        
        void
        enqueue (Command&& cmd)  override
          {
            queue_.emplace_front (move (cmd));
          }
        
      public:
        bool
        contains (Command const& ref)
          {
            return queue_.end()!= std::find_if (queue_.begin()
                                               ,queue_.end()
                                               ,[=](Command const& elm)
                                                    {
                                                      return elm == ref;
                                                    });
          }
        
        void
        invokeAll()
          {
            for (Command& cmd : queue_)
              cmd();
            clear();
          }
      };
  }
  
  
  
  /***********************************************************************//**
   * @test CommandInstanceManager is responsible for providing individual
   *       clone copies from a basic command definition, to be bound with
   *       actual arguments and finally handed over to the SteamDispatcher
   *       for invocation.
   * 
   * @see steam::control::CommandInstanceManager
   */
  class CommandInstanceManager_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_simpleUsage();
          verify_extendedUsage();
          verify_instanceIdentity();
          verify_duplicates();
          verify_lifecycle();
          verify_fallback();
        }
      
      
      /** @test demonstrate the transparent instance generation (»fire and forget«)
       *        - when just specifying a global commandID and arguments, an anonymous
       *          instance will be created on-the-fly, bound and dispatched, without
       *          leaving any traces in the global or local registry
       *        - when dispatching a global commandID, where the corresponding
       *          prototype entry is already fully bound and ready for execution,
       *          likewise an anonymous clone copy is created and dispatched.
       *  @remarks these simplified use cases cover a large fraction of all usages,
       *          and most notably, the internal registry embedded within the
       *          CommandInstanceManager won't be used at all. */
      void
      verify_simpleUsage()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          CHECK (not iManager.contains (COMMAND_PROTOTYPE));
          
          int r1{rand()%1000}, r2{rand()%2000};
          command1::check_ = 0; // commands will add to this on invocation
          
          iManager.bindAndDispatch (COMMAND_PROTOTYPE, Rec{r1});
          CHECK (not iManager.contains (COMMAND_PROTOTYPE));
          
          Command com{COMMAND_PROTOTYPE};
          com.bind(r2);
          CHECK (com.canExec());
          
          iManager.dispatch (COMMAND_PROTOTYPE);
          CHECK (not iManager.contains (COMMAND_PROTOTYPE));
          
          // an anonymous clone instance was dispatched,
          // thus re-binding the arguments won't interfere with execution
          com.bind(-1);
          
          CHECK (command1::check_ == 0);       // nothing invoked yet
          fixture.invokeAll();
          CHECK (command1::check_ == r1 + r2); // both instances were invoked with their specific arguments
          
          // clean-up: we have bound arguments on the global prototype
          com.unbind();
        }
      
      
      /** @test demonstrate the complete command instance usage pattern.*/
      void
      verify_extendedUsage()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          Symbol instanceID = iManager.newInstance (COMMAND_PROTOTYPE, INVOCATION_ID);
          CHECK (iManager.contains (instanceID));
          
          Command cmd = iManager.getInstance (instanceID);
          CHECK (cmd);
          CHECK (not cmd.canExec());
          
          cmd.bind(42);
          CHECK (cmd.canExec());
          
          iManager.dispatch (instanceID);
          CHECK (fixture.contains (cmd));
          CHECK (not iManager.contains (instanceID));
          VERIFY_ERROR (LIFECYCLE, iManager.getInstance (instanceID));
          
          command1::check_ = 0;
          fixture.invokeAll();
          CHECK (command1::check_ == 42); // the dispatched instance was executed
        }
      
      
      
      /** @test relation of command, instanceID and concrete instance
       * The CommandInstanceManager provides the notion of a _current instance,_
       * which can then be used to bind arguments. When done, it will be _dispatched,_
       * and then go through the SteamDispatcher's CommandQueue (in this test, we use
       * just a dummy Fixture, which only enqueues the dispatched commands.
       * 
       * The following notions need to be kept apart
       * - a *command* is the operation _definition_. It is registered with a commandID.
       * - the *instance ID* is a decorated commandID and serves to keep different
       *   usage contexts of the same command (prototype) apart. For each instanceID
       *   there is at any given time maximally _one_ concrete instance "opened"
       * - the *concrete command instance* is what can be bound and executed.
       *   It retains it's own identity, even after being handed over for dispatch.
       * Consequently, a given instance can sit in the dispatcher queue to await
       * invocation, while the next instance for the _same instance ID_ is already
       * opened in the CommandInstanceManager for binding arguments.
       */
      void
      verify_instanceIdentity()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          Symbol i1 = iManager.newInstance (COMMAND_PROTOTYPE, "i1");
          Symbol i2 = iManager.newInstance (COMMAND_PROTOTYPE, "i2");
          
          Command c11 = iManager.getInstance (i1);
          Command c12 = iManager.getInstance (i1);
          CHECK (c11 == c12);
          CHECK (c11.isValid());
          CHECK (not c11.canExec());
          
          int r1{rand()%100}, r2{rand()%200}, r3{rand()%300};
          command1::check_ = 0; // commands will add to this on invocation
          
          c11.bind (r1);
          CHECK (c12.canExec());
          CHECK (c11.canExec());
          
          Command c2 = iManager.getInstance (i2);
          CHECK (c2 != c11);
          CHECK (c2 != c12);
          c2.bind (r2);
          
          CHECK (iManager.contains (i1));
          CHECK (iManager.contains (i2));
          CHECK (not fixture.contains (c11));
          CHECK (not fixture.contains (c12));
          CHECK (not fixture.contains (c2));
          
          iManager.dispatch (i1);
          CHECK (not iManager.contains (i1));
          CHECK (    iManager.contains (i2));
          CHECK (    fixture.contains (c11));
          CHECK (    fixture.contains (c12));
          CHECK (not fixture.contains (c2));
          
          CHECK (command1::check_ == 0);
          
          Symbol i11 = iManager.newInstance (COMMAND_PROTOTYPE, "i1");
          CHECK (i11 == i1);
          CHECK ((const char*)i11 == (const char*) i1);
          
          // but the instances themselves are disjoint
          Command c13 = iManager.getInstance (i1);
          CHECK (c13 != c11);
          CHECK (c13 != c12);
          CHECK (c11.canExec());
          CHECK (not c13.canExec());
          
          c13.bind (r3);
          CHECK (c13.canExec());
          
          CHECK (command1::check_ == 0);
          c12();
          CHECK (command1::check_ == 0+r1);
          
          // even a command still in instance manager can be invoked
          c2();
          CHECK (command1::check_ == 0+r1+r2);
          
          CHECK (    iManager.contains (i1));
          CHECK (    iManager.contains (i2));
          CHECK (    fixture.contains (c11));
          CHECK (    fixture.contains (c12));
          CHECK (not fixture.contains (c2));
          
          iManager.dispatch (i2);
          iManager.dispatch (i11);
          CHECK (not iManager.contains (i1));
          CHECK (not iManager.contains (i2));
          CHECK (    fixture.contains (c11));
          CHECK (    fixture.contains (c12));
          CHECK (    fixture.contains (c13));
          CHECK (    fixture.contains (c2));
          
          // if we continue to hold onto an instance,
          // we can do anything with it. Like re-binding arguments.
          c2.bind (47);
          c2();
          c13();
          c13();
          CHECK (command1::check_ == 0+r1+r2+47+r3+r3);
          
          c11.undo();
          CHECK (command1::check_ == 0);
          c2.undo();
          CHECK (command1::check_ == 0+r1+r2);       // undo() restores the value captured before second invocation of c2()
          c12.undo();                                // c11 and c12 refer to the same instance, which was invoked first
          CHECK (command1::check_ == 0);
        }
      
      
      /** @test there can be only one active "opened" instance
       * The CommandInstanceManager opens (creates) a new instance by cloning from the prototype.
       * Unless this instance is dispatched, it does not allow to open a further instance
       * (for the same instanceID). But of course it allows to open a different instance from
       * the same prototype, but with a different invocationID and hence a different instanceID
       */
      void
      verify_duplicates()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          Symbol i1 = iManager.newInstance (COMMAND_PROTOTYPE, "i1");
          Symbol i2 = iManager.newInstance (COMMAND_PROTOTYPE, "i2");
          
          VERIFY_ERROR (DUPLICATE_COMMAND, iManager.newInstance (COMMAND_PROTOTYPE, "i1"));
          VERIFY_ERROR (DUPLICATE_COMMAND, iManager.newInstance (COMMAND_PROTOTYPE, "i2"));
          
          iManager.bindAndDispatch (i1, {-1}); // bind and dispatch i1, thus i1 is ready for new cycle

          iManager.newInstance (COMMAND_PROTOTYPE, "i1"); // open new cycle for i1
          VERIFY_ERROR (DUPLICATE_COMMAND, iManager.newInstance (COMMAND_PROTOTYPE, "i2"));
          
          CHECK (iManager.getInstance (i1));
          CHECK (iManager.getInstance (i2));
        }
      
      
      /** @test verify sane command lifecycle is enforced
       *        - instance need to be opened (created) prior to access
       *        - can not dispatch an instance not yet created
       *        - can not create new instance before dispatching the existing one
       *        - can not dispatch an instance before binding its arguments
       *        - can not access an instance already dispatched
       */
      void
      verify_lifecycle()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          
          // a manually constructed ID is unknown of course
          Symbol instanceID{COMMAND_PROTOTYPE, INVOCATION_ID};
          VERIFY_ERROR (INVALID_COMMAND, iManager.getInstance (instanceID));
          VERIFY_ERROR (INVALID_COMMAND, iManager.dispatch (instanceID));
          
          Symbol i2 = iManager.newInstance (COMMAND_PROTOTYPE, INVOCATION_ID);
          CHECK (i2 == instanceID);
          CHECK (iManager.getInstance (instanceID));
          
          
          Command cmd = iManager.getInstance (instanceID);
          CHECK (cmd);
          CHECK (not cmd.canExec());
          
          VERIFY_ERROR (UNBOUND_ARGUMENTS, iManager.dispatch (instanceID));
          VERIFY_ERROR (DUPLICATE_COMMAND, iManager.newInstance (COMMAND_PROTOTYPE, INVOCATION_ID));
          CHECK (iManager.contains (instanceID)); // errors have not messed up anything
          
          cmd.bind(23);
          CHECK (cmd.canExec());
          iManager.dispatch (instanceID);
          
          CHECK (not iManager.contains (instanceID));
          VERIFY_ERROR (LIFECYCLE, iManager.getInstance (instanceID));
          VERIFY_ERROR (LIFECYCLE, iManager.dispatch (instanceID));
          CHECK (instanceID == iManager.newInstance (COMMAND_PROTOTYPE, INVOCATION_ID));
        }
      
      
      /** @test the instance manager automatically falls back on globally registered commands,
       *        when the given ID is not and was not known locally */
      void
      verify_fallback()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          
          CHECK (not iManager.contains (COMMAND_PROTOTYPE));
          Command cmd = iManager.getInstance (COMMAND_PROTOTYPE);
          
          CHECK (cmd.isValid());
          CHECK (not cmd.isAnonymous());
          CHECK (cmd == Command::get(COMMAND_PROTOTYPE));
          CHECK (cmd == Command{COMMAND_PROTOTYPE});
          
          cmd.bind(-12);
          CHECK (cmd.canExec());
          CHECK (not fixture.contains(cmd));
          
          iManager.dispatch (COMMAND_PROTOTYPE);
          CHECK (fixture.contains(cmd));   // an equivalent clone was enqueued
          
          command1::check_ = 0;
          fixture.invokeAll();
          CHECK (command1::check_ == -12); // the clone copy was executed
          
          // clean-up: we have bound arguments on the global prototype
          cmd.unbind();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandInstanceManager_test, "unit controller");
  
  
}}} // namespace steam::control::test
