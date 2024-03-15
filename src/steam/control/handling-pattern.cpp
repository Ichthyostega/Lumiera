/*
  HandlingPattern  -  A skeleton for executing commands, including standard implementations

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file handling-pattern.cpp
 ** Implementation of pre-defined command handling patterns
 ** @internal detail of the Steam-Layer command handling framework.
 */



#include "lib/error.hpp"
#include "steam/control/handling-pattern.hpp"
#include "steam/control/handling-patterns.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"


using util::isnil;
using util::_Fmt;


namespace steam {
namespace control {
  namespace error = lumiera::error;
  
  HandlingPattern const&
  HandlingPattern::get (ID id)
  {
    return getPatternInstance(id); 
  }
  
  
  /** @internal dispatch to the desired operation, with error handling */
  ExecResult
  HandlingPattern::invoke (CommandImpl& command, string id, Action action)  const
  {
    const char* cmdName = cStr(id);
    TRACE (proc_dbg, "invoking %s...", cmdName);
    static _Fmt err_pre ("Error state detected, %s *NOT* invoked.");
    static _Fmt err_post ("Error state after %s invocation.");
    static _Fmt err_fatal ("Execution of %s raised unknown error.");
    try
      {
        Symbol errID_pre = lumiera_error();
        if (errID_pre)
          return ExecResult (error::Logic (err_pre % command, errID_pre));
        
        // execute or undo it...
        (this->*action) (command);
        
        Symbol errID = lumiera_error();
        if (errID)
          return ExecResult (error::State (err_post % command, errID));
        else
          return ExecResult();
      }
    
    
    catch (lumiera::Error& problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", cmdName, problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID.c());
        return ExecResult (problem);
      }
    catch (std::exception& library_problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", cmdName, library_problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID.c());
        return ExecResult (error::External (library_problem));
      }
    catch (...)
      {
        Symbol errID = lumiera_error();
        ERROR (command, "Invocation of %s failed with unknown exception; error flag is: %s", cmdName, errID.c());
        throw error::Fatal (err_fatal % command, errID);
      }
  }
  
  
  
  /* ====== execution result state object ======= */
  
  
  /** @note just grab and retain the error message, but _clear_ the error flag.
   *        Rationale: by packaging into the ExecResult, the excepton counts as treated.
   */
  ExecResult::ExecResult (lumiera::Error const& problem)
    : log_(problem.what())
    {
      lumiera_error(); // ensure error flag is cleared
    }
  
  
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
  
  
}} // namespace steam::control
