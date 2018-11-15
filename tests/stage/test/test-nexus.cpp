/*
  test::Nexus  -  implementation base for test user interface backbone

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file test/test-nexus.cpp
 ** Implementation of a fake UI backbone for testing.
 ** This compilation unit provides the actual setup for running a faked
 ** user interface from unit tests. Test code is assumed to access those
 ** features through the [front-end](\ref stage::test::TestNexus), while the
 ** actual implementation instances are placed [as singletons](\ref depend.hpp)
 ** 
 ** This test setup will mostly treat messages similar to the [real UI-Bus hub](\ref nexus.hpp),
 ** with additional [logging](\ref event-log.hpp). Since the TestNexus runs as singleton,
 ** there is a single shared "nexus-log", which can be [accessed](\ref test::Nexus::getLog) or
 ** even [cleared](\ref test::Nexus::startNewLog) through the [static front-end](\ref test::Nexus).
 ** But there is no connection to any [core services](\ref core-service.hpp), so neither
 ** commands nor state marks will be processed in any way. In case the unit tests need to
 ** integrate with or verify these handling operations, we provide the ability to install
 ** custom handler functions.
 ** 
 ** @see abstract-tangible-test.cpp
 ** @see BusTerm_test
 ** 
 */


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/itertools.hpp"
#include "test/test-nexus.hpp"
#include "lib/test/event-log.hpp"
#include "steam/control/command.hpp"
#include "stage/ctrl/nexus.hpp"
#include "stage/ctrl/state-recorder.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/depend.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"

#include <string>
#include <deque>

using std::string;

using lib::Symbol;
using lib::append_all;
using lib::transformIterator;
using lib::diff::Rec;
using lib::diff::GenNode;
using lib::diff::DataCap;
using lib::diff::MutationMessage;
using lib::idi::instanceTypeID;
using lib::test::EventLog;
using stage::ctrl::BusTerm;
using stage::ctrl::StateManager;
using stage::ctrl::StateRecorder;
using steam::control::Command;
using steam::control::CommandImpl;
using steam::control::HandlingPattern;
using util::_Fmt;

namespace stage {
namespace test{
  
  
  namespace { // internal details
    
    using BusHub = stage::ctrl::Nexus;
    
