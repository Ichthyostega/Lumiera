/*
  PlacementIndex  -  tracking individual Placements and their relations

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file placement-index.cpp 
 ** Implementation of core session storage structure.
 ** The PlacementIndex associates IDs to instances, within a nested scope structure.
 ** Moreover, it provides and manages the actual Placement instances (storage),
 ** considered to be part of the session.
 ** 
 ** Simple hash based implementation. Seems adequate for now (12/09).
 ** A main table associates Placement-ID to a Placement \em instance, which is contained
 ** and managed within this index. A second hashtable allows to reverse lookup the scope
 ** associations, especially for enumerating the contents of a scope. The latter is done
 ** by wrapping up an STL iterator range into a "Lumiera Forward Iterator" (adapter).
 ** Generally speaking, PlacementIndex is an implementation level facility and provides
 ** the basic/low-level functionality. For example, the PlacementIndexQueryResolver 
 ** provides depth-first exploration of all the contents of an scope, including
 ** nested scopes, building on top of the scope iterators from PlacementIndex.
 ** 
 ** PlacementIndex can be seen as the core datastructure of the session. Objects are attached
 ** to the session by adding (copying) a Placement instance, which is owned and managed by
 ** the PlacementIndex. Adding this Placement instance creates a new Placement-ID, which
 ** from then on acts as a shorthand for "the object instance" within the session.
 ** The actual storage is provided by an embedded TypedAllocationManager instance, which
 ** is planned (as of 12/09) to be backed later by a memory pool based custom allocator.
 ** 
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */


#include "steam/mobject/session/placement-index.hpp"
#include "steam/mobject/session/session-impl.hpp"
#include "steam/mobject/session/scope.hpp"
#include "lib/typed-allocation-manager.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/util-foreach.hpp"
#include "lib/iter-source.hpp"
#include "include/logging.h"

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>


namespace proc {
namespace mobject {
namespace session {
  
  using boost::hash;
  using std::shared_ptr;
  using std::unordered_map;
  using std::unordered_multimap;
  using lib::TypedAllocationManager;
  using lib::iter_stl::IterSnapshot;
  using lib::iter_stl::eachVal;
  using lib::iter_source::eachMapKey;
  using lib::iter_source::eachDistinctKey;
  using lib::iter_source::eachValForKey;
  using std::placeholders::_1;
  using std::function;
  using std::bind;
  using std::make_pair;
  using std::pair;
  
  using util::for_each;
  using util::has_any;
  
  LUMIERA_ERROR_DEFINE (NOT_IN_SESSION, "referring to a Placement not known to the current session");
  LUMIERA_ERROR_DEFINE (PLACEMENT_TYPE, "requested Placement (pointee) type not compatible with data or context");
  LUMIERA_ERROR_DEFINE (NONEMPTY_SCOPE, "Placement scope (still) contains other elements");
  
  
  
  /* some type shorthands */
  typedef mobject::PlacementMO PlacementMO; 
  typedef PlacementIndex::PRef PRef;
  typedef PlacementIndex::ID ID;
  
  
  /*****************************************************************//**
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
      
      // using hashtables to implement the index
      typedef PlacementMO::ID PID;
      typedef unordered_map<PID, PlacementEntry>  IDTable;
      typedef std::unordered_multimap<PID,PID> ScopeTable;
      
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
      size()  const            ///<@note always at least 1 because of root
        {
          return placementTab_.size();
        }
            
      size_t
      scope_cnt()  const       ///<@note root doesn't produce an scope entry
        {
          return scopeTab_.size();
        }
            
      size_t
      element_cnt()  const
        {
          return allocator_.numSlots<PlacementMO>();
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
          return iterator (ScopeRangeIter(contents.first, contents.second)
                          ,scopeIndexElementsResolver() );
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
       *  within the index, together with the Scope
       *  this Placement belongs to.
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
      
      void
      removeAll (ID scopeID)
        {
          remove_all_from_scope (scopeID); // recursive
          removeEntry (scopeID);          //  discard top-level
          
          ENSURE (!util::contains(scopeTab_, scopeID));
          ENSURE (!contains (scopeID));
        }
      
      
      /* == access for self-test == */
      
      using IDIter = lib::IterSource<PID>::iterator;
      
      PlacementMO* _root_4check ()        { return root_.get();                 }
      PlacementMO* _element_4check (ID id){ return base_entry(id).element.get();}
      PlacementMO* _scope_4check (ID id)  { return base_entry(id).scope.get();  }
      IDIter       _eachEntry_4check ()   { return eachMapKey (placementTab_);  }
      IDIter       _eachScope_4check ()   { return eachDistinctKey (scopeTab_); }
      IDIter       _contents_4check(ID id){ return eachValForKey (scopeTab_,id);}
      
      
    private:
      typedef IDTable::const_iterator Slot;
      
