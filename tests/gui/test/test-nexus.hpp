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
      
      
      /* == allow to set custom handlers for commands and state changes == */
      
      using CommandHandler   = std::function<void(lib::diff::GenNode const&)>;
      using StateMarkHandler = std::function<void(lib::idi::BareEntryID, lib::diff::GenNode const&)>;
      
      static void setCommandHandler (CommandHandler  =CommandHandler());
      static void setStateMarkHandler (StateMarkHandler  =StateMarkHandler());
      
      
      using Cmd = interact::InvocationTrail;
      
      template<typename...ARGS>
      static Cmd prepareMockCmd();
      
      static bool wasInvoked (Cmd);
      
      template<typename...ARGS>
      static bool wasBound (Cmd, ARGS const& ...args);
      
      template<typename...ARGS>
      static bool wasInvoked (Cmd, ARGS const& ...args);
      
    private:
      static void prepareDiagnosticCommandHandler();
    };
  
  
  
  /**
   * Setup of test fixture: prepare a mocked Proc-Layer command,
   * which accepts arguments with the denoted types.
   * @note this call installs the command mock into the Proc-Layer
   *       command registry, where it remains in place until shutdown.
   *       The command uses a synthetic command ID, which is available
   *       through the returned InvocationTrail. Besides, this call
   *       also installs a command handler into the Test-Nexus,
   *       causing "`act`" messages to be processed and logged.
   * @return InvocationTrail, the UI-representation of a Proc-Layer command.
   *         This can be used to trigger command actions on any model::Tangible.
   */
  template<typename...ARGS>
  inline interact::InvocationTrail
  Nexus::prepareMockCmd()
  {
    prepareDiagnosticCommandHandler();
    return Cmd {PlaceholderCommand<ARGS...>::fabricateNewInstance(getLog())};
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
   *    matching is only possible after our probe-command was actually invoked,
   *    since we're controlling the implementation of that probe-command and
   *    can thus record arbitrary data embedded therein.
   */
  template<typename...ARGS>
  inline bool
  Nexus::wasBound (Cmd cmd, ARGS const& ...args)
  {
    using lib::diff::DataCap;
    
    return getLog()
            .verifyMatch("TestNexus.+HANDLING Command-Message for .+" +cmd.getID())
            .beforeCall("bind-command").on("TestNexus").arg(string(DataCap(args))...);
  }
  
  template<typename...ARGS>
  inline bool
  Nexus::wasInvoked (Cmd cmd, ARGS const& ...args)
  {
    UNIMPLEMENTED("verify the denoted command was indeed invoked and received the given concrete Arguments (String-match)");
  }
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_TEST_NEXUS_H*/
