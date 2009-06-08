/*
  COMMAND-DEF.hpp  -  defining and binding a Proc-Layer command
 
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


/** @file command-def.hpp
 ** Actually defining a command and binding it to execution parameters.
 ** While the header command.hpp contains everything needed for executing and
 ** commands and referring to them, this more heavy-weight header is needed when
 ** \em defining the concrete operations to be encapsulated into a command. To
 ** create a command, you need to provide three functors (for the actual operation,
 ** the undo operation and for capturing undo state prior to invoking the operation).
 ** Besides, you provide a \em binding, thus creating a closue out of these three
 ** function objects and a set of actual parameters. This closure effectively is 
 ** the command, which in a last step can be either dispatched, stored or 
 ** invoked immediately.
 ** //TODO 
 **
 ** @see Command
 ** @see Mutation
 ** @see CommandClosure
 ** @see ProcDispatcher
 ** @see CommandBasic_test simple usage example
 **
 */



#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

//#include "pre.hpp"
#include "proc/control/command.hpp"
#include "proc/control/mutation.hpp"
#include "proc/control/command-closure.hpp"

//#include <tr1/memory>




namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  
  
  /**
   * @todo Type-comment
   */
  class CommandDef
    {
      
    public:
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
