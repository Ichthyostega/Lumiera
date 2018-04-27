/*
  DIAGNOSTIC-CONTEXT.hpp  -  thread local stack for explicitly collecting diagnostic context info

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file diagnostic-context.hpp
 ** Facility for collecting diagnostic context information explicitly.
 ** Unlike a trace logging run, this facility is intended to be fed explicitly with
 ** diagnostic information describing the currently ongoing operation in a semantic
 ** high-level manner. The rationale is to pinpoint \em those pieces of information,
 ** which aren't obvious when just looking at a callstack with the debugger.
 ** Instances of the class DiagnosticContext should be placed explicitly as automatic
 ** (stack) variable into selected relevant scopes; these "information frames" could
 ** be accessed from an enclosed scope  as a per-thread stack. DiagnosticContext
 ** provides an controlled environment for adding diagnostic code on demand; typically
 ** to be configured such as to resolve into an empty class for release builds.
 ** 
 ** As of 2/10, this is an experimental feature in evaluation. To start with,
 ** I'll use it to solve the problem of providing a NoBug resource tracker handle
 ** without tangling the object monitor code (sync.hpp) with low level, NoBug related
 ** implementation details.
 ** 
 ** @todo add the actual diagnostic content
 **/


#ifndef LIB_DIAGNOSTIC_CONTEXT_H
#define LIB_DIAGNOSTIC_CONTEXT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/thread-local.hpp"

#include <nobug.h>



namespace lib {
  
  
  
  
  /** 
   * Diagnostic data frame to collect specific information concerning a scope.
   * To be placed explicitly as an automatic (stack) variable. Provides a controlled
   * environment for hooking up diagnostic code. Within each thread, a stack of
   * such information frames concerning nested scopes is maintained automatically.
   * It can be accessed via static API.
   * @warning relies on thread-local access; never use this within global data structures.
   */
  template<typename VAL>
  class DiagnosticContext
    : util::NonCopyable
    {
      typedef ThreadLocalPtr<DiagnosticContext> ThreadLocalAccess;
      typedef std::vector<VAL>                  ValSequence;
      
      const VAL value_;
      DiagnosticContext * const prev_;
      
      /** embedded thread local pointer
       *  to the innermost context encountered */
      static ThreadLocalAccess&
      current()
        {
          static ThreadLocalAccess accessPoint;
          return accessPoint;
        }
      
      
    public:
      DiagnosticContext(VAL const& value_to_log = VAL())
        : value_(value_to_log)
        , prev_(current().get())
        {
          current().set (this);
        }
      
     ~DiagnosticContext()
        {
          ASSERT (this == current().get());
          current().set (prev_);
        }
      
      
      operator VAL const&()  ///< access the payload by conversion
        {
          return value_;
        }
      
      /** accessing the innermost diagnostic context created */
      static DiagnosticContext&
      access ()
        {
          DiagnosticContext* innermost = current().get();
          if (!innermost)
            throw lumiera::error::Logic ("Accessing Diagnostic context out of order; "
                                         "an instance should have been created within "
                                         "an enclosing scope");
          return *innermost;
        }
      
      
      /** snapshot of the current stack of diagnostic frames
       * @return vector with all the payload values currently
       *         on the thread-local diagnostic stack. Might
       *         be empty. Values start with frame next to
       *         the current scope and end with outermost.
       * @warning can be inefficient on very large stacks
       * @todo benchmark and improve the data structure
       *       used for the snapshot. Vector is not
       *       an optimal choice here.
       */
      static ValSequence
      extractStack()
        {
          ValSequence loggedValues;
          DiagnosticContext* next = current().get();
          while (next)
            {
              loggedValues.push_back (*next);
              next = next->prev_;
            }
          return loggedValues;
        }
    };
  
  
  
  
  
} // namespace lib
#endif
