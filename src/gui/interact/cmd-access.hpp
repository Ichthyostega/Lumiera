/*
  CMD-ACCESSOR.hpp  -  interface to access command invocation services within the UI

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


/** @file cmd-access.hpp
 ** Abstraction: access to command invocation for UI-Elements.
 ** The interact::CmdAccess front-end is used by any UI element about to invoke and dispatch
 ** commands into the session via ProcDispatcher. To invoke a command, typically it is necessary
 ** to prepare a _command instance_ and to pick up and bind _command arguments._ This can be achieved
 ** with the help of an intermediary, known as ["interaction state manager"](\ref InteractionState).
 ** Thus, the command accessor allows to discover a specific InteractionState instance, which
 ** - is responsible for the specific command to be invoked
 ** - can handle context information related to a specific _control system_ (e.g mouse, keyboard,
 **   hardware controller, pen)
 ** - might handle changing contextual state and thus decide if a command can be invoked
 ** From the InteractionState instance, it is possible to retrieve a concrete InvocationTrail for
 ** this specific command instance about to be invoked. This InvocationTrail is an embedded command ID
 ** and can be used, to bind arguments and finally trigger the command invocation.
 ** 
 ** @todo as of 3/2017 this is a early design draft and WIP-WIP-WIP
 ** 
 ** @see TODO___cmd-access-test.cpp
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_CMD_ACCESS_H
#define GUI_INTERACT_CMD_ACCESS_H


#include "lib/error.hpp"
//#include "gui/ctrl/bus-term.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * Abstract foundation of UI state tracking components.
   * @todo write type comment...
   */
  class CmdAccess
    : boost::noncopyable
    {
      
    public:
      ~CmdAccess();  ///< @todo do we need a VTable / virtual dtor?
      
    private:
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_CMD_ACCESS_H*/
