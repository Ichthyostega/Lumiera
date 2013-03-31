/*
  HIERARCHY-ORIENTATION-INDICATOR.hpp  -  service for
  
  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>
  
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

/** @file hierarchy-orientation-indicator.hpp
 ** Service for hierarchy-orientation-indicator.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2013 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see hierarchy-orientation-indicator.cpp implementation
 ** 
 */

#ifndef HIERARCHY_ORIENTATION_INDICATOR_HPP_
#define HIERARCHY_ORIENTATION_INDICATOR_HPP_

#include "lib/error.hpp"
#include "lib/hash-value.h"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>

namespace lib {
  
  using lib::HashVal;
  using util::isnil;
  using std::string;
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see NA_test
   */
  template<class X>
  class Murps
    {
      string nothing_;

    public:
      explicit Murps (string const& b) :
          nothing_ (b)
        {
        }
      
      // using default copy/assignment
      
      /* == Adapter interface for == */

      void setSolution (string const& solution = "")
        {
          UNIMPLEMENTED ("tbw");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
          if (isDeaf())
          this->transmogrify (solution);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
        }
      
    protected:
      void maybe ( ) const;

      friend HashVal hash_value (Murps const& entry)
        {
          return hash_value (entry.nothing_);
        }
    };
  
  class HierarchyOrientationIndicator
    {
      string nothing_;

    public:
      explicit HierarchyOrientationIndicator (string const& b) :
          nothing_ (b)
        {
        }
      
      // using default copy/assignment
      
      /* == X interface for == */

      void putXY (string const& crap = "")
        {
          UNIMPLEMENTED ("tbw");
        }
      
    protected:
      void maybe ( ) const;

    public:
      HierarchyOrientationIndicator ( )
        {
          // ////TODO Auto-generated constructor stub
          
        }
      
      friend HashVal hash_value (HierarchyOrientationIndicator const& entry)
        {
//        return hash_value (entry.nothing_);
        }
    };
  
  /** @internal in case
   */
  template<class X>
  inline void Murps<X>::maybe ( ) const
    {
      UNIMPLEMENTED ("tbw");
    }

} // namespace lib
#endif /*HIERARCHY_ORIENTATION_INDICATOR_HPP_*/