    /**
     * @internal fake interface backbone and unit test rig
     * for simulated command and presentation state handling.
     * This implementation embodies the routing functionality
     * as found in the [real nexus](\ref stage::ctrl::Nexus), and additionally
     * also implements the handler functions of the [stage::ctrl::CoreService].
     * The latter allows us to intercept command invocations and presentation
     * state messages
     */
    class TestNexus
      : public BusHub
      {
        EventLog log_{this};
        
        using CommandHandler = test::Nexus::CommandHandler;
        using StateMarkHandler = test::Nexus::StateMarkHandler;
        
        CommandHandler commandHandler_;
        StateMarkHandler stateMarkHandler_;
        
        
        
        virtual void
        act (GenNode const& command)
          {
            log_.call (this, "act", command);
            commandHandler_(command);
            log_.event("TestNexus", _Fmt("bind and trigger command \"%s\"%s")
                                        % command.idi.getSym()
                                        % command.data.get<Rec>());
          }
        
        virtual void
        note (ID subject, GenNode const& mark)  override
          {
            log_.call (this, "note", subject, mark);
            stateMarkHandler_(subject, mark);
            log_.event("TestNexus", _Fmt("processed note from %s |%s") % subject % mark);
          }
        
        virtual bool
        mark (ID subject, GenNode const& mark)  override
          {
            log_.call(this, "mark", subject, mark);
            if (BusHub::mark (subject, mark))
              {
                log_.event ("TestNexus", _Fmt("delivered mark to %s |%s") % subject % mark);
                return true;
              }
            else
              {
                log_.warn (_Fmt("discarding mark to unknown %s |%s") % subject % mark);
                return false;
              }
          }
        
        virtual size_t
        markAll (GenNode const& mark)  override
          {
            log_.call(this, "markAll", mark);
            log_.event("Broadcast", _Fmt("Broadcast mark(\"%s\"): %s") % mark.idi.getSym() % mark.data);
            size_t cnt = BusHub::markAll (mark);
            log_.event("TestNexus", _Fmt("successfully broadcasted mark to %d terminals") % cnt);
            return cnt;
          }
        
        virtual bool
        change (ID subject, MutationMessage&& diff)  override
          {
            string diffSeqLog = diff.updateDiagnostics();   // snapshot of generated diff sequence
            log_.call (this, "change", subject, diffSeqLog);
            if (BusHub::change (subject, move(diff)))
              {
                log_.event ("TestNexus", _Fmt("applied diff to %s |%s") % subject % diffSeqLog);
                return true;
              }
            else
              {
                log_.warn (_Fmt("disregarding change/diff to unknown %s |%s") % subject % diffSeqLog);
                return false;
              }
          }
        
        virtual BusTerm&
        routeAdd (ID identity, Tangible& newNode)  override
          {
            log_.call (this, "routeAdd", identity, instanceTypeID(&newNode));
            BusHub::routeAdd (identity, newNode);
            log_.event("TestNexus", _Fmt("added route to %s |%s| table-size=%2d")
                                        % identity
                                        % instanceTypeID(&newNode)
                                        % BusHub::size());
            return *this;
          }
        
        virtual void
        routeDetach (ID node)  noexcept override
          {
            log_.call (this, "routeDetach", node);
            BusHub::routeDetach (node);
            log_.event("TestNexus", _Fmt("removed route to %s | table-size=%2d") % node % BusHub::size());
          }
        
        virtual operator string()  const
          {
            return getID().getSym()+"."+instanceTypeID(this);
          }
        
        
      public:
        TestNexus()
          : BusHub(*this, lib::idi::EntryID<TestNexus>("mock-UI"))
          {
            installCommandHandler();
            installStateMarkHandler();
          }
        
        // standard copy operations
        
        
        EventLog&
        getLog()
          {
            return log_;
          }
        
        void
        installCommandHandler (CommandHandler newHandler =CommandHandler())
          {
            if (newHandler)
              commandHandler_ = newHandler;
            else
              commandHandler_ =
                [=](GenNode const& cmd)
                  {
                    log_.warn(_Fmt("NOT handling command-message %s in test-mode") % cmd);
                  };
          }
        
        void
        installStateMarkHandler (StateMarkHandler newHandler =StateMarkHandler())
          {
            if (newHandler)
              stateMarkHandler_ = newHandler;
            else
              stateMarkHandler_ =
                [=](ID subject, GenNode const& mark)
                  {
                    log_.warn(_Fmt("NOT handling state-mark %s passed from %s in test-mode")
                                                            % mark         % subject);
                  };
          }
      };
    
