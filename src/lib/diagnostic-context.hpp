/*
  DIAGNOSTIC-CONTEXT.hpp  -  thread local stack for explicitly collecting diagnostic context info
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file diagnostic-context.hpp
 ** Facility for collecting diagnostic informations explicitly.
 ** Unlike a trace logging run, this facility is intended to be fed explicitly with
 ** diagnostic information describing the currently ongoing operation in a semantic
 ** high-level manner. The rationale is to pinpoint \em those informations which aren't
 ** obvious when just looking at a callstack with the debugger. Instances of the class
 ** DiagnosticContext should be placed explicitly as automatic (stack) variable
 ** into selected relevant scopes; these "information frames" can be accessed
 ** from an enclosed scope  as a per-thread stack. DiagnosticContext provides
 ** an controlled environment for adding diagnostic code on demand; typically
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
#include "lib/thread-local.hpp"

#include <boost/noncopyable.hpp>
#include <nobug.h>



namespace lib {
  
  
  
#ifdef NOBUG_MODE_ALPHA      ////////////////////////TODO don't we need the handle in BETA builds for resource logging?
  
  /** 
   * Diagnostic data frame to collect specific informations concerning a scope.
   * To be placed explicitly as an automatic (stack) variable. Provides a controlled
   * environment for hooking up diagnostic code. Within each thread, a stack of
   * such information frames concerning nested scopes is maintained automatically.
   * It can be accessed via static API.
   * @warning never store this into global data structures.
   * @note as of 2/10 used for housing the NoBug resource tracker handle
   *       in conjunction with object monitor based locking
   * @see sync.hpp
   */
  class DiagnosticContext
    : boost::noncopyable
    {
      typedef nobug_resource_user* Handle;
      typedef ThreadLocalPtr<DiagnosticContext> ThreadLocalAccess;
      
      Handle handle_;
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
      DiagnosticContext()
        : handle_(0)
        , prev_(current().get())
        {
          current().set (this);
        }
      
     ~DiagnosticContext()
        {
          ASSERT (this == current().get());
          current().set (prev_);
        }
      
      
      operator Handle* () ///< payload: NoBug resource tracker user handle
        {
          return &handle_;
        }
      
      /** accessing the innermost diagnostic context created */
      static DiagnosticContext&
      access ()
        {
          DiagnosticContext* innermost = current().get();
          if (!innermost)
            throw lumiera::error::Logic ("Accessing Diagnostic context out of order; "
                                         "an instance should have been created in "
                                         "an enclosing scope");
          return *innermost;
        }
    };
  
  
#else  /* not NOBUG_ALPHA */
  
  
  /** 
   * Disabled placeholder for the Diagnostic context, not used in release builds.
   */
  class DiagnosticContext
    : boost::noncopyable
    {
      
      
    public:
      
      operator Handle* () ///< payload: NoBug resource tracker user handle
        {
          return 0;
        }
      
      /** accessing the innermost diagnostic context created */
      static DiagnosticContext&
      access ()
        {
          UNIMPLEMENTED ("how to disable DiagnosticContext with minimum overhead");
        }
    };
#endif /* NOBUG_ALPHA? */
  
  
  
} // namespace lib
#endif
