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
 ** Pre-defined command execution templates.
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

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/symbol.hpp"

//#include <tr1/memory>
#include <string>



namespace control {
  
  using std::string;
  using lib::Symbol;
//  using std::tr1::shared_ptr;
  
  
  class CommandImpl;
  
  
  /**
   * @todo Type-comment
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
   * @todo Type-comment
   */
  class HandlingPattern
    : public lib::BoolCheckable<HandlingPattern>
    {
      
    public:
      
      enum ID
        { SYNC
        , SYNC_THROW
        , ASYNC
        
        , NUM_IDS
        };
      
      static ID defaultID() { return SYNC; }
      
      
      static HandlingPattern const& get (ID id);
      
      
      virtual ~HandlingPattern() {}
      
      /** main functionality: invoke a command, detect errors.
       *  @return ExecResult object, which might later be used to 
       *          detect errors on execution */
      ExecResult invoke (CommandImpl& command, Symbol name)  const;
      
      /** @return HandlingPatter describing how the UNDO operation is to be performed */
      HandlingPattern const& howtoUNDO()  const;
      
      
      virtual bool isValid()  const  =0;
      
    protected:
      
      virtual void perform (CommandImpl& command)  const  =0;
      
      virtual HandlingPattern const& defineUNDO()  const  =0;
      
      
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
