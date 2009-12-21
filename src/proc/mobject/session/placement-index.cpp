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
 ** Implementation of core session storage structure.
 ** The PlacementIndex associates IDs to instances, and nested scope structure.
 ** Moreover, it provides and manages the actual Placement instances (storage),
 ** considered to be part of the session. 
 ** 
 ** Simple hash based implementation. Proof-of-concept and for fleshing out the API.
 ** The actual storage is provided by an embedded TypedAllocationManager instance, which
 ** is planned (as of 12/09) to be backed later by a memory pool based custom allocator. 
 ** 
 ** @todo change PlacementIndex into an interface and create a separated implementation class
 ** @todo really? it seems PlacementIndex has gotten an implementation class without much relevance on the Session API
 ** 
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */


#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-impl.hpp"
#include "proc/mobject/session/scope.hpp"
#include "lib/typed-allocation-manager.hpp"
//#include "proc/mobject/mobject.hpp" ///////////////////////TODO necessary?
#include "lib/util.hpp"


//#include <boost/format.hpp>
//using boost::str;
#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>
#include <tr1/unordered_map>
#include <tr1/functional>
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
//  using std::tr1::placeholders::_1;
  using std::tr1::function;
  using std::tr1::bind;
//using util::contains;
//using std::string;
//using std::map;
  using std::make_pair;
  using std::pair;
  
  using namespace lumiera;
  
  LUMIERA_ERROR_DEFINE (NOT_IN_SESSION, "referring to a Placement not known to the current session");
  LUMIERA_ERROR_DEFINE (PLACEMENT_TYPE, "requested Placement (pointee) type not compatible with data or context");
  LUMIERA_ERROR_DEFINE (NONEMPTY_SCOPE, "Placement scope (still) contains other elements");

  
  
  /* some type shorthands */
  typedef PlacementIndex::PlacementMO PlacementMO; 
  typedef PlacementIndex::PRef PRef;
  typedef PlacementIndex::ID ID;
  
  
  /**
   * Storage and implementation backing the PlacementIndex
   * - #placementTab_ is an hashtable mapping IDs to Placement + Scope
   * - #scopeTab_ is an reverse association serving to keep track of 
   *   any scope's contents
   * - root scope element is stored and maintained explicitly.
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
      typedef PlacementMO::ID PID;
      typedef unordered_map<PID, PlacementEntry, hash<PID> > IDTable;
      typedef std::tr1::unordered_multimap<PID,PID, hash<PID> > ScopeTable;
      
      typedef pair<ScopeIter, ScopeIter> ScopeContents;
      
      
      TypedAllocationManager allocator_;
      IDTable placementTab_;
      ScopeTable scopeTab_;
      
      PPlacement root_;
      
    public:
      Table () 
        { }
      
     ~Table ()
        {
          try 
            {
              root_.reset();
              clear(); 
            }
          catch(lumiera::Error& err)
            {
              WARN (session, "Problem while purging PlacementIndex: %s", err.what());
              lumiera_error(); // discard any error state
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
          PPlacement const& entry = base_entry(id).element;
          
          ENSURE (entry);
          ENSURE (id == entry->getID());
          return *entry;
        }
      
      PlacementMO&
      fetchScope (ID id)  const
        {
          REQUIRE (contains (id));
          PPlacement const& scope = base_entry(id).scope;
          
          ENSURE (scope);
          ENSURE (contains (scope->getID()));
          return *scope;
        }
      
      PlacementIndex::iterator
      queryScopeContents (ID id)  const
        {
          REQUIRE (contains (id));
          ScopeContents contents = scopeTab_.equal_range (id);
          UNIMPLEMENTED ("WIP-WIP-WIP");
//          return iterator (ScopeRangeIter(contents.first, contents.second)
//                          ,scopeIndexElementsResolver() );
        }
      
      
      
      void
      clear()
        {
          INFO (session, "Purging Placement Tables...");
          scopeTab_.clear();
          placementTab_.clear();
          
          if (root_)
            setupRoot (*root_);
        }
      
      
      /** insert a specially configured root entry into
       *  the yet empty table. Root is it's own scope 
       */
      void
      setupRoot (PlacementMO const& rootDef)
        {
          REQUIRE (0 == placementTab_.size());
          REQUIRE (0 == scopeTab_.size());
          REQUIRE (!root_);
          
          root_ = allocator_.create<PlacementMO> (rootDef);
          ID rootID = root_->getID();
          placementTab_[rootID].element = root_;
          placementTab_[rootID].scope   = root_;
          
          ENSURE (contains (rootID));
          ENSURE (scopeTab_.empty());
          ENSURE (1 == size());
        }
      
      PlacementMO&
      getRootElement()
        {
          REQUIRE (root_);
          REQUIRE (0 < size());
          REQUIRE (contains (root_->getID()));
          
          return *root_;
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
      addEntry (PlacementMO const& newObj, ID scopeID)
        {
          REQUIRE (contains (scopeID));
          
          PPlacement newEntry = allocator_.create<PlacementMO> (newObj);
          ID newID = newEntry->getID();
          
          ASSERT (newID, "invalid");
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
      typedef IDTable::const_iterator Slot;
      
      PlacementEntry const&
      base_entry (ID key)  const
        {
          Slot pos = placementTab_.find (key);
          if (pos == placementTab_.end())
            throw error::Logic("lost a Placement expected to be registered in the index.");
          
          return pos->second;
        }
      
      PlacementEntry
      remove_base_entry (ID key)
        {
          Slot pos = placementTab_.find (key);
          REQUIRE (pos != placementTab_.end());
          PlacementEntry dataToRemove (pos->second);
          placementTab_.erase(pos);
          return dataToRemove;
        }
      
      void
      remove_from_scope (ID scopeID, ID entryID)
        {
          typedef ScopeTable::const_iterator Pos;
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
      
      
      /** Helper for building a scope exploring iterator
       *  for PlacementIndex: our "reverse index" (scopeTab_)
       *  tracks the contents of each scope as pairs (scopeID,elementID).
       *  After fetching the range of matching entries, whenever the client
       *  dereferences the iterator, we have to pick up the second ID and
       *  resolve it through the main index table (placementTab_).
       */
      PlacementMO&
      resolveScopeIndexElement(pair<PID,PID> const& entry)  const
        {
          ID elemID (entry.second);
          ASSERT (contains (elemID));
          return fetch (elemID);
        }
      
      
      typedef function<PlacementMO& (pair<PID,PID> const&)> ElementResolver;
      mutable ElementResolver elementResolver_;
      
      ElementResolver
      scopeIndexElementsResolver()  const  ///< @return functor for iterator element access 
        {
          if (!elementResolver_)
            elementResolver_ = bind (&Table::resolveScopeIndexElement, this, _1 );
          return elementResolver_;
        }
      
    };
  
  
  
  
  
  PlacementIndex::PlacementIndex (PlacementMO const& rootDef)
    : pTab_(new Table)
    {
      pTab_->setupRoot(rootDef);
      ENSURE (isValid());
    }
  
  PlacementIndex::~PlacementIndex() { }
 

  PlacementMO&
  PlacementIndex::getRoot()  const
  {
    return pTab_->getRootElement();
  }
  
  
  /** validity self-check, used for sanity checks
   *  and the session self-check. */
  bool
  PlacementIndex::isValid()  const
  {
    UNIMPLEMENTED ("PlacementIndex validity self-check");
  }
  
  
  size_t
  PlacementIndex::size()  const
  {
    ASSERT (0 < pTab_->size());
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
  
  
  /** Retrieve all the elements attached to the given entry (scope)
   *  Each element (Placement) can act as a scope, containing other
   *  Placements, which will be discovered by this query one level
   *  deep (not recursive).
   *  @return an Lumiera Forward Iterator, yielding the children,
   *          possibly empty if the denoted element is a leaf.
   *  @note results are returned in arbitrary order (hashtable)
   */
  PlacementIndex::iterator
  PlacementIndex::getReferrers (ID id)  const
  {
    __check_knownID(*this, id);
    return pTab_->queryScopeContents(id);
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
  PlacementIndex::insert (PlacementMO const& newObj, ID targetScope)
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
    if (id == getRoot().getID())
      throw error::Fatal ("Request to kill the model root.");
    
    return pTab_->removeEntry (id);
  }
  
  
  void
  PlacementIndex::clear()
  {
    pTab_->clear();
  }
  
  
  
  


}} // namespace mobject::session
