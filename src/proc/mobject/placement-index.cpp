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
  
  
  /** @internal Factory for creating a new placement index.
   *            For use by the Session and for unit tests.  
   */
  PlacementIndex::Factory PlacementIndex::create;
  
  PlacementIndex::PlacementIndex()
    : pTab_()
    { }
  
  PlacementIndex::~PlacementIndex() { }
 

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
  PlacementIndex::contains (ID id)  const
  {
    UNIMPLEMENTED ("containment test: is the given Placement known within this index");
    return false;
  }
  
  
  PlacementMO&
  PlacementIndex::find (ID)  const
  {
    UNIMPLEMENTED ("main operation of PlacmentIndex: lookup a Placement by ID");
  }
    
  
  
  PlacementMO&
  PlacementIndex::getScope (ID)  const
  {
    UNIMPLEMENTED ("Secondary core operation of PlacmentIndex: find the 'parent' Placement by using the Placement relation index");
  }
  
  
  vector<PRef>
  PlacementIndex::getReferrers (ID)  const
  {
    UNIMPLEMENTED ("query the Placement relation index and retrieve all other placements bound to this one by a placement-relation");
  }
  
  
  ID
  PlacementIndex::insert (PlacementMO& newObj, PlacementMO& targetScope)
  {
    UNIMPLEMENTED ("store a new information record into PlacmentIndex: ID -> (ref-to-Placement, parent-Placement)");
  }
  
  
  bool
  PlacementIndex::remove (ID)
  {
    UNIMPLEMENTED ("remove a information record from PlacementIndex, and also deregister any placement-relations bound to it");
  }
  
  
  
  
  namespace { // implementation detail: default global placement index access
    
    PIdx globalIndex;
    
    PIdx const&
    getGlobalIndex()
    {
      if (globalIndex)
        return globalIndex;
      else
        return session::SessManagerImpl::getCurrentIndex();
    }
    
  } // (End) implementation detail
  
  
  
  
  
  void
  reset_PlacementIndex (PIdx const& alternativeIndex)
  {
    globalIndex = alternativeIndex;
  }
  
  /** @internal restore the implicit PlacementIndex to its default implementation (=the session) */
  void
  reset_PlacementIndex()
  {
    globalIndex.reset();
  }
  
  /** by default, this reaches for the PlacementIndex maintained within
   *  the current session. But for the purpose of unit tests, an alternative
   *  PlacementIndex may have been \link #reset_PlacementIndex installed \endlink 
   */
  Placement<MObject> &
  fetch_PlacementIndex (Placement<MObject>::ID const& pID)
  {
    return getGlobalIndex()->find (pID);
  }
  
  /** @internal used by PlacementRef to implement a self-check */
  bool
  checkContains_PlacementIndex (Placement<MObject>::ID const& pID)
  {
    return getGlobalIndex()->contains (pID);
  }


} // namespace mobject
