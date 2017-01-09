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
 ** @see AbstractTangible_test::invokeCommand()
 ** @see gui::test::Nexus::prepareDiagnosticCommandHandler()
 ** 
 */


#ifndef GUI_CTRL_COMMAND_HANDLER_H
#define GUI_CTRL_COMMAND_HANDLER_H


#include "lib/error.hpp"
#include "lib/diff/gen-node.hpp"
#include "include/session-command-facade.h"

#include <boost/noncopyable.hpp>


namespace gui {
namespace ctrl{
  namespace error = lumiera::error;
  
  using lib::diff::Rec;
  using lib::diff::GenNode;
  using lib::diff::DataCap;
  using proc::control::SessionCommand;
  
  
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
      
      /** @todo unimplemented extension of command protocol
       *        to clone a given command definition prototype
       * @param subID additional identifier to extend command-ID
       * @return extended command ID, comprised of the basic ID, as given by
       *        the GenNode::ID of the commandMsg (see CommandHandler(GenNode)),
       *        extended by the subID and some random digits.
       * @throw error::Logic always, not yet implemented        ///////////////////////////////////////////////////TICKET #1058 consider extension of UI-Bus protocol
       */
      bool
      handle (string const& subID) override                ///< the "bang!" message (command invocation)
        {
          throw error::Logic ("Extended Protocol for cloning command prototypes (Ticket #1058)"
                             , error::LUMIERA_ERROR_UNIMPLEMENTED);
        }
      
      
      /** handle command argument binding message */
      bool
      handle (Rec const& bindingArgs) override
        {
          SessionCommand::facade().bindArg (commandID_, bindingArgs);
          return true;
        }
      
      
      /** handle the "bang!" message (trigger invocation) */
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
