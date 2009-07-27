/*
  COMMAND-IMPL.hpp  -  Proc-Layer command implementation (top level)
 
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


/** @file command-impl.hpp
 ** Top level of the command implementation. CommandImpl holds together
 ** the various data and sub-objects involved into the inner workings of a
 ** Proc-Layer command. It serves to implement a "command definition" (prototype)
 ** as well as a concrete command instance. It is a data holder with a well defined
 ** identity and usually located within the (pooled) storage managed by the
 ** CommandRegistry. Client code gets access to a specific CommandImpl through
 ** a Command instance, which is a small (refcounting smart-ptr) handle.
 ** 
 ** //TODO 
 **
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_COMMAND_IMPL_H
#define CONTROL_COMMAND_IMPL_H

#include "proc/control/command.hpp"

//#include <tr1/memory>



namespace control {
  

  
  
  /**
   * @todo Type-comment
   */
  class CommandImpl
    {
      
    public:
      /* === command registry === */
      
      
     ~CommandImpl();
      
      
      /** core operation: invoke the command
       *  @param execPattern describes the individual steps
       *         necessary to get this command invoked properly
       */
      void exec (HandlingPattern const& execPattern);
      
      
      template<typename TYPES>
      void bindArg (Tuple<TYPES> const&);
      
      
      /* === diagnostics === */
      
      bool isValid()  const;
      bool canExec()  const;
      bool canUndo()  const;
      
    protected:
//     static Command& fetchDef (Symbol cmdID);
     
//     friend class CommandDef;

    };
  ////////////////TODO currently just fleshing  out the API....
  


  
  
} // namespace control
#endif
