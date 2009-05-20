/*
  PlacementIndex  -  tracking individual Placements and their relations
 
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
 
* *****************************************************/


/** @file placement-index.cpp 
 ** 
 ** simple hash based implementation. Proof-of-concept
 ** and for fleshing out the API
 ** 
 ** @todo change PlacementIndex into an interface and create a separated implementation class
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */


#include "proc/mobject/placement-index.hpp"

//#include <boost/format.hpp>
//using boost::str;

namespace mobject {


  class PlacementIndex::Table 
    {
    public:
      Table () 
        { }
      
      size_t
      size()  const
        {
          UNIMPLEMENTED ("PlacementIndex datastructure");
          return 0;
        }
        
    };
  
  
  /** */
  
  
  size_t
  PlacementIndex::size()  const
  {
    return pTab_->size() - 1;
  }

  
  
  /** @internal factory function for creating a placement index for tests.
   */
  shared_ptr<PlacementIndex>
  createPlacementIndex ()
  {
    return shared_ptr<PlacementIndex> (new PlacementIndex());
  }
  


} // namespace mobject
