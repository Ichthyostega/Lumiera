/*
  Command  -  Key abstraction for proc/edit operations and UNDO management
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


/** @file command.cpp 
 ** //TODO 
 ** 
 ** @see command.hpp
 **
 */


#include "proc/control/command.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

//#include <boost/format.hpp>
//using boost::str;

namespace control {
  
  LUMIERA_ERROR_DEFINE (UNBOUND_ARGUMENTS, "Mutation functor not yet usable, because arguments aren't bound");
  LUMIERA_ERROR_DEFINE (MISSING_MEMENTO, "Undo functor not yet usable, because no undo state has been captured");
  


  /** */
  Command& 
  Command::get (Symbol cmdID)
  {
    UNIMPLEMENTED ("fetch an existing command from the internal cmd registry");
  }
  
  
  MementoClosure::MementoClosure (CmdFunctor&)
  {
    
  }
  

  
  
  
  
} // namespace control
