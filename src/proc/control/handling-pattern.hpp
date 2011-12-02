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
 ** @todo any integration with the (yet undefined as of 9/09) ProcDispatcher is missing.
 **
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_HANDLING_PATTERN_H
#define CONTROL_HANDLING_PATTERN_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/bool-checkable.hpp"

#include <string>



namespace proc {
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
    : public lib::BoolCheckable<ExecResult>
    {
      const string log_;
      
    public:
      bool isValid() const;
      void maybeThrow() const;
      
    protected:
      ExecResult () { }                   ///< default: command executed successfully
      ExecResult (lumiera::Error const&); ///< this result marks a failed execution
      
      friend class HandlingPattern;
    };
  
  
  
  /**
   * Operation Skeleton how to invoke or undo a command.
   * Concrete implementations may be retrieved by ID;
   * they range from just invoking the command operations
   * straight forward to dispatching with the ProcDispatcher
   * or running the command asynchronously in a background thread.
   * A HandlingPattern first of all describes how to invoke the
   * command operation, but for each pattern it is possible to
   * get a special "undo pattern", which, on activation, will
   * reverse the effect of the basic pattern.  
   */
  class HandlingPattern
    : public lib::BoolCheckable<HandlingPattern>
    {
      
    public:
      
      enum ID
        { SYNC
        , SYNC_THROW
        , ASYNC
        , DUMMY
        
        , NUM_IDS
        };
      
      static ID defaultID() { return DUMMY; }   ///////////TODO: should be ID::SYNC   Ticket #211
      
      
      static HandlingPattern const& get (ID id);
      
      
      virtual ~HandlingPattern() {}
      
      /** main functionality: invoke a command, detect errors.
       *  @return ExecResult object, which might later be used to 
       *          detect errors on execution */
      ExecResult invoke (CommandImpl& command, Symbol name)  const;
      
      /** @return HandlingPattern describing how the UNDO operation is to be performed */
      HandlingPattern const& howtoUNDO()  const { return getUndoPatt(); }
      
      
      virtual bool isValid()  const  =0;
      
    protected:
      
      virtual HandlingPattern const& getUndoPatt() const =0;
      virtual void perform (CommandImpl& command)  const =0;
      
      virtual void exec (CommandImpl& command)  const  =0;
      virtual void undo (CommandImpl& command)  const  =0;
      
      
      
    };
  
  
  
  
}} // namespace proc::control
#endif
