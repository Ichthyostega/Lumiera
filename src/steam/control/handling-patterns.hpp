/*
  HANDLILNG-PATTERNS.hpp  -  Collection of predefined command handling patterns

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


/** @file handling-patterns.hpp
 ** A hard wired collection of predefined command handling patterns.
 ** There is a small number of different possibilities to handle execution
 ** and UNDO of Steam-Layer commands. Each of these is defined as a subclass
 ** in this header and then hard wired into a small table. Handling patterns
 ** are stateless singleton objects, thus we build using multiple Singleton
 ** factory objects and configure them hard wired with the respective
 ** implementation classes. The index positions in this table match
 ** the sequence within the enum HandlingPattern::ID; all of this
 ** is done hard wired and without any dynamic configuration.
 **
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_HANDLING_PATTERNS_DEF_H
#define CONTROL_HANDLING_PATTERNS_DEF_H

#include "lib/error.hpp"
#include "lib/multifact.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/command-impl.hpp"



namespace proc {
namespace control {

  namespace { // concrete command handling patterns 
    
    
    
    
    /**
     * Handling Pattern Foundation: invoke command directly and without
     * any external intervention. This pattern is intended as implementation
     * base class, but can be used as-is for unit tests.
     */
    class BasicHandlingPattern
      : public HandlingPattern
      {
        bool isValid()  const { return true; }
        
        void
        performExec (CommandImpl& command)  const override
          {
            REQUIRE (command.canExec());
            command.invokeCapture();
            command.invokeOperation();
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            REQUIRE (command.canUndo());
            command.invokeUndo();
          }
      };
    
    
    
    /**
     * Handling Pattern: invoke blocking, translate exceptions into an error state
     * @todo describe this pattern in more detail....
     * @todo unimplemented...               //////////////////////////////////////////////////////////TICKET #210
     */
    class InvokeSyncNoThrow
      : public BasicHandlingPattern
      {
        void
        performExec (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually undo the effect of a command, according to this pattern");
          }
        
        bool
        isValid()  const override
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    /**
     * Handling Pattern: invoke blocking, propagating any exceptions immediately
     * @todo is throwing here helpful, and how to integrate it into ExecResult...?
     * @todo describe this pattern in more detail....
     * @todo unimplemented...               //////////////////////////////////////////////////////////TICKET #210
     */
    class InvokeSyncThrow
      : public BasicHandlingPattern
      {
        void
        performExec (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually undo the effect of a command, according to this pattern");
          }
        
        bool
        isValid()  const override
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    /**
     * Handling Pattern: just schedule command to be invoked asynchronously
     * @todo clarify what "async" means and if we need it.....
     * @todo describe this pattern in more detail....
     * @todo unimplemented...               //////////////////////////////////////////////////////////TICKET #210
     */
    class InvokeAsync
      : public BasicHandlingPattern
      {
        void
        performExec (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        void
        performUndo (CommandImpl& command)  const override
          {
            UNIMPLEMENTED ("actually undo the effect of a command, according to this pattern");
          }
        
        bool
        isValid()  const override
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    
    
    
    /* ======== Handling Pattern Table ========== */
    
    typedef lib::factory::MultiFact<HandlingPattern&, HandlingPattern::ID> HandlingPatternFactory;
    
    /** holds singleton pattern instances by ID */
    HandlingPatternFactory patternTable;
    
    HandlingPatternFactory::Singleton<InvokeSyncNoThrow>    holder1 (patternTable, HandlingPattern::SYNC);
    HandlingPatternFactory::Singleton<InvokeSyncThrow>      holder2 (patternTable, HandlingPattern::SYNC_THROW);
    HandlingPatternFactory::Singleton<InvokeAsync>          holder3 (patternTable, HandlingPattern::ASYNC);
    HandlingPatternFactory::Singleton<BasicHandlingPattern> holder4 (patternTable, HandlingPattern::DUMMY);
    
    
    /** access the singleton instance for a given ID */
    inline HandlingPattern const&
    getPatternInstance (HandlingPattern::ID id)
    {
      REQUIRE (patternTable.contains(id));
      
      return patternTable (id);
    }
    
  
  } // (END) definition of concrete handling patterns
  
  
  
  
  
  
  
}} // namespace proc::control
#endif
