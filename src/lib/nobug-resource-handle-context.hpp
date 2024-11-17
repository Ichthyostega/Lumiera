/*
  NOBUG-RESOURCE-HANDLE-CONTEXT.hpp  -  thread local stack to manage NoBug resource handles

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file nobug-resource-handle-context.hpp
 ** Thread-local stack of NoBug resource handles.
 ** This helper allows to access the resource handle in the nearest enclosing scope.
 ** The motivation for this approach was to avoid passing the resource handle over
 ** several intermediary function calls when using a scoped variable to control
 ** object monitor locking. Within this usage context, the necessity of passing
 ** a NoBug resource handle seems to be a cross-cutting concern, and not directly
 ** related to the core concern (controlling a mutex).
 ** 
 ** @remarks as of 8/2011, this feature is not used anymore. In 12/2011, the concept
 ** of a diagnostic context stack was generalised. At that point, this header was
 ** created as an off-spin, now based on the generalised feature, to document this
 ** usage possibility, which might be required again at some point in the future.
 ** 
 ** @see lib::DiagnosticContext
 ** @see diagnostic-context-test.hpp
 ** 
 **/


#ifndef LIB_NOBUG_RESOURCE_HANDLE_CONTEXT_H
#define LIB_NOBUG_RESOURCE_HANDLE_CONTEXT_H


#include "lib/error.hpp"
#include "lib/diagnostic-context.hpp"
#include "lib/thread-local.hpp"

#include <nobug.h>



namespace lib {
  
  
  
#ifdef NOBUG_MODE_ALPHA      ////////////////////////TODO don't we need the handle in BETA builds for resource logging?
  
  /** 
   * Diagnostic data frame to hold a NoBug resource handle.
   * This way, code in nested function calls may pick up the nearest available handle.
   * @warning relies on thread-local access; never use this within global data structures.
   */
  class NobugResourceHandleContext
    : DiagnosticContext<nobug_resource_user*>
    {
    public:
    };
  
  
#else  /* not NOBUG_ALPHA */
  
  
  /** 
   * Disabled placeholder for the Diagnostic context, not used in release builds.
   */
  class NobugResourceHandleContext
    : util::NonCopyable
    {
      
      typedef nobug_resource_user* Handle;
      
    public:
      
      operator Handle () ///< payload: NoBug resource tracker user handle
        {
          return 0;
        }
      
      /** accessing the innermost diagnostic context created */
      static NobugResourceHandleContext&
      access ()
        {
          UNIMPLEMENTED ("how to disable DiagnosticContext with minimum overhead");
        }
    };
#endif /* NOBUG_ALPHA? */
  
  
  
} // namespace lib
#endif
