/*
  DEPENDABLE-BASE.hpp  -  fundamental structures with extended lifespan

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dependable-base.hpp
 ** Static container to hold basic entities needed during static init and shutdown.
 ** @see sync-classlock.hpp
 ** @see depend.hpp
 */


#ifndef LIB_DEPENDABLE_BASE_H
#define LIB_DEPENDABLE_BASE_H

#include "lib/nocopy.hpp"

#include <iostream>
#include <utility>


namespace lib {
  
  namespace nifty { // implementation details
    
    template<class X>
    struct Holder 
      {
        char payload_[sizeof(X)];
        
        //NOTE: deliberately no ctor/dtor
       
        X&
        access()
          { 
            return *reinterpret_cast<X*> (&payload_);
          }
      };
    
    ///////////////////////////////////////////////////////////////TICKET #1133 damn it. How the hell do we determine when the object is initialised...?
    ///////////////////////////////////////////////////////////////TICKET #1133 ........ Looks like the whole approach is ill guided
    
  } // (End) nifty implementation details
  
  
  
  /**
   * A dependable data container available with extended lifespan.
   * Automatically plants a ref-count to ensure the data stays alive
   * until the last static reference goes out of scope.
   */
  template<class X>
  class DependableBase
    : util::NonCopyable
    {
      static nifty::Holder<X> storage_;
      
    public:
      template<typename...ARGS>
      explicit
      DependableBase (ARGS&& ...args)
        {
          storage_.maybeInit (std::forward<ARGS> (args)...);
        }
      
      operator X& ()  const
        {
          return storage_.access();
        }
      
      uint use_count() { return nifty::Holder<PerClassMonitor>::accessed_; }
    };
  
  /** plant a static buffer to hold the payload X */
  template<class X>
  nifty::Holder<X> DependableBase<X>::storage_;
  
  
} // namespace lib
#endif /*LIB_DEPENDABLE_BASE_H*/