      PlacementEntry const&
      base_entry (ID key)  const
        {
          Slot pos = placementTab_.find (key);
          if (pos == placementTab_.end())
            throw error::Logic("lost a Placement expected to be registered within PlacementIndex.");
          
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
      
      void
      remove_all_from_scope (ID scopeID)
        {
          typedef ScopeTable::const_iterator Pos;
          pair<Pos,Pos> scopeEntries = scopeTab_.equal_range(scopeID);
          Pos first = scopeEntries.first;
          Pos end   = scopeEntries.second;
          
          // take a snapshot of all children to be processed recursively
          typedef IterSnapshot<PID> ChildIDs;
          ChildIDs child (eachVal(first,end));
          
          scopeTab_.erase (first,end); // assumed to be NOP for first==end
          
          for ( ; child; ++child )
            {
              remove_all_from_scope (*child); // recursive
              remove_base_entry (*child);    //  discard storage
              
              ENSURE (!util::contains(scopeTab_, *child));
              ENSURE (!contains (*child));
            }
        }
      
      
      
      /** Helper for building a scope exploring iterator
       *  for PlacementIndex: our "reverse index" (`scopeTab_`)
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
   //(End) placement index table implementation
  
  
  
  
  
  
  
  /* ============ PlacementIndex implementation functions ============ */
  
  
  PlacementIndex::PlacementIndex (PlacementMO const& rootDef)
    : pTab_(new Table)
    {
      INFO (session, "Initialising PlacementIndex...");
      
      pTab_->setupRoot(rootDef);
      ENSURE (isValid());
    }
  
  PlacementIndex::~PlacementIndex() { }
  
  
  PlacementMO&
  PlacementIndex::getRoot()  const
  {
    return pTab_->getRootElement();
  }
  
  
  size_t
  PlacementIndex::size()  const
  {
    ASSERT (0 < pTab_->size());
    return pTab_->size() - 1;  // root not counted
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
   *  Each element (Placement) can act as a scope, containing other Placements,
   *  which will be discovered by this query one level deep (not recursive).
   *  @return a Lumiera Forward Iterator, yielding the children,
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
                         ,LERR_(INVALID_SCOPE));                          ////////////////TICKET #197
      
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
  
  
  /** recursively kill a complete scope,
   *  including the given element and all children.
   *  @note as an exception, when specifying model root,
   *        any sub-elements are cleared but root is retained
   */
  void
  PlacementIndex::clear (ID targetScope)
  {
    if (targetScope == getRoot().getID())
      pTab_->clear();
    else
      pTab_->removeAll (targetScope);
    
    ENSURE (isValid());
  }
  
  
  void
  PlacementIndex::clear()
  {
    pTab_->clear();
  }
  
  
  
  
  
  /* ====== PlacementIndex validity self-check ====== */
  
  namespace { // self-check implementation helpers...
    
    LUMIERA_ERROR_DEFINE(INDEX_CORRUPTED, "PlacementIndex corrupted");
    
    struct SelfCheckFailure
      : error::Fatal
      {
        SelfCheckFailure (lib::Literal currentTest, string failure)
          : error::Fatal (string("Failed test: ")+currentTest+ " : "+failure
                         ,LUMIERA_ERROR_INDEX_CORRUPTED)
          { }
      };
  }
  
  
  /** 
   * PlacementIndex self-verification code
   * Executes all built-in checks automatically
   * on object creation.
   * @throw SelfCheckFailure   
   */
  class PlacementIndex::Validator
    {
      
      PlacementIndex::Table& tab;
      
      PlacementMO* elm (ID id) { return tab._element_4check (id);}
      PlacementMO* sco (ID id) { return tab._scope_4check (id);  }
      
      
      
#define VERIFY(_CHECK_, CHECK_ID, DESCRIPTION) \
      if (!(_CHECK_))                           \
        throw SelfCheckFailure (CHECK_ID, (DESCRIPTION));
      
      
      void
      checkRoot (PMO* root)
        {
          VERIFY ( root,                 "(0.1) Basics",   "Root element missing");
          VERIFY ( root->isValid(),      "(0.2) Basics",   "Root Placement invalid");
          VERIFY ( (*root)->isValid(),   "(0.3) Basics",   "Root MObject self-check failure");
        }
      
      void
      checkEntry (ID id)
        {
          VERIFY ( tab.contains(id),     "(1.1) Elements", "PlacementIndex main table corrupted");
          VERIFY ( elm(id),              "(1.2) Elements", "Entry doesn't hold a Placement");
          VERIFY ( id==elm(id)->getID(), "(1.3) Elements", "Element stored with wrong ID");      ////////////////TICKET #197
          VERIFY ( elm(id)->isValid(),   "(1.4) Elements", "Index contains invalid Placement")
          VERIFY ( sco(id),              "(1.5) Elements", "Entry has undefined scope");
          VERIFY ( sco(id)->isValid(),   "(1.6) Elements", "Entry has invalid scope");
          VERIFY ( tab.contains (sco(id)->getID()),
                                         "(1.7) Elements", "Element associated with an unknown scope");
          
          PMO& theElement = *elm(id);
          ID theScope (sco(id)->getID());
          if (theScope == id
             && elm(id)==tab._root_4check()
             )        // no need to check root,
            return;  //  because root is it's own scope
          
          iterator elementsInScope = tab.queryScopeContents(theScope);
          auto equalsTheElement   = [&](PMO& entry) { return entry == theElement; };
          bool properlyRegistered = has_any (elementsInScope, equalsTheElement);
          
          VERIFY ( properlyRegistered,   "(1.8) Elements", "Element not registered as member of the enclosing scope: "+ theElement);
        }
      
      void
      checkScope (ID id)
        {
          VERIFY ( tab.contains(id),     "(2.1) Scopes",   "Scope not registered in main table");
          VERIFY ( elm(id),              "(2.2) Scopes",   "Scope entry doesn't hold a Placement");
          VERIFY ( sco(id),              "(2.3) Scopes",   "Scope entry doesn't hold a containing Scope");
          
          PMO* root  = tab._root_4check();
          PMO* scope = sco(id);
          while (scope && scope != sco(scope->getID()))
            scope = sco(scope->getID());
          
          VERIFY ( root==scope,          "(2.4) Scopes",   "Found a scope not attached below root.");
          
          for_each ( tab._contents_4check(id), &Validator::checkScopeEntry, this, id, _1 );
        }
      
      void
      checkScopeEntry (ID scope, ID entry)
        {
          VERIFY ( tab.contains(entry),  "(2.5) Scopes",   "Scope member not registered in main table");
          VERIFY ( elm(entry),           "(2.6) Scopes",   "Scope member entry doesn't refer to a valid Placement");
          VERIFY ( sco(entry),           "(2.7) Scopes",   "Scope member entry is lacking valid scope information");
          VERIFY ( sco(entry)->getID() == scope,
                                         "(2.8) Scopes",   "Scope member registered as belonging to a different scope in main table");
        }
      
      void
      checkAllocation ()
        {
          VERIFY ( 0 < tab.size(),       "(4.1) Storage",  "Implementation table is empty");
          VERIFY ( 0 < tab.element_cnt(),"(4.2) Storage",  "No Placement instances allocated");
          VERIFY ( tab.size()==tab.scope_cnt()+1,
                                         "(4.3) Storage",  "Number of elements and scope entries disagree");
          VERIFY ( tab.size()==tab.element_cnt(),
                                         "(4.4) Storage",  "Number of entries doesn't match number of allocated Placement instances");
        }
      
      
      public:
        Validator (Table& indexTable)
          : tab(indexTable)
          {
            checkRoot (tab._root_4check());
            
            for_each ( tab._eachEntry_4check(), &Validator::checkEntry, this, _1 );
            for_each ( tab._eachScope_4check(), &Validator::checkScope, this, _1 );
            
            checkAllocation();
          }
      
    };//(End) Validator (PlacementIndex self-check implementation)
  
  
  
  
    

  /** validity self-check, used for sanity checks and the session self-check.
   *  The following checks are performed (causing at least one full table scan)
   *  - root element exists and is valid.
   *  - each element
   *    - has a known scope
   *    - is registered as child of it's scope
   *  - can reach root from each scope
   *  - element count of the allocator matches table size
   */
  bool
  PlacementIndex::isValid()  const
  {
    try
      {
        VERIFY ( pTab_, "(0) Basics" ,"Implementation tables not initialised");
        
        Validator verify(*pTab_);
        return true;
      }
    
    catch(SelfCheckFailure& failure)
      {
        lumiera_error();
        ERROR (session, "%s", failure.what());
      }
    return false;
  }
  
#undef VERIFY
  
  
  
}}} // namespace proc::mobject::session