    /** singleton instance of the [TestNexus]
     *  used for rigging unit tests */
    lib::Depend<TestNexus> testNexus;
    
    
    
    
    /**
     * @internal a defunct interface backbone.
     * All UI-Bus operations are implemented NOP, but warning on STDRR
     * and logging the invocation to the internal log of [TestNexus].
     * This allows to set up deceased entities within a test rigged UI.
     */
    class ZombieNexus
      : public BusTerm
      {
        
        EventLog&
        log()
          {
            return testNexus().getLog();
          }
        
        
        
        /* ==== defunct re-implementation of the BusTerm interface ==== */
        
        virtual void
        act (GenNode const& command)
          {
            log().call(this, "act", command);
            log().error ("sent command invocation to ZombieNexus");
            cerr << "Command " << command << " -> ZombieNexus" <<endl;
          }
        
        virtual void
        note (ID subject, GenNode const& mark)  override
          {
            log().call(this, "note", subject, mark);
            log().error ("sent note message to ZombieNexus");
            cerr << "note message "<< mark
                 << " FROM:"       << subject
                 << " -> ZombieNexus" <<endl;
          }
        
        virtual bool
        mark (ID subject, GenNode const& mark)  override
          {
            log().call(this, "mark", subject, mark);
            log().error ("request to deliver mark message via ZombieNexus");
            cerr << "mark message -> ZombieNexus" <<endl;
            return false;
          }
        
        virtual size_t
        markAll (GenNode const& mark)  override
          {
            log().call(this, "markAll", mark);
            log().error ("request to broadcast to all Zombies");
            cerr << "broadcast message -> ZombieNexus" <<endl;
            return 0;
          }
        
        virtual bool
        change (ID subject, MutationMessage&& diff)  override
          {
            log().call(this, "change", subject, diff);
            log().error ("request to apply a diff message via ZombieNexus");
            cerr << "change diff -> ZombieNexus" <<endl;
            return false;
          }
        
        virtual BusTerm&
        routeAdd (ID identity, Tangible& newNode)  override
          {
            log().call(this, "routeAdd", identity, newNode);
            log().error ("attempt to connect against ZombieNexus");
            cerr << "connect("<< identity <<" -> ZombieNexus" <<endl;
            return *this;
          }
        
        virtual void
        routeDetach (ID node)  noexcept override
          {
            log().call(this, "routeDetach", node);
            log().error ("disconnect from ZombieNexus");
            cerr << "disconnect("<< node <<" -> ZombieNexus" <<endl;
          }
        
        virtual operator string()  const
          {
            return getID().getSym()+"."+instanceTypeID(this);
          }
        
        
      public:
        /** fabricate a "dead terminal", marked as deceased, viciously connected to itself.
         * @note intentionally to be sliced right after generation.
         *       All operations on this object are defunct.
         */
        ZombieNexus(string formerID, BusTerm& homeland)
          : BusTerm(lib::idi::EntryID<ZombieNexus>("defunct-"+formerID), homeland)
          { }
        
        explicit
        ZombieNexus()
          : ZombieNexus{"zombieland", *this}
          { }
        
       ~ZombieNexus()
          {
            cerr << this->getID().getSym() << ": Zombies never die" << endl;
          }
      };
    
    
    
    lib::Depend<ZombieNexus> zombieNexus;
    
  }//(End) internal details
  
  
  
  
  /**
   * @return reference to a node of the test UI bus,
   *         which allows to hook up new nodes for test
   */
  ctrl::BusTerm&
  Nexus::testUI()
  {
    return testNexus();
  }
  
  lib::test::EventLog const&
  Nexus::getLog()
  {
    return testNexus().getLog();
  }
  
  lib::test::EventLog const&
  Nexus::startNewLog()
  {
    return testNexus().getLog().clear();
  }
  
  size_t
  Nexus::size()
  {
    return testNexus().size();
  }
  
  
  /**
   * install a closure (custom handler function)
   * to deal with any command invocations encountered
   * in the test-UI-Bus. In the real Lumiera-UI, the UI-Bus
   * is wired with a [core service handler](\ref core-service.hpp),
   * which processes command messages by actually triggering
   * command invocation on the Session within Steam-Layer
   * @note when called without arguments, a default handler
   *       will be installed, which just logs and discards
   *       any command invocation message.
   * @warning when you install a closure from within unit test code,
   *       be sure to re-install the default handler prior to leaving
   *       the definition scope; since the "test nexus" is actually
   *       implemented as singleton, an installed custom handler
   *       will outlive the immediate usage scope, possibly
   *       leading to segfault
   */
  void
  Nexus::setCommandHandler (CommandHandler newHandler)
  {
    testNexus().installCommandHandler (newHandler);
  }
  
  /**
   * similar to the [custom command handler](\ref Nexus::setCommandHandler)
   * this hook allows to install a closure to intercept any
   * "state mark" messages passed over the test-UI-Bus
   */
  void
  Nexus::setStateMarkHandler(StateMarkHandler newHandler)
  {
    testNexus().installStateMarkHandler (newHandler);
  }
  
  
  namespace { // install a diagnostic dummy-command-handler
    
