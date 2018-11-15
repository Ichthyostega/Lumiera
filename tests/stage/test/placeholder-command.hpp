/*
  PLACEHOLDER-COMMAND.hpp  -  fake operation to mock command invocation from UI

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file placeholder-command.hpp
 ** Generate fake commands with stub operations and the ability to verify invocation.
 ** This helper for unit testing of UI interactions might serve as dummy placeholder,
 ** or be used to mock some operation expected to happen within Steam-Layer. The test::Nexus
 ** offers a convenience front-end to install such an placeholder operation and use it
 ** as counterpart for some tested elements connected to the UI-bus.
 ** 
 ** The actual operation is void of any functionality, but might be installed to accept
 ** arbitrary predetermined argument bindings, and to verify invocation and passed arguments.
 ** 
 ** @see BusTerm_test
 ** @see test::Nexus::prepareMockCmd
 ** 
 */


#ifndef STAGE_TEST_PLACEHOLDER_COMMAND_H
#define STAGE_TEST_PLACEHOLDER_COMMAND_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/test/event-log.hpp"
#include "steam/control/command-def.hpp"
#include "lib/format-util.hpp"

#include <vector>
#include <string>


namespace stage {
namespace test{
  
  using std::string;
  using lib::Symbol;
  
    
  
  
  /**
   * Set of stub command operations.
   * This is a typed definition frame with some operations,
   * suitably to be bound into a Steam-Layer command. The actual
   * command "operation" just logs invocation into a statically
   * obtained [Event-Log](\ref EventLog) instance.
   * @note by design, in Lumiera the actual command functions are stateless.
   *       For this reason, the command framework provides no way to associate
   *       an _instance_ of some kind of "command object" with the (conceptual)
   *       command binding or prototype, thus we're unable to log anything
   *       beyond the concrete argument types and values on invocation.
   */
  template<typename...ARGS>
  class PlaceholderCommand
    {
      static lib::test::EventLog log_;
      
      /** @internal unique ID-string specific for the instance `ARGS` */
      static string
      uniqueTypeInstance()
        {
          return lib::idi::generateExtendedID<PlaceholderCommand>();
        }
      
      static string
      fullTypeID()
        {
          return lib::idi::typeFullID<PlaceholderCommand>();
        }
      
      
      /** a dummy command "operation */
      static void
      operate (ARGS ...args)
        {
          log_.call(fullTypeID(), "operate", std::forward<ARGS>(args)...);
        }
      
      static string
      capture (ARGS ...args)
        {
          using VecS = std::vector<string>;
          return "Memento⧏" + util::join (util::stringify<VecS> (args...),"⧓") + "⧐";
        }
      
      static void
      undo (ARGS ..., string memento)
        {
          log_.call(fullTypeID(), "undo", memento);
        }
      
      
    public:
      static steam::control::Command
      fabricateNewInstance (lib::test::EventLog const& invocationLog)
        {
          log_ = invocationLog;
          return steam::control::CommandDef{Symbol{uniqueTypeInstance()}}
                                 .operation(PlaceholderCommand::operate)
                                 .captureUndo(PlaceholderCommand::capture)
                                 .undoOperation(PlaceholderCommand::undo);

        }
    };
  
  
  template<typename...ARGS>
  lib::test::EventLog  PlaceholderCommand<ARGS...>::log_{"test-dummy-"+fullTypeID()};

  
  
  
}} // namespace stage::test
#endif /*STAGE_TEST_PLACEHOLDER_COMMAND_H*/
