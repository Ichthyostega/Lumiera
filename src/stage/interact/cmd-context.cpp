/*
  CmdContext  -  interface to access command context binding services within the UI

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

* *****************************************************/


/** @file cmd-context.cpp
 ** Implementation details of support for context-bound commands.
 ** 
 ** @see TODO___cmd-access-test.cpp
 ** 
 */


//#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "stage/interact/cmd-context.hpp"

//#include <string>
//#include <map>

//using std::map;
//using std::string;

//using util::contains;
//using util::isnil;

namespace stage {
namespace interact {
  
  namespace { // internal details
    
  } // internal details
  
  
  
  Subject::~Subject() { }  // Emit VTable here...
  
  
  /** global static service accessor function.
   * @remarks the implementation of this function taps into the UI-Backbone
   *          to access the InteractionDirector for the context interaction state
   */
  CmdContext&
  CmdContext::of (Symbol cmdID, string ctxID)
  {
    UNIMPLEMENTED ("context-bound commands: tap into the InteractionDirector to access the interaction state");
  }
  
  
  /**
   * @remarks this service is used to resolve command arguments
   *  based on the current state of UI interaction. This can be used
   *  to get e.g. the scope enclosing the element currently in focus
   */
  CmdContext::Resolver::operator LuidH ()
  {
    REQUIRE (this->spec > 0); //note we can access the spec embedded in the Resolver struct
    UNIMPLEMENTED ("forward the resolution operation, probably to InteractionDirector or similar...");
  }
  
  
  /** nonsense */

}} // namespace stage::interact