    /**
     * Compact diagnostic dummy command handler.
     * Used as disposable one-way off object.
     * Is handles the "`act`" to bind arguments and trigger execution,
     * and it implements the HandlingPattern interface to receive and
     * invoke the prepared command closure.
     */
    class SimulatedCommandHandler
      : public HandlingPattern
      {
        mutable EventLog log_;
        Command command_;
        
        
        
        /* ==== HandlingPattern - Interface ==== */
        
        void
        performExec (CommandImpl& command)  const override
          {
            log_.call ("MockHandlingPattern", "exec", command);
            command.invokeCapture();
            command.invokeOperation();
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            log_.call ("MockHandlingPattern", "undo", command);
            command.invokeUndo();
          }
        
        bool
        isValid()  const override
          {
            return true;
          }
        
        
      public:
        SimulatedCommandHandler (GenNode const& cmdMsg)
          : log_(Nexus::getLog())
          , command_(retrieveCommand(cmdMsg))
          {
            log_.event("TestNexus", "HANDLING Command-Message for "+string(command_));
            
            Rec const& argData{cmdMsg.data.get<Rec>()};
            log_.call ("TestNexus", "bind-command", enumerate(argData));
            command_.bindArg (argData);
            
            log_.call ("TestNexus", "exec-command", command_);
            if (command_.exec (*this))
              log_.event("TestNexus", "SUCCESS handling "+command_.getID());
            else
              log_.warn(_Fmt("FAILED to handle command-message %s in test-mode") % cmdMsg);
          }
        
      private:
        EventLog::ArgSeq
        enumerate (Rec const& argData)
          {
            EventLog::ArgSeq strings;
            strings.reserve (argData.childSize());
            append_all (transformIterator (childData (argData.scope())
                                          , util::toString<DataCap>)
                       ,strings);
            return strings;
          }
        
        static Command
        retrieveCommand (GenNode const& cmdMsg)
          {
            Symbol cmdID {cmdMsg.idi.getSym().c_str()};
            return Command::get (cmdID);
          }
      };

  }//(End)diagnostic dummy-command-handler
  
  void
  Nexus::prepareDiagnosticCommandHandler()
  {
    testNexus().installCommandHandler(
                [](GenNode const& cmdMsg)
                  {
                    SimulatedCommandHandler{cmdMsg};
                  });
  }
  
  
  
  
  
  
  
  namespace { // install a diagnostic dummy-command-handler
    
    using ID = lib::idi::BareEntryID;
    
    class SimulatedStateManager
      : public StateRecorder
      {
        
      public:
        SimulatedStateManager()
          : StateRecorder{testNexus()}
          { }
        
        using StateManager::clearState;
      };
    
    lib::Depend<SimulatedStateManager> stateManager;
    
  }//(End)diagnostic mock-state-manager
  
  
  /**
   * install a standard handler for state mark messages,
   * which is actually backed by a mock implementation of the
   * PresentationStateManager interface. This mock is based on
   * the same implementation techniques as the full fledged
   * state manager in the Lumiera GTK UI; any state mark
   * notification messages appearing after that point
   * at the test-UI-Bus will be accounted for.
   */
  ctrl::StateManager&
  Nexus::useMockStateManager()
  {
    // discard possible leftover
    // from previous test installations
    stateManager().clearState();
    
    testNexus().installStateMarkHandler(
                [&](ID const& elementID, lib::diff::GenNode const& stateMark)
                  {
                    stateManager().recordState (elementID, stateMark);
                  });
    
    return getMockStateManager();
  }
  
  ctrl::StateManager&
  Nexus::getMockStateManager()
  {
    return stateManager();
  }
  
  
  
  
  
  
  /**
   * @return a defunct BusTerm with up-link to [ZombieNexus]
   * @remarks useful to create zombie mock UI-Elements for testing.
   */
  void
  Nexus::zombificate (BusTerm& doomed)
  {
    string lateName = doomed.getID().getSym();
    doomed.~BusTerm();
    testNexus().getLog().destroy (lateName);
    
    static_assert (sizeof(BusTerm) >= sizeof(ZombieNexus), "Zombie overflow");
    new(&doomed) ZombieNexus{lateName, zombieNexus()};
    testNexus().getLog().event(lateName + " successfully zombificated.");
  }

}} // namespace stage::test
