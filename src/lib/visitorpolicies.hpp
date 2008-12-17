/*
  VISITOR.hpp  -  Acyclic Visitor library
 
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
 
*/


/** @file visitorpolicies.hpp
 ** Policies usable for configuring the lumiera::visitor::Tool for different kinds of error handling. 
 ** @see buildertool.hpp for another flavour (calling an catch-all-function there)
 **
 */



#ifndef LUMIERA_VISITORPOLICIES_H
#define LUMIERA_VISITORPOLICIES_H

#include "include/error.hpp"


namespace lumiera
  {
  namespace visitor
    {
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
      
      
    
  } // namespace visitor

} // namespace lumiera
#endif
