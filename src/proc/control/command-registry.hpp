/*
  COMMAND-REGISTRY.hpp  -  proc-Command object registration and storage management 
 
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


/** @file command-registry.hpp
 ** Managing command definitions and the storage of individual command objects.
 ** @todo WIP WIP.
 ** 
 ** \par Lifecycle
 ** 
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_COMMAND_REGISTRY_H
#define CONTROL_COMMAND_REGISTRY_H

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/singleton.hpp"
#include "lib/sync.hpp"
//#include "lib/bool-checkable.hpp"
//#include "proc/control/command-closure.hpp"
//#include "proc/control/memento-tie.hpp"

//#include <iostream>
//#include <string>



namespace control {
  
//  using std::ostream;
//  using std::string;
  
  
  
  
  /**
   * TODO type comment
   */
  class CommandRegistry
    : lib::Sync<>
    {
      
    public:
      static lumiera::Singleton<CommandRegistry> instance;
      
      
      /** register a command (Frontend) under the given ID
       *  @return either the new command, or an already existing
       *          command registerd under the given ID*/ 
      static Command&
      track (Symbol cmdID, Command& commandHandle)
        {
          return instance().putIndex (cmdID, commandHandle);
        }
      
      
      /** set up a new command implementation frame */
      static CommandImpl*
      newCommandImpl ()
        {
          return instance().createImpl();
        }
      
      
      /** discard an command implementation frame */
      static void
      killCommandImpl (CommandImpl* entry)
        {
                                  ///////////////////////////////////////////////TODO: clean behaviour while in App shutdown (Ticket #196)
          instance().removeImpl(entry);
        }
      
      
    private:
      CommandImpl*
      createImpl ()
        {
          Lock sync(this);
          UNIMPLEMENTED ("set up a new impl instance located within the instance table");
        }
      
      void
      removeImpl (CommandImpl* entry)
        {
          UNIMPLEMENTED ("remove entry from instance table");
        }
      
      Command&
      putIndex (Symbol cmdID, Command& commandHandle)
        {
          Lock sync(this);
          UNIMPLEMENTED ("place a commandHandle into the command index, or return the command already registered there");
        }
    };
  
  
//  inline ostream& operator<< (ostream& os, Mutation const& muta) { return os << string(muta); }
  
  /** storage for the singleton factory used to access CommandRegistry */
  lumiera::Singleton<CommandRegistry> CommandRegistry::instance;
  
  
} // namespace control
#endif
