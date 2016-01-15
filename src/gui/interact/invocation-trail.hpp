/*
  INVOCATION-TRAIL.hpp  -  a tangible element of the user interface

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


/** @file invocation-trail.hpp
 ** A command in preparation of being issued from the UI.
 ** The actual persistent operations on the session model are defined
 ** as DSL scripts acting on the session interface, and configured as a
 ** _command prototype_. Typically these need to be enriched with at least
 ** the actual subject to invoke this command on; many commands require
 ** additional parameters, e.g. some time or colour value. These actual
 ** invocation parameters need to be picked up from UI elements, and the
 ** process of preparing and outfitting a generic command with these
 ** actual values is tracked by an [InvocationTrail] handle. When
 ** ready, finally this handle can be issued on any [BusTerm].
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_INVOCATION_TRAIL_H
#define GUI_INTERACT_INVOCATION_TRAIL_H


#include "proc/control/command.hpp"
#include "lib/diff/gen-node.hpp"

#include <utility>
#include <string>


/** @todo unused as of 11/2015
 * some additional instantiation metadata
 * could be passed alongside with the invocation.
 */
#define RESERVED_FOR_FUTURE_USE_ 42


namespace gui {
namespace interact {

  using lib::diff::GenNode;
  using lib::diff::Rec;
  using std::string;
  
  
  /**
   * A concrete command invocation in the state of preparation and argument binding.
   * This value object is a tracking handle used within the UI to deal with establishing
   * a command context, maybe to present the command within a menu or to picking up
   * actual invocation parameters from the context.
   * @remarks typically you don't create an InvocationTrail from scratch; rather
   *          you'll find it embedded into rules placed into a [InteractionStateManager].
   *          The intention is to define it alongside with the command prototype.
   */
  class InvocationTrail
    {
      string cmdID_;
      
    public:
      /**
       * Build a "command-as-prepared-for-UI".
       * @param prototype an _already existing_ command prototype definition within Proc-Layer
       * @remarks we deliberately link InvocationTrail to the existence of an actual prototype.
       *    Invocation trails will be created in advance for various scenarios to invoke commands,
       *    and are in fact lightweight placeholder handles -- so we do not want placeholders to
       *    exist somewhere in the system and IDs to be sent over the bus, without the certainty
       *    of a real invocation site and a matching command operation to exist somewhere else
       *    within the system.
       */
      InvocationTrail(proc::control::Command prototype)
        : cmdID_(prototype.getID())
        { }
      
      GenNode bind (Rec&& cmdArgs)  const
        {
          return GenNode(cmdID_, std::forward<Rec>(cmdArgs));
        }
      
      GenNode bang()  const
        {
          return GenNode(cmdID_, RESERVED_FOR_FUTURE_USE_);
        }
      
      operator string()  const
        {
          return "InvocationTrail cmd(\""+cmdID_+"\"";
        }
      
    private:
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_INVOCATION_TRAIL_H*/
