/*
  TEST-DUMMY-COMMANDS.hpp  -  dummy functions used to build test commands

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


/** @file test-dummy-commands.hpp
 ** Some dummy command functions used for building unit test cases. 
 ** Any of these functions comes in triples of operation function, undo state
 ** capturing function and UNDO function. They are placed into a nested test
 ** namespace, together with some global variables used as a backdoor to
 ** verify the effect of calling these functions.
 ** 
 ** @see command-use1-test.cpp
 ** @see CommandBasic_test simple complete command definition example
 **
 */



#ifndef COMMAND_TEST_DUMMY_COMMANDS_H
#define COMMAND_TEST_DUMMY_COMMANDS_H

#include "lib/error.hpp"
#include "lib/test/test-helper.hpp"

#include <functional>
#include <sstream>
#include <string>


namespace proc {
namespace control {
namespace test    {
  
  using std::ostringstream;
  using std::function;
  using std::string;
  
  
  
  
  namespace command1 { ///< test command just adding a given value
    
    extern long check_;
    
    
    inline void
    operate (int someVal)
    {
      check_ += someVal;
    }
    
    inline long
    capture (int)
    {
      return check_;
    }
    
    inline void
    undoIt (int, long oldVal)
    {
      check_ = oldVal;
    }
  
  }
  
  
  
  
  
  namespace command2 { ///< test command writing to protocol and possibly throwing
    
    using lumiera::error::External;
    
    
    extern ostringstream check_;
      
    
    typedef function<string()> FunS;
    
    inline void
    operate (FunS func, bool *fail)
    {
      if (fail and *fail) throw External("simulated exception");
      
      check_ << func();
    }
    
    inline string
    capture (FunS, bool*)
    {
      return check_.str();
    }
    
    inline void
    undoIt (FunS, bool *fail, string previousProtocol)
    {
      if (fail and *fail) throw External("simulated exception in UNDO");
      
      check_.str("");
      check_ << previousProtocol << "|UNDO|";
    }
  
  }
  
  
  
  
  
  namespace command3 { ///< test command taking zero arguments
    
    extern ulong check_;
    
    
    inline void
    operate ()
    {
      ++check_;
    }
    
    inline ulong
    capture ()
    {
      return check_;
    }
    
    inline void
    undoIt (ulong oldVal)
    {
      check_ = oldVal;
    }
  
  }
  
  
}}} // namespace proc::control::test
#endif
