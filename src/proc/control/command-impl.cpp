/*
  CommandImpl  -  Proc-Layer command implementation (top level)
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file command-impl.cpp 
 ** //TODO 
 ** 
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "proc/control/command-impl.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

//#include <boost/format.hpp>
//using boost::str;

namespace control {
  


  /** */
  CommandImpl::~CommandImpl() { }
  

  /** */
  void
  CommandImpl::exec (HandlingPattern const& execPattern)
  {
    UNIMPLEMENTED ("actually invoke the command");
  }

  

  
  
  
  
} // namespace control
