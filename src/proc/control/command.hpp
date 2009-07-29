/*
  COMMAND.hpp  -  Key abstraction for proc/edit operations and UNDO management
 
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


/** @file command.hpp
 ** //TODO 
 **
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

#include "pre.hpp"
#include "include/symbol.hpp"
#include "proc/control/command-binding.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-closure.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/handle.hpp"

//#include <tr1/memory>

///////////////////////////////////////////TODO: define an C-API representation here, make the header multilingual!



namespace control {
  
  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
  LUMIERA_ERROR_DECLARE (INVALID_COMMAND);    ///< Unknown or insufficiently defined command
  LUMIERA_ERROR_DECLARE (INVALID_ARGUMENTS);  ///< Arguments provided for binding doesn't match stored command function parameters

  
  
  class CommandDef;
  class HandlingPattern;
  
  
  /**
   * @todo Type-comment
   */
  class Command
    : public com::ArgumentBinder<Command    // accepts arbitrary bind(..) calls (with runtime check)
           , lib::BoolCheckable<Command    //  implicit conversion to bool for status check
           , lib::Handle<CommandImpl>     //   actually implemented as ref counting Handle
           > >
    {
      
    public:
      /* === command registry === */
      static Command& get (Symbol cmdID);
      static bool  remove (Symbol cmdID); 
      static bool   undef (Symbol cmdID);
      
      CommandDef storeDef (Symbol newCmdID);
      
      
     ~Command();
      
      void operator() () ;
      void undo () ;
      
      
      /** core operation: invoke the command
       *  @param execPattern describes the individual steps
       *         necessary to get this command invoked properly
       */
      void exec (HandlingPattern const& execPattern);
      
      void execSync ();
      
      HandlingPattern const& getDefaultHandlingPattern()  const;
      
      
      template<typename TYPES>
      void bindArg (Tuple<TYPES> const&);
      
      
      /* === diagnostics === */
      
      static size_t definition_count();
      static size_t instance_count();
      
      bool isValid()  const;
      bool canExec()  const;
      bool canUndo()  const;
      
    protected:
      static Command& fetchDef (Symbol cmdID);
      
      friend class CommandDef;
      
      
    private:
    /** Commands can only be created through the framework
     *  (by setting up an CommandDef), thus ensuring there's
     *  always a corresponding CommandImpl within the registry.
     *  @note the copy operations are public though
     *  @see Command#fetchDef
     *  @see CommandDef 
     */
    Command (CommandImpl* pImpl);
     
    };
  ////////////////TODO currently just fleshing  out the API....
  

  inline void
  Command::operator() ()
  {
    exec (getDefaultHandlingPattern());
  }

  
  
} // namespace control
#endif
