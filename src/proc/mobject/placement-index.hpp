/*
  PLACEMENT-INDEX.hpp  -  tracking individual Placements and their relations
 
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


/** @file placement-index.hpp 
 **
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */



#ifndef MOBJECT_PLACEMENT_INDEX_H
#define MOBJECT_PLACEMENT_INDEX_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"
#include "proc/mobject/placement-ref.hpp"

//#include <tr1/memory>
#include <vector>


namespace mobject {

//  using std::tr1::shared_ptr;
  using std::vector;

  
  
  /**
   */
  class PlacementIndex
    {
      public:
        typedef Placement<MObject> PlacementMO;
        typedef PlacementRef PRef;
        typedef PRef::ID ID;
        
        PlacementMO& find (ID)  const;
        
        template<class MO>
        Placement<MO>& find (PRef::IDp<MO>)  const;
        
        PlacementMO& getScope (PlacementMO&)  const;
        PlacementMO& getScope (ID)            const;
        
        vector<PRef> getReferrers (ID)        const;
        
        
        /** diagnostic: number of indexed entries */
        size_t size()  const;
        
        
        /* == mutating operations == */
        
        ID   insert (PlacementMO& newObj, PlacementMO& targetScope);
        bool remove (PlacementMO&);
        bool remove (ID);
    };
  ////////////////TODO currently just fleshing  out the API; probably have to splitt off an impl.class
  
  
  
  


} // namespace mobject
#endif
