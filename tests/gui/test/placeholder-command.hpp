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
 ** or be used to mock some operation expected to happen within Proc-Layer. The test::Nexus
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


#ifndef GUI_TEST_PLACEHOLDER_COMMAND_H
#define GUI_TEST_PLACEHOLDER_COMMAND_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/test/event-log.hpp"
#include "proc/control/command-def.hpp"
#include "lib/format-util.hpp"
//#include "lib/diff/gen-node.hpp"

//#include <boost/noncopyable.hpp>
//#include <functional>
#include <vector>
#include <string>


namespace gui {
namespace test{
  
  using std::string;
  using lib::Symbol;
  
    
  /** place the string persistently in memory.
   * @internal used as workaround for creating command-IDs on the fly
   * @todo temporary workaround, shall be replaced by lib::Symbol implementation ///////////////TICKET #157  maintain symbol table for interned strings
   * @return a C-String marked as lib::Literal, pointing
   *      to the permanent location in heap memory.
   * @see \ref test-nexus.cpp implementation
   */
  Symbol internedString (string&& idString);
  
  
  
  /**
   * Set of stub command operations.
   * This is a typed definition frame with some operations,
   * suitably to be bound into a Proc-Layer command. The actual
   * command "operation" just logs invocation into a statically
   * obtained \ref EventLog Event-Log instance. 
   */
  template<typename...ARGS>
  class PlaceholderCommand
    {
      static lib::test::EventLog log_;
      
      /** @internal ID-string specific for the instance `ARGS` */
      static string
      thisTypeInstance()
        {
          return lib::idi::generateExtendedID<PlaceholderCommand>();
        }
      
      
      /** a dummy command "operation */
      static void
      operate (ARGS ...args)
        {
          log_.call(thisTypeInstance(), "operate", std::forward<ARGS>(args)...);
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
          log_.call(thisTypeInstance(), "undo", memento);
        }
      
      
    public:
      static proc::control::Command
      fabricateNewInstance (lib::test::EventLog const& invocationLog)
        {
          log_ = invocationLog;
          return proc::control::CommandDef(internedString (thisTypeInstance()))
                                 .operation(PlaceholderCommand::operate)
                                 .captureUndo(PlaceholderCommand::capture)
                                 .undoOperation(PlaceholderCommand::undo);

        }
    };
  
  
  template<typename...ARGS>
  lib::test::EventLog  PlaceholderCommand<ARGS...>::log_{"test-dummy-"+thisTypeInstance()};

  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_PLACEHOLDER_COMMAND_H*/
