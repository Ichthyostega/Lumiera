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
#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
//#include "include/symbol.hpp"

//#include <tr1/memory>
#include <string>



namespace control {
  
  using std::string;
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  
  
  class Command;
  
  
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
    {
      
    public:
      
      enum ID
        { SYNC
        , SYNC_THROW
        , ASYNC
        };
      
      static HandlingPattern const& get (ID id);
      
      
      virtual ~HandlingPattern() {}
      
      /** main functionality: invoke a command, detect errors.
       *  @return ExecResult object, which might later be used to 
       *          detect errors on execution */
      ExecResult operator() (Command& command)  const;
      
      /** @return HandlingPatter describing how the UNDO operation is to be performed */
      HandlingPattern const& howtoUNDO()  const;
      
      
      
    protected:
      
      virtual void perform (Command& command)  const  =0;
      
      virtual HandlingPattern const& defineUNDO()  const  =0;
      
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
