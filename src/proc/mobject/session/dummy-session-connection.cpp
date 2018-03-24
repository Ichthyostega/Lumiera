/*
  DummySessionConnection  -  scaffolding placeholder to drive the GUI-Session connection

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

* *****************************************************/


/** @file dummy-session-connection.cpp
 ** Implementation details to build some scaffolding for UI<-->Session integration.
 ** In this translation unit, actually a singleton will be instantiated, whenever
 ** some other part of the application (or some unit test) needs backing by a faked
 ** session with...
 ** - some session content
 ** - commands to be invoked
 ** 
 ** @todo WIP as of 12/2016                         ///////////////////////TICKET #1042
 ** 
 ** @see DummySessionConnection_test
 ** 
 */

#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "proc/mobject/session/dummy-session-connection.hpp"

#include <string>
#include <map>

using std::map;
using std::string;

using util::contains;
using util::isnil;

namespace proc {
namespace mobject {
namespace session {
  
  namespace { //Implementation details....
  
  } //(End)Implementation details....
  
  DummySessionConnection::DummySessionConnection ( )
    {
    }
  
  DummySessionConnection::~DummySessionConnection ( )
    {
      // ////TODO Auto-generated destructor stub
    }
  
  
  /** storage for the Singleton accessor */
  lib::Depend<DummySessionConnection> DummySessionConnection::instance;
  
  
  /**
   * 
   * @param id
   * @return
   */
  
  
}}} // namespace proc::mobject::session
