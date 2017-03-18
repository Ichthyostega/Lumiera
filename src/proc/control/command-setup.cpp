/*
  CommandSetup  -  Implementation of command registration and instance management

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


/** @file command-setup.cpp
 ** Implementation details of instance management for command invocation by the GUI.
 ** 
 ** @see command-setup.hpp
 ** @see command-instance-manager.hpp
 ** @see TODO_CommandInstanceManager_test
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/format-string.hpp"
#include "proc/control/command-instance-manager.hpp"

//#include <string>

//using std::string;
//using util::cStr;
//using util::_Fmt;


namespace proc {
namespace control {
  namespace error = lumiera::error;
  
  
  
  namespace { // implementation helper...
  }//(End) implementation helper
  
  
  
  
  /** storage for.... */
  
  
  // emit dtors of embedded objects here....
  CommandInstanceManager::~CommandInstanceManager() { }
  CommandSetup::~CommandSetup()                     { }
  
  CommandInstanceManager::CommandInstanceManager() { }
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace proc::control
