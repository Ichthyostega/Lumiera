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
  
  
  
  
  /** Builder function: establish and wire
   *  a new BusTerm as connected to this one */
  BusTerm&&
  BusTerm::attach (ID newAddress)
  {
    return std::move( BusTerm(newAddress, *this));
  }
  
  
  /* ==== standard implementation of the BusTerm interface ==== */
  
  void
  BusTerm::act (GenNode command)
  {
    UNIMPLEMENTED("issue command");
  }
  
  
  /** */
  void
  BusTerm::note (GenNode mark)
  {
    UNIMPLEMENTED("send notification downstream");
  }
  
  
  /** */
  void
  BusTerm::note (ID subject, GenNode mark)
  {
    UNIMPLEMENTED("send notification to subject");
  }
  
  
  /** */
  void
  BusTerm::mark (GenNode mark)
  {
    UNIMPLEMENTED("record state mark");
  }
  
  
  /** */
  void
  BusTerm::mark (ID subject, GenNode mark)
  {
    UNIMPLEMENTED("forward state mark from subject");
  }
  
  
}} // namespace gui::ctrl

