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


#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-impl.hpp"
#include "proc/mobject/session/scope.hpp"
#include "lib/typed-allocation-manager.hpp"
#include "proc/mobject/mobject.hpp" ///////////////////////TODO necessary?
#include "lib/util.hpp"


//#include <boost/format.hpp>
//using boost::str;
#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>
#include <tr1/unordered_map>
#include <tr1/memory>
#include <string>
//#include <map>


namespace mobject {
namespace session {

  using boost::hash;
  using boost::noncopyable;
  using std::tr1::shared_ptr;
  using std::tr1::unordered_map;
  using std::tr1::unordered_multimap;
  using lib::TypedAllocationManager;
//using util::getValue_or_default;
//using util::contains;
//using std::string;
//using std::map;
  using std::make_pair;
  
  using namespace lumiera;
  
  LUMIERA_ERROR_DEFINE (NOT_IN_SESSION, "referring to a Placement not known to the current session");
  
  
  namespace { // implementation helpers

    template <typename MAP>
    inline typename MAP::mapped_type const&
    getEntry_or_throw (MAP& map, typename MAP::key_type const& key)
    {
      typename MAP::const_iterator pos = map.find (key);
      if (pos == map.end())
        throw error::Logic("lost a Placement expected to be registered in the index.");
      
      return pos->second;
    }
      
  } // (End) impl.helpers
  
  
  /* some type shorthands */
  typedef PlacementIndex::PlacementMO PlacementMO; 
  typedef PlacementIndex::PRef PRef;
  typedef PlacementIndex::ID ID;
  
  
  class PlacementIndex::Table 
    {
      typedef shared_ptr<PlacementMO> PPlacement;
      
      // using a hashtables to implement the index
      typedef unordered_map<ID, PPlacement, hash<ID> > IDTable;
      typedef std::tr1::unordered_multimap<ID,ID, hash<ID> > ScopeTable;
      
      
      TypedAllocationManager allocator_;
      IDTable placementTab_;
      ScopeTable scopeTab_;
      
      
    public:
      Table () 
        { }
      
     ~Table ()
        {
          try { clear(); }
          catch(lumiera::Error& err)
            {
              WARN (session, "Problem while purging PlacementIndex: %s", err.what());
              lumiera_error();
        }   }
      
      
      size_t
      size()  const
        {
          return placementTab_.size();
        }
      
      bool
      contains (ID id)  const
        {
          return util::contains(placementTab_, id);
        }
      
      PlacementMO&
      fetch (ID id)  const
        {
          REQUIRE (contains (id));
          PPlacement const& entry = getEntry_or_throw (placementTab_,id);
          
          ENSURE (entry);
          ENSURE (id == entry->getID());
          return *entry;
        }
      
      
      void
      clear()
        {
          INFO (session, "Purging Placement Tables...");
          scopeTab_.clear();
          placementTab_.clear();
        }
        
      ID
      addEntry (PlacementMO const& newObj, PlacementMO const& targetScope)
        {
          ID scopeID = targetScope.getID();
          REQUIRE (contains (scopeID));
          
          /////////////////////////////////////////////////////////////////////TICKET #436
          PPlacement newEntry = allocator_.create<PlacementMO> (newObj);
          ID newID = newEntry->getID();
          
          ASSERT (!contains (newID));
          placementTab_[newID] = newEntry;
          scopeTab_.insert (make_pair (scopeID, newID));
          return newID;
        }
      
    };
  
  
  
  
  /** @internal Factory for creating a new placement index.
   *            For use by the Session and for unit tests.  
   */
  PlacementIndex::Factory PlacementIndex::create;
  
  PlacementIndex::PlacementIndex()
    : pTab_(new Table)
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
    return pTab_->contains (id);
  }
  
  
  PlacementMO&
  PlacementIndex::find (ID id)  const
  {
    if (!contains (id))
      throw error::Invalid ("Accessing Placement not registered within the index"
                           ,LUMIERA_ERROR_NOT_IN_SESSION);              ///////////////////////TICKET #197
    
    return pTab_->fetch (id);
  }
    
  
  
  PlacementMO&
  PlacementIndex::getScope (ID)  const
  {
    UNIMPLEMENTED ("Secondary core operation of PlacmentIndex: find the 'parent' Placement by using the Placement relation index");
    /// decision: root is his own scope
  }
  
  
  PlacementIndex::iterator
  PlacementIndex::getReferrers (ID)  const
  {
    UNIMPLEMENTED ("query the Placement relation index and retrieve all other placements bound to this one by a placement-relation");
    // do a query using equal_range of the hashtable (unordered_multimap)
    // build a RangeIter from them
    // use this to build an auto-fetching IterAdapter
  }
  
  
  /** Add a new Placement (Object "instance") into the index.
   *  Usually this means effectively adding this "Object" to the Session.
   *  The given Placement is copied into the storage managed within the session.
   *  This copy within the storage is what will be "the placement of this object".
   *  It can be discovered as index (Session) content, re-accessed by the ID returned
   *  from this call and modified in the course of editing the session. 
   *  @param newObj reference placement pointing to the MObject to be added
   *  @param targetScope ref to a placement already added to the index, serving as
   *         container "into" which the new placement will be located
   *  @return placement ID of the newly added Placement
   *  @note the newly added Placement has an identity of its own.
   */
  ID
  PlacementIndex::insert (PlacementMO const& newObj, PlacementMO const& targetScope)
  {
    if (!contains (targetScope))
      throw error::Logic ("Specified a non-registered Placement as scope "
                          "while adding another Placement to the index"
                         ,LUMIERA_ERROR_INVALID_SCOPE);              ////////////////TICKET #197
      
    return pTab_->addEntry(newObj, targetScope);
  }
  
  
  bool
  PlacementIndex::remove (ID)
  {
    UNIMPLEMENTED ("remove a information record from PlacementIndex, and also de-register any placement-relations bound to it");
  }
  
  
  void
  PlacementIndex::clear()
  {
    pTab_->clear();
  }
  
  
  
  


}} // namespace mobject::session
