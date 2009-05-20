/*
  PLACEMENT-REF.hpp  -  generic reference to an individual placement added to the session
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file placement-ref.hpp 
 **
 ** @see Placement
 ** @see PlacementRef_test
 **
 */



#ifndef MOBJECT_PLACEMENT__REF_H
#define MOBJECT_PLACEMENT__REF_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"

//#include <tr1/memory>


namespace mobject {

//  using std::tr1::shared_ptr;
  
  
  
  /**
   */
  class PlacementRef
    {
    public:
      /**
       * 
       */
      class ID
        {
        };
      
      template<class MO>
      class IDp : public ID
        {
        };
      
      
    };
  
  
  
  
  


} // namespace mobject
#endif
