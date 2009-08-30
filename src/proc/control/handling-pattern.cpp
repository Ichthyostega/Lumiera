/*
  HandlingPattern  -  A skeleton for executing commands, including standard implementations
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/



#include "proc/control/command.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/handling-patterns.hpp"

#include "include/symbol.hpp"
#include "include/logging.h"
#include "lib/util.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

#include <boost/format.hpp>


using boost::str;
using boost::format;
using namespace lumiera;
using util::cStr;
using util::isnil;


namespace control {
  
  /** */
  HandlingPattern const&
  HandlingPattern::get (ID id)
  {
    REQUIRE ((0 <= id) && (id < NUM_IDS));
    
    return getPatternInstance(id); 
  }
  

  /** @note: does error handling, but delegates the actual
   *         execution to the protected (subclass) member */
  ExecResult
  HandlingPattern::operator() (Command& command)  const
  {
    TRACE (proc_dbg, "invoking %s...", cStr(command));
    static format err_pre ("Error state detected, %s *NOT* invoked.");
    static format err_post ("Error state after %s invocation.");
    static format err_fatal ("Execution of %s raised unknown error.");
    try
      {
        Symbol errID_pre = lumiera_error();
        if (errID_pre)
          return ExecResult (error::Logic (str (err_pre % command), errID_pre));
        
        // Execute the command
        perform (command);
        
        Symbol errID = lumiera_error();
        if (errID)
          return ExecResult (error::State (str (err_post % command),errID));
        else
          return ExecResult();
      }
    
    
    catch (lumiera::Error& problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", cStr(command), problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID);
        return ExecResult (problem);
      }
    catch (std::exception& library_problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", cStr(command), library_problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID);
        return ExecResult (error::External (library_problem));
      }
    catch (...)
      {
        Symbol errID = lumiera_error();
        ERROR (command, "Invocation of %s failed with unknown exception; error flag is: %s", cStr(command), errID);
        throw error::Fatal (str (err_fatal % command), errID);
      }
  }
  
  
  HandlingPattern const&
  HandlingPattern::howtoUNDO()  const
  {
    return defineUNDO();
  }
  
  
  
  /* ====== execution result state object ======= */
  
  
  /** @note we just grab an retain the error message.
   *  @todo rather keep the exception object around. */
  ExecResult::ExecResult (lumiera::Error const& problem)
    : log_(problem.what())
  { }
  
  
  bool
  ExecResult::isValid() const
  {
    return isnil(log_);
  }
  
  
  void
  ExecResult::maybeThrow() const
  {
    if (!isnil (log_))
      throw error::Logic ("Command execution failed: "+log_);
  }
      
  
} // namespace control
