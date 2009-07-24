/*
  HANDLILNG-PATTERN.hpp  -  A skeleton for executing commands, including standard implementations
 
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
 
*/


/** @file handling-pattern.hpp
 ** //TODO 
 **
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_HANDLING_PATTERN_H
#define CONTROL_HANDLING_PATTERN_H

//#include "pre.hpp"
//#include "include/symbol.hpp"

//#include <tr1/memory>



namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  
  
  class Command;
  
  
  /**
   * @todo Type-comment
   */
  class HandlingPattern
    {
      
    public:
      
      virtual ~HandlingPattern() {}
      
      virtual void invoke (Command& command)  const  =0;
      
      virtual HandlingPattern const& howtoUNDO()  const  =0;
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
