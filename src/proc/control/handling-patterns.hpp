/*
  HANDLILNG-PATTERNS.hpp  -  Collection of predefined command handling patterns
 
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


/** @file handling-patterns.hpp
 ** A hard wired collection of predefined command handling patterns.
 ** There is a small number of different possibilities to handle execution
 ** and UNDO of proc-Layer commands. Each of these is defined as a subclass
 ** in this header and then hard wired into a small table. Handling patterns
 ** are stateless singleton objects, thus we build using SingletonSubclass
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

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/singleton-subclass.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/command-impl.hpp"
#include "include/lifecycle.h"
//#include "lib/symbol.hpp"

//#include <tr1/memory>
//#include <string>
#include<vector>



namespace control {

  namespace { // concrete command handling patterns 
  
    using std::vector;
//  using std::string;
//  using lib::Symbol;
//  using std::tr1::shared_ptr;
    
    
    
    /**
     * Handling Pattern: invoke blocking, translate exceptions into an error state
     * @todo describe this pattern in more detail....
     */
    class InvokeSyncNoThrow
      : public HandlingPattern
      {
        void
        perform (CommandImpl& command)  const
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        HandlingPattern const&
        defineUNDO()  const
          {
            UNIMPLEMENTED ("yield a handling pattern suitable for UNDOing a command, according to this pattern");
          }
        
        bool
        isValid()  const
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    /**
     * Handling Pattern: invoke blocking, propagating any exceptions immediately
     * @todo describe this pattern in more detail....
     */
    class InvokeSyncThrow
      : public HandlingPattern
      {
        void
        perform (CommandImpl& command)  const
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        HandlingPattern const&
        defineUNDO()  const
          {
            UNIMPLEMENTED ("yield a handling pattern suitable for UNDOing a command, according to this pattern");
          }
        
        bool
        isValid()  const
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    /**
     * Handling Pattern: just schedule command to be invoked asynchronously
     * @todo describe this pattern in more detail....
     */
    class InvokeAsync
      : public HandlingPattern
      {
        void
        perform (CommandImpl& command)  const
          {
            UNIMPLEMENTED ("actually invoke a command, according to this pattern");
          }
        
        HandlingPattern const&
        defineUNDO()  const
          {
            UNIMPLEMENTED ("yield a handling pattern suitable for UNDOing a command, according to this pattern");
          }
        
        bool
        isValid()  const
          {
            UNIMPLEMENTED ("is this pattern currently able to handle commands?");
          }
      };
    
    
    
    
    
    
    /* ======== Handling Pattern Table ========== */
    
    typedef lib::SingletonSub<HandlingPattern> SingletonFac;
    
    /** Table of available command handling patterns */
    vector<SingletonFac> patternTable;
    
    
    /** access the singleton instance for a given ID */
    inline HandlingPattern const&
    getPatternInstance (size_t id)
    {
      REQUIRE (id < patternTable.size());
      return patternTable[id] ();
    }
    
    
    /** populate the handling pattern table.
     *  This init-function will be invoked each time
     *  a new session is created or loaded.
     */
    void
    prepareCommandHandlingPatterns()
    {
      using lib::singleton::UseSubclass;
      
      patternTable[HandlingPattern::SYNC      ]  = SingletonFac(UseSubclass<InvokeSyncNoThrow>());
      patternTable[HandlingPattern::SYNC_THROW]  = SingletonFac(UseSubclass<InvokeSyncThrow>());
      patternTable[HandlingPattern::ASYNC     ]  = SingletonFac(UseSubclass<InvokeAsync>());
    }
    
    
    lumiera::LifecycleHook _schedule (lumiera::ON_GLOBAL_INIT, &prepareCommandHandlingPatterns);
  
  } // (END) definition of concrete handling patterns
  
  
  
  
  
  
  
} // namespace control
#endif
