/*
  VISITOR-POLICIES.hpp  -  Acyclic Visitor library

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file visitor-policies.hpp
 ** Policies usable for configuring the lib::visitor::Tool for different kinds of error handling. 
 ** @see buildertool.hpp for another flavour (calling an catch-all-function there)
 **
 */



#ifndef LUMIERA_VISITOR_POLICIES_H
#define LUMIERA_VISITOR_POLICIES_H

#include "lib/error.hpp"


namespace lib {
namespace visitor {
  
  /** 
   * Policy returning just the default return value in case
   * of encountering an unknown Visitor (typically caused by
   * adding a new class to the visitable hierarchy)
   */
  template<class RET>
  struct UseDefault
    {
      template<class TAR>
      RET 
      onUnknown (TAR&)
        {
          return RET();
        }
    };
  
  /** 
   * Policy to throw when encountering an unknown visiting tool
   */
  template<class RET>
  struct ThrowException
    {
      template<class TAR>
      RET 
      onUnknown (TAR&)
        {
          throw lumiera::error::Config("unable to decide what tool operation to call");
        }
    };
  
  
  
}} // namespace lib::visitor
#endif
