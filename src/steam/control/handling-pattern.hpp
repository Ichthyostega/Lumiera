/*
  HANDLILNG-PATTERN.hpp  -  A skeleton for executing commands, including standard implementations

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file handling-pattern.hpp
 ** Pre-defined command execution skeletons.
 ** Any command can be configured to use a specific handling pattern
 ** on invocation. Moreover, there is a default handling pattern for commands.
 ** These patterns define the steps necessary for getting the command actually
 ** invoked (template method pattern). A pattern may cause the command to be
 ** enqueued, registered for UNDO or dispatched into a background thread.
 ** To carry out the work, HandlingPattern implementations are allowed to
 ** invoke the CommandImpl API directly. 
 **
 ** @todo it is not clear what's the difference between "throw" and "no-throw" pattern
 ** @todo any integration with the (yet undefined as of 9/09) SteamDispatcher is missing.
 **
 ** @see SteamDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_HANDLING_PATTERN_H
#define CONTROL_HANDLING_PATTERN_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"

#include <string>



namespace steam {
namespace control {
  
  using std::string;
  using lib::Symbol;
  
  
  class CommandImpl;
  
  
  /**
   * Result (Status) of command execution.
   * It is returned when invoking a HandlingPattern
   * and can be used to check for success and/or re-throw
   * any Exception encountered during the command execution.
   * @todo couldn't that be replaced by a lib::Result<void> instance??
   */
  class ExecResult
    {
      const string log_;
      
    public:
      bool isValid() const;
      void maybeThrow() const;
      operator bool() const { return isValid(); }
      
    protected:
      ExecResult () { }                   ///< default: command executed successfully
      ExecResult (lumiera::Error const&); ///< this result marks a failed execution
      
      friend class HandlingPattern;
    };
  
  
  
  /**
   * Interface: Operation Skeleton how to invoke or undo a command.
   * Concrete implementations may be retrieved by ID; they range
   * from just invoking the command operations straight forward
   * to dispatching with the SteamDispatcher or running the command
   * asynchronously in a background thread.
   */
  class HandlingPattern
    {
    public:
      virtual ~HandlingPattern() {}   ///< this is an interface
      
      enum ID
        { SYNC
        , SYNC_THROW
        , ASYNC
        , DUMMY
        
        , NUM_IDS
        };
      
      static ID defaultID() { return DUMMY; }   ///////////TODO: should be ID::SYNC   Ticket #211
      
      /** retrieve the pre-configured pattern */
      static HandlingPattern const& get (ID id);
      
      
      
      /** main functionality: invoke a command, detect errors.
       * @param string id of the command for error logging
       * @return ExecResult object, which might later be used
       *          to detect errors on execution */
      ExecResult exec (CommandImpl& command, string)  const;
      
      /** likewise invoke the configured UNDO operation */
      ExecResult undo (CommandImpl& command, string)  const;
      
      explicit operator bool()  const { return isValid(); }
      virtual bool isValid()    const                         =0;
      
    protected:
      
      virtual void performExec (CommandImpl& command)  const  =0;
      virtual void performUndo (CommandImpl& command)  const  =0;
      
    private:
      typedef void (HandlingPattern::*Action) (CommandImpl&) const;
      
      ExecResult invoke (CommandImpl&, string id, Action)  const;
    };
  
  
  
  inline ExecResult
  HandlingPattern::exec (CommandImpl& command, string id)  const
  {
    return this->invoke (command, id, &HandlingPattern::performExec);
  }
  
  inline ExecResult
  HandlingPattern::undo (CommandImpl& command, string id)  const
  {
    return this->invoke (command, id, &HandlingPattern::performUndo);
  }
  
  
  
  
}} // namespace steam::control
#endif
