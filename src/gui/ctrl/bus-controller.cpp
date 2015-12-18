/*
  BusController  -  the UI-Bus service

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


/** @file bus-controller.cpp
 ** Implementation of the UI backbone service for messaging.
 ** Any globally relevant widget or controller within the Lumiera UI
 ** is connected to the [UI-Bus][ui-bus.hpp], which is largely implemented
 ** within this compilation unit. [Clients][gui::model::Tangible] typically
 ** use the [BusTerm-Interface][bus-term.hpp] to route generic actions and
 ** receive notifications, state changes and mutations.
 ** 
 ** @note as of 11/2015 this is complete WIP_WIP
 ** @todo WIP  ///////////////////////TICKET #975
 ** 
 ** @see ////TODO_test usage example
 ** @see bus-controller.cpp implementation
 ** 
 */


#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "gui/ctrl/bus-term.hpp"
#include "gui/ctrl/bus-controller.hpp"

//#include <boost/noncopyable.hpp>
#include <string>
#include <map>

using std::map;
using std::string;

using util::contains;
using util::isnil;


namespace gui {
namespace ctrl { 

  namespace { // internal details
    
  } // internal details
  
  
  
  BusTerm::~BusTerm() { }  // emit VTables here... 
  
  
  
  
  /** Builder function: establish and wire a new BusTerm.
   * @param newNode the UI-Element corresponding to and using the new BusTerm
   * @remarks automatically establishes a down-link connection to the given
   *        [Tangible]; the UI-Bus will use this node as target to dispatch
   *        `mark` messages addressed to the new node's ID. It is expected
   *        that the Tangible in turn will use the returned BusTerm for
   *        any up-link communication. Thus, on destruction, the new
   *        BusTerm will detach this UI-Bus connection alltogether.
   */
  BusTerm
  BusTerm::attach (ID identity, Tangible newNode)
  {
    return BusTerm(identity, theBus_.routeAdd(newNode));
  }
  
  
  
  /* ==== standard implementation of the BusTerm interface ==== */
  
  void
  BusTerm::act (GenNode const& command)
  {
    UNIMPLEMENTED("issue command");
  }
  
  
  /** record state mark from this subject */
  void
  BusTerm::note (GenNode const& mark)
  {
    theBus_.note(this->endpointID_, mark);
  }
  
  
  /** */
  void
  BusTerm::note (ID subject, GenNode const& mark)
  {
    UNIMPLEMENTED("forward state mark from subject");
  }
  
  
  /** */
  void
  BusTerm::mark (ID subject, GenNode const& mark)
  {
    UNIMPLEMENTED("send notification or state mark to subject");
  }
  
  
  /**
   * @internal establish new down-link connection form UI-Bus
   * @param node reference to the [Tangible] to be connected.
   * @return corresponding up-link for the initiating node to use
   */
  BusTerm&
  BusTerm::routeAdd (Tangible node)
  {
    return theBus_.routeAdd(node);
  }
  
  
  /**
   * @internal disable down-link connection
   * @remarks corresponding node is about to go away.
   */
  void
  BusTerm::routeDetach(ID node)  noexcept
  {
    theBus_.routeDetach (node);
  }

  
  
}} // namespace gui::ctrl

