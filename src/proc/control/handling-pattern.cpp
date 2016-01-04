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



#include "lib/error.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/handling-patterns.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"


using util::isnil;
using util::cStr;
using util::_Fmt;


namespace proc {
namespace control {
  namespace error = lumiera::error;
  
  /** retrieve pre-configured pattern */
  HandlingPattern const&
  HandlingPattern::get (ID id)
  {
    return getPatternInstance(id); 
  }
  
  
  /** @param name to use in log and error messages
   *  @note  does error handling, but delegates the actual
   *         execution to the protected (subclass) member */
  ExecResult
  HandlingPattern::invoke (CommandImpl& command, Symbol name)  const
  {
    TRACE (proc_dbg, "invoking %s...", name.c());
    static _Fmt err_pre ("Error state detected, %s *NOT* invoked.");
    static _Fmt err_post ("Error state after %s invocation.");
    static _Fmt err_fatal ("Execution of %s raised unknown error.");
    try
      {
        Symbol errID_pre = lumiera_error();
        if (errID_pre)
          return ExecResult (error::Logic (err_pre % command, errID_pre));
        
        // execute or undo it...
        perform (command);
        
        Symbol errID = lumiera_error();
        if (errID)
          return ExecResult (error::State (err_post % command, errID));
        else
          return ExecResult();
      }
    
    
    catch (lumiera::Error& problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", name.c(), problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID.c());
        return ExecResult (problem);
      }
    catch (std::exception& library_problem)
      {
        Symbol errID = lumiera_error();
        WARN (command, "Invocation of %s failed: %s", name.c(), library_problem.what());
        TRACE (proc_dbg, "Error flag was: %s", errID.c());
        return ExecResult (error::External (library_problem));
      }
    catch (...)
      {
        Symbol errID = lumiera_error();
        ERROR (command, "Invocation of %s failed with unknown exception; error flag is: %s", name.c(), errID.c());
        throw error::Fatal (err_fatal % command, errID);
      }
  }
  
  
  
  /* ====== execution result state object ======= */
  
  
  /** @note we just grab and retain the error message.
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
  
  
}} // namespace proc::control
