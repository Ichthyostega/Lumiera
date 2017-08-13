/*
  TEST-NEXUS.hpp  -  fake user interface backbone for test support

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

*/


/** @file test/test-nexus.hpp
 ** A fake UI backbone for investigations and unit testing.
 ** Any relevant element within the Lumiera GTK UI is connected to the [UI-Bus](ui-bus.hpp)
 ** So for testing and investigation we need a white room setup to provide an instrumented
 ** backbone to run any test probes against. The test::Nexus allows to [hook up](\ref testUI())
 ** a generic interface element, to participate in a simulated interface interaction.
 ** 
 ** This class test::Nexus acts as front-end for unit tests, while the actual implementation
 ** of a test rigged mock interface backbone remains an implementation detail. The purpose
 ** of this setup is to capture messages sent from elements operated within a test setup
 ** and directed at "core services" (that is, towards a presentation state manager or
 ** towards the Proc-Layer for command invocation). Test code may then verify the
 ** proper shape and incidence of these messages.
 ** 
 ** @see [abstract-tangible-test.cpp]
 ** @see [BusTerm_test]
 ** 
 */


#ifndef GUI_TEST_TEST_NEXUS_H
#define GUI_TEST_TEST_NEXUS_H


#include "lib/error.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/model/tangible.hpp"
#include "gui/ctrl/state-manager.hpp"
#include "test/placeholder-command.hpp"
#include "lib/test/event-log.hpp"
#include "lib/diff/gen-node.hpp"

#include <boost/noncopyable.hpp>
#include <functional>
#include <string>


namespace gui {
namespace test{
  
  
  /**
   * Mock UI backbone for unit testing.
   * In the absence of a real UI, this simulated [UI-Bus](ui-bus.hpp)
   * can be used to wire a [test probe](\ref MockElm) and address it in unit testing.
   * 
   * @note behind the scenes, this is a singleton. Use the provided
   *       attachment point testUI() in order to wire and hook up new
   *       interface elements. When using or deriving from [MockElm] this
   *       wiring happens automatically within the ctor.
   * @see abstract-tangible-test.cpp
   */
  class Nexus
    : boost::noncopyable
    {
      
    public:
      /** get a connection point to a UI backbone faked for test */
      static ctrl::BusTerm& testUI();
      
      /** kill the given [BusTerm] and implant a dead terminal in place */
      static void zombificate(ctrl::BusTerm&);
      
      static lib::test::EventLog const& getLog();
      static lib::test::EventLog const& startNewLog();
      
      static size_t size();
      
      
      /* == allow to set custom handlers for commands and state changes == */
      
      using CommandHandler   = std::function<void(lib::diff::GenNode const&)>;
      using StateMarkHandler = std::function<void(lib::idi::BareEntryID const&, lib::diff::GenNode const&)>;
      
      static void setCommandHandler (CommandHandler  =CommandHandler());
      static void setStateMarkHandler (StateMarkHandler  =StateMarkHandler());
      
      
      static ctrl::StateManager& useMockStateManager();
      static ctrl::StateManager& getMockStateManager();
      
      
      template<typename...ARGS>
      static Symbol prepareMockCmd();
      
      static bool wasInvoked (Symbol);
      
      template<typename...ARGS>
      static bool wasBound (Symbol, ARGS const& ...args);
      
      template<typename...ARGS>
      static bool wasInvoked (Symbol, ARGS const& ...args);
      
    private:
      static void prepareDiagnosticCommandHandler();
    };
  
  
  
  /**
   * Setup of test fixture: prepare a mocked Proc-Layer command,
   * which accepts arguments with the denoted types.
   * @note this call installs the command mock into the Proc-Layer
   *       command registry, where it remains in place until shutdown.
   *       The can be accessed through the generated command ID. Besides,
   *       this call also installs a command handler into the Test-Nexus,
   *       causing "`act`" messages to be processed and logged.
   * @return the ID of the generated mock command.
   */
  template<typename...ARGS>
  inline Symbol
  Nexus::prepareMockCmd()
  {
    prepareDiagnosticCommandHandler();
    return PlaceholderCommand<ARGS...>::fabricateNewInstance(getLog())
                                       .getID();
  }
  
  
  /**
   * Test predicate: verify by string match
   * that the denoted command was actually bound against the given concrete arguments.
   * Actually, we'll match against the Test-Nexus log, where the processing of the
   * corresponding "bind" message should have logged all parameter values
   * @remarks The difficulty here is that the whole command machinery
   *    was made to work opaque (type-erased), and that the bind message
   *    is also opaque, to allow to send arbitrary binding data. The remedy
   *    is to rely on diff::DataCap's `operator string()`, so we can at least
   *    match with the transport format of the Data. A precise and complete
   *    matching would only possible after actually invoking our probe-command,
   *    since we're controlling the implementation of that probe-command.
   *    Right now (2016) this implementation was deemed adequate
   */
  template<typename...ARGS>
  inline bool
  Nexus::wasBound (Symbol cmd, ARGS const& ...args)
  {
    using lib::diff::DataCap;
    
    return getLog()
            .verifyMatch("TestNexus.+HANDLING Command-Message for .+" +cmd)
            .beforeCall("bind-command").on("TestNexus")
                                       .arg(string(DataCap(args))...);
  }
  
  /**
   * Test predicate: verify actual command invocation by string match on test log.
   * This match ensures that
   * - first the installed command handler processed the '`act`' message
   * - then the installed (mock) command handling pattern triggered invocation
   * - and finally our installed mock command function received the call
   * @remarks again arguments are verified by string match; but now we're looking
   *    on the concrete arguments as seen from within the command `operate` function.
   *    These may differ from the transport values, which are used to initialise
   *    the concrete arguments.
   */
  template<typename...ARGS>
  inline bool
  Nexus::wasInvoked (Symbol cmd, ARGS const& ...args)
  {
    return getLog()
            .verifyMatch("TestNexus.+HANDLING Command-Message for .+" +cmd)
            .beforeCall("exec-command").on("TestNexus").arg(cmd)
            .beforeCall("exec").on("MockHandlingPattern")
            .beforeCall("operate").arg(util::toString(args)...)
            .beforeEvent("TestNexus", "SUCCESS handling "+cmd);
  }
  
  /**
   * Test predicate: verify at least one actual invocation happened for the given commend,
   * without matching any concrete arguments
   */
  inline bool
  Nexus::wasInvoked (Symbol cmd)
  {
    return getLog()
            .verifyMatch("TestNexus.+HANDLING Command-Message for .+" +cmd)
            .beforeCall("exec-command").on("TestNexus").arg(cmd)
            .beforeCall("operate")
            .beforeEvent("TestNexus", "SUCCESS handling "+cmd);
  }
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_TEST_NEXUS_H*/
