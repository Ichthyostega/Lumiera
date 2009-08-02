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

#include "proc/control/command.hpp"
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
    : public lib::Sync<>
    {
      
    public:
      static lumiera::Singleton<CommandRegistry> instance;
      
      
      /** register a command (Frontend) under the given ID
       *  @return either the new command, or an already existing
       *          command registered under the given ID*/ 
      Command
      track (Symbol cmdID, Command const& commandHandle)
        {
          Lock sync(this);
          UNIMPLEMENTED ("place a commandHandle into the command index, or return the command already registered there");
        }
      
      
      /** query the command index by ID
       *  @return the registered command,
       *          or an "invalid" token */
      Command
      queryIndex (Symbol cmdID)
        {
          Lock sync(this);
          UNIMPLEMENTED ("retrieve the command registered under the given ID, maybe return an »empty« command");
          // if index.contains(cmdID)
          //   return index[cmdID]
          // else
          return Command();
        }
      
      
      /** search the command index for a definition
       *  @param cmdInstance using the definition to look up
       *  @return the ID used to register this definition 
       *          or \c NULL in case of an "anonymous" command */
      const char*
      findDefinition (Command const& cmdInstance)
        {
          UNIMPLEMENTED ("try to find a registration in the index for a given command instance");
        }
      
      
      size_t
      index_size()
        {
          UNIMPLEMENTED ("number of defs in the index");
        }
      
      
      size_t
      instance_count()
        {
          UNIMPLEMENTED ("number of active command impl instances");
        }
      
      
      /** set up a new command implementation frame */
      template< typename SIG_OPER    ///< signature of the command operation
              , typename SIG_UNDO    ///< signature to undo the command
              , typename SIG_CAPT    ///< signature for capturing undo state
              >
      CommandImpl&
      newCommandImpl (function<SIG_OPER>& operFunctor_
                     ,function<SIG_UNDO>& undoFunctor_
                     ,function<SIG_CAPT>& captFunctor_)
        {
          Lock sync(this);
          UNIMPLEMENTED ("set up a new impl instance located within the instance table");
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
          ///////////////////////////////TODO: still necessary?
        }
      
      void
      removeImpl (CommandImpl* entry)
        {
          UNIMPLEMENTED ("remove entry from instance table");
        }
      
    };
  
  
} // namespace control
#endif
