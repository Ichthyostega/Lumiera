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
#include "proc/mobject/session/session-impl.hpp"

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
  
  
  /* some type shorthands */
  typedef shared_ptr<PlacementIndex> PIdx;
  typedef PlacementIndex::PlacementMO PlacementMO; 
  typedef PlacementIndex::PRef PRef;
  typedef PlacementIndex::ID ID;
  
  
  PlacementMO&
  PlacementIndex::getRoot()  const
  {
    UNIMPLEMENTED ("managing the implicit root context within a scope hierarchy");
  }
  
  
  size_t
  PlacementIndex::size()  const
  {
    return pTab_->size() - 1;
  }
  
  
  bool
  PlacementIndex::contains (PlacementMO&)  const
  {
    UNIMPLEMENTED ("containment test: is the given Placement known within this index");
    return false;
  }
  
  
  
  
  namespace { // implementation detail: default global placement index access
    
    PIdx globalIndex (0);
    
    PIdx const&
    getGlobalIndex()
    {
      if (globalIndex)
        return globalIndex;
      else
        return session::SessManagerImpl::getCurrentIndex();
    }
    
  } // (End) implementation detail
  
  
  /** @internal factory function for creating a placement index for tests.  */
  PIdx
  create_PlacementIndex ()
  {
    return PIdx (new PlacementIndex());
  }
  
  PIdx const&
  reset_PlachementIndex(PIdx const& alternativeIndex)
  {
    globalIndex = alternativeIndex;
  }
  
  /** @internal restore the implicit PlacementIndex to its default implementation (=the session) */
  PIdx const&
  reset_PlachementIndex()
  {
    globalIndex.reset(0);
  }
  
  /** by default, this reaches for the PlacementIndex maintained within
   *  the current session. But for the purpose of unit tests, an alternative
   *  PlacementIndex may have been \link #reset_PlacementIndex installed \endlink 
   */
  Placement<MObject> &
  fetch_PlachementIndex(Placement<MObject>::ID const& pID)
  {
    return getGlobalIndex()->find (pID);
  }



} // namespace mobject
