/*
  COMMAND-HANDLER.hpp  -  helper to turn command messages into invocations in Proc

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

*/


/** @file command-handler.hpp
 ** Visitor to process command messages and turn them into command invocations
 ** in Proc-Layer. While the actual operation corresponding to a command is fixed
 ** as a script working on the internal Session interface, the invocation of a
 ** command is often the result of an ongoing user interaction. The invocation
 ** itself is formed like a sentence of spoken language, including some context.
 ** For this reason, several messages can be sent over the [UI-Bus](\ref ui-bus.hpp)
 ** to prepare command invocation and explicate the actual command arguments. The
 ** concept and topology of the UI-Bus allows to send those messages from arbitrary
 ** locations within the UI, just assuming there is a CoreService somewhere to
 ** receive and treat those messages. In fact, parameters need to be extracted
 ** and for the actual invocation, a command handle needs to be passed to the
 ** ProcDispatcher for processing in the session thread. CommandHandler is
 ** a delegate to implement those translation tasks on receipt of a
 ** command related UI-bus message.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 1/2017
 ** 
 ** @see AbstractTangible_test::invokeCommand()
 ** @see gui::test::Nexus::prepareDiagnosticCommandHandler()
 ** 
 */


#ifndef GUI_CTRL_COMMAND_HANDLER_H
#define GUI_CTRL_COMMAND_HANDLER_H


#include "lib/error.hpp"
//#include "include/logging.h"
//#include "lib/idi/entry-id.hpp"
#include "include/session-command-facade.h"
#include "proc/control/command-def.hpp"
//#include "gui/notification-service.hpp"
//#include "gui/ctrl/bus-term.hpp"
//#include "gui/ctrl/nexus.hpp"
//#include "lib/util.hpp"
//#include "gui/model/tangible.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/idi/entry-id.hpp"

#include <boost/noncopyable.hpp>
//#include <string>


namespace gui {
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
  using lib::diff::Rec;
  using lib::diff::GenNode;
  using lib::diff::DataCap;
  using proc::control::SessionCommand;
//  using std::string;
  
  
  /**
   * Visitor to help with processing command related messages on the UI-Bus.
   * Used by CoreService to translate such messages into Command invocation
   * by the ProcDispatcher.
   * @remark we need a Visitor here to deal with the different flavours
   *         of command messages, some of which provide arguments as payload
   */
  class CommandHandler
    : public DataCap::Predicate
    , boost::noncopyable
    {
      GenNode::ID const& commandID_;
      
      bool
      handle (Rec const& bindingArgs) override    ///< the argument binding message
        {
          SessionCommand::facade().bindArg (commandID_, bindingArgs);
          return true;
        }
      
      bool
      handle (int const&) override                ///< the "bang!" message (command invocation)
        {
          SessionCommand::facade().invoke (commandID_);
          return true;
        }
      
    public:
      CommandHandler (GenNode const& commandMsg)
        : commandID_{commandMsg.idi}
        { }
    };
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_COMMAND_HANDLER_H*/
