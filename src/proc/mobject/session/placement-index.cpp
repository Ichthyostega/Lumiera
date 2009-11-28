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
//#include <algorithm>
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
  using util::getValue_or_default;
//using util::contains;
//using std::string;
//using std::map;
  using std::make_pair;
  using std::pair;
  
  using namespace lumiera;
  
  LUMIERA_ERROR_DEFINE (NOT_IN_SESSION, "referring to a Placement not known to the current session");
  LUMIERA_ERROR_DEFINE (PLACEMENT_TYPE, "requested Placement (pointee) type not compatible with data or context");
  LUMIERA_ERROR_DEFINE (NONEMPTY_SCOPE, "Placement scope (still) contains other elements");

  
  
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
  
  
  /**
   * Storage and implementation
   * backing the PlacementIndex
   */
  class PlacementIndex::Table 
    {
      typedef shared_ptr<PlacementMO> PPlacement;
      
      struct PlacementEntry
        {
          PPlacement element;
          PPlacement scope;
        };
      
      // using a hashtables to implement the index
      typedef unordered_map<ID, PlacementEntry, hash<ID> > IDTable;
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
          PPlacement const& entry = getEntry_or_throw (placementTab_,id).element;
          
          ENSURE (entry);
          ENSURE (id == entry->getID());
          return *entry;
        }
      
      PlacementMO&
      fetchScope (ID id)  const
        {
          REQUIRE (contains (id));
          PPlacement const& scope = getEntry_or_throw (placementTab_,id).scope;
          
          ENSURE (scope);
          ENSURE (contains (scope->getID()));
          return *scope;
        }
      
      
      
      void
      clear()
        {
          INFO (session, "Purging Placement Tables...");
          scopeTab_.clear();
          placementTab_.clear();
        }
        
      /** Store a copy of the given Placement as new instance
       *  within the index, together with the Scope this Placement
       *  belongs to. 
       * @note we discard the specific type info.
       *       It can be rediscovered later with the help
       *       of the pointee's vtable
       * @see Placement#isCompatible      
       */ 
      ID
      addEntry (PlacementMO const& newObj, PlacementMO const& targetScope)
        {
          ID scopeID = targetScope.getID();
          REQUIRE (contains (scopeID));
          
          PPlacement newEntry = allocator_.create<PlacementMO> (newObj);
          ID newID = newEntry->getID();
          
          ASSERT (!contains (newID));
          placementTab_[newID].element = newEntry;
          placementTab_[newID].scope   = placementTab_[scopeID].element;
          scopeTab_.insert (make_pair (scopeID, newID));
          return newID;
        }
      
      bool
      removeEntry (ID id)
        {
          if (!contains (id))
            {
              ENSURE (!util::contains(scopeTab_, id));
              return false;
            }
          
          if (util::contains(scopeTab_, id))
            throw error::State ("Unable to remove the specified Placement, "
                                "because it defines an non-empty scope. "
                                "You need to delete any contents first."
                               ,LUMIERA_ERROR_NONEMPTY_SCOPE);              ////////////////TICKET #197
          
          ASSERT (contains (id));
          PlacementEntry toRemove = remove_base_entry (id);
          remove_from_scope (toRemove.scope->getID(), id);
          ENSURE (!util::contains(scopeTab_, id));
          ENSURE (!contains (id));
          return true;
        }
      
      
    private:
      PlacementEntry
      remove_base_entry (ID key)
        {
          IDTable::iterator pos = placementTab_.find (key);
          REQUIRE (pos != placementTab_.end());
          PlacementEntry dataToRemove (pos->second);
          placementTab_.erase(pos);
          return dataToRemove;
        }
      
      void
      remove_from_scope (ID scopeID, ID entryID)
        {
          typedef ScopeTable::iterator Pos;
          pair<Pos,Pos> searchRange = scopeTab_.equal_range(scopeID);
          
          Pos pos = searchRange.first;
          Pos end = searchRange.second;
          for ( ; pos!=end; ++pos)
            if (pos->second == entryID)
              {
                scopeTab_.erase(pos);
                return;
              }
          
          NOTREACHED();
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
    __check_knownID(*this,id);
    return pTab_->fetch (id);
  }
    
  
  
  /** retrieve the Scope information
   *  registered alongside with the denoted Placement.
   *  @throw error::Invalid when the given ID isn't registered
   *  @note root is it's own scope, per definition. 
   */
  PlacementMO&
  PlacementIndex::getScope (ID id)  const
  {
    __check_knownID(*this,id);
    return pTab_->fetchScope (id);
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
  
  
  /** Remove and discard a Placement (Object "instance") from the index.
   *  Usually this means removing this Object from the session.
   *  @return \c true if actually removed an object.
   *  @throw error::State if the object to be removed is an non-empty scope
   */
  bool
  PlacementIndex::remove (ID id)
  {
    return pTab_->removeEntry (id);
  }
  
  
  void
  PlacementIndex::clear()
  {
    pTab_->clear();
  }
  
  
  
  


}} // namespace mobject::session
