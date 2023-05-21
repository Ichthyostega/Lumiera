/*
  SCOPE-PATH.hpp  -  logical access path down from Session root

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scope-path.hpp
 ** An Object representing a sequence of nested scopes within the Session.
 ** MObjects are being attached to the model by Placements, and each Placement
 ** is added as belonging \em into another Placement, which defines the Scope
 ** of the addition. There is one (formal) root element, containing the timelines;
 ** from there a nested sequence of scopes leads down to each Placement.
 ** Ascending this path yields all the scopes to search or query in proper order
 ** to be used when resolving some attribute of placement. Placements use visibility
 ** rules comparable to visibility of scoped definitions in common programming languages
 ** or in cascading style sheets, where a local definition can shadow a global one.
 ** In a similar way, properties not defined locally may be resolved by querying
 ** up the sequence of nested scopes.
 ** 
 ** A scope path is represented as sequence of scopes, where each Scope is implemented
 ** by a PlacementRef pointing to the »scope top«, i.e. the placement in the session
 ** constituting this scope. The leaf of this path can be considered the current scope.
 ** ScopePath is intended to be used for remembering a \em current location within the
 ** model, usable for resolving queries and discovering contents.
 ** 
 ** \par operations and behaviour
 ** 
 ** In addition to some search and query functions, a scope path has the ability to 
 ** \em navigate to a given target scope, which must be reachable by ascending and
 ** descending into the branches of the overall tree or DAG (in the general case).
 ** Navigating is a mutating operation which usually happens when the current
 ** "focus" shifts while operating on the model.
 ** 
 ** - ScopePath can be default constructed, yielding an \em invalid path.
 ** - When created with a given target Scope, a connection to the current Session
 **   is created behind the scenes to discover the path starting from this target
 **   Scope up to model root. This is the core "locating" operation. It may fail.
 ** - There is a pre defined \c ScopePath::INVALID token
 ** - ScopePaths are intended to be handled <b>by value</b> (as are Scopes and
 **   PlacementRefs). They are equality comparable and provide several specialised
 **   relation predicates.
 ** - while generally copying is permitted, you may not overwrite an ScopePath
 **   which is attached (referred by a QueryFocus, see below)
 ** - all implementations are focused on clarity, not uttermost performance, as
 **   the assumption is for paths to be relatively short and path operations to
 **   be executed rather in a GUI action triggered context.
 ** - the iteration (Lumiera Forward Iterator) yields the path elements in
 **   \em ascending order, starting with the leaf element
 ** - a path containing just the root element evaluates to \c bool(false) 
 **   (rationale is that any valid, usable path is below just root).
 ** - an empty (nil) path doesn't even contain the root element and 
 **   may throw on many operations.
 ** 
 ** \par relation to ScopeLocator
 ** 
 ** ScopeLocator holds the QueryFocusStack, which contains ScopePath objects.
 ** Each of these stack frames represents the current location for some evaluation
 ** context; it is organised as stack to allow intermediate evaluations. Management
 ** of these stack frames is automated, with the assistance of ScopePath by
 ** incorporating a ref-count. Client code usually accesses this mechanism
 ** through QueryFocus objects as frontend, which is reflected in the
 ** mentioned embedded refcount
 ** 
 ** @see scope-path-test.cpp
 ** @see Scope
 ** @see ScopeLocator
 ** 
 */


#ifndef STEAM_MOBJECT_SESSION_SCOPE_PATH_H
#define STEAM_MOBJECT_SESSION_SCOPE_PATH_H

#include "steam/mobject/session/scope.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/error.hpp"

#include <vector>
#include <string>


namespace lib {
namespace meta{

  using steam::mobject::session::Scope;

  /**
   * this explicit specialisation allows to build a RangeIter
   * to yield const Scope elements, based on the const_reverse_iterator
   * used internally within ScopePath. This specialisation needs to be
   * injected prior to actually building the iterator type of ScopePath
   * @see value-type-binding.hpp
   * @see iter-adapter.hpp
   */
  template<>
  struct ValueTypeBinding<vector<Scope>::const_reverse_iterator>
    {
      typedef const Scope   value_type;
      typedef Scope const&  reference;
      typedef const Scope*  pointer;
    };
}}

namespace steam {
namespace mobject {
namespace session {
  
  
  LUMIERA_ERROR_DECLARE (EMPTY_SCOPE_PATH);  ///< Placement scope not locatable (empty model path)
  
  
  /**
   * Sequence of nested scopes within the high-level model.
   * Implemented as vector of Scope elements. Providing
   * state and relation predicates, and the ability to
   * \em navigate the current location, as represented
   * by the current leaf element of the path. 
   * 
   * Incorporates a ref count to be utilised by ScopeLocator
   * and QueryFocus to establish the \em current focus (path).
   */
  class ScopePath
    {
      size_t refcount_;
      std::vector<Scope> path_;
      
      typedef vector<Scope>                  _VType;
      typedef _VType::const_reverse_iterator _VIter;
      typedef lib::RangeIter<_VIter>      _IterType;
      
    public:
     ~ScopePath ();
      ScopePath ();
      ScopePath (Scope const& leaf);
      
      ScopePath (ScopePath const&);
      ScopePath&
      operator= (ScopePath const&);
      
      static const ScopePath INVALID;
      
      explicit operator bool()  const { return isValid(); }
      
      
      /* == state diagnostics == */
      bool isValid()    const;
      bool empty()      const;
      bool isRoot()     const;
      size_t size()     const;
      size_t length()   const;
      size_t ref_count()const;
      operator string() const;
      
      /// Iteration is always ascending from leaf to root
      typedef _IterType iterator;
      iterator begin()  const;
      iterator end()    const;
      
      
      /* == relations == */
      Scope const& getLeaf()           const;
      bool endsAt (Scope const&)       const;
      bool contains (Scope const&)     const;
      bool contains (ScopePath const&) const;
      
      friend ScopePath commonPrefix (ScopePath const&, ScopePath const&);
      friend bool      disjoint     (ScopePath const&, ScopePath const&);
      
      friend bool operator== (ScopePath const&, ScopePath const&);
      
      friend void intrusive_ptr_add_ref (ScopePath*);
      friend void intrusive_ptr_release (ScopePath*);
      
      
      /* == mutations == */
      void clear();
      Scope const& moveUp();
      Scope const& goRoot();
      void navigate (Scope const&);
      
      
    private:
      bool hasValidRoot()                 const;
      PlacementMO const& currModelRoot()  const;
      void appendScope (Scope const&);
    };
  
  
  
  
  
  
  
  inline bool
  operator== (ScopePath const& path1, ScopePath const& path2)
  {
    return path1.path_ == path2.path_;
  }
  
  inline bool
  operator!= (ScopePath const& path1, ScopePath const& path2)
  {
    return not (path1 == path2);
  }
  
  
  /** management function for boost::intrusive_ptr
   *  to be picked up by ADL
   */
  inline void
  intrusive_ptr_add_ref (ScopePath* pathFrame)
  {
    REQUIRE (pathFrame);
    ++(pathFrame->refcount_);
  }
  
  inline void
  intrusive_ptr_release (ScopePath* pathFrame)
  {
    REQUIRE (pathFrame);
    if (0 < pathFrame->refcount_)
      --(pathFrame->refcount_);
  }

  
  
  inline size_t
  ScopePath::ref_count()  const
  {
    return refcount_;
  }
  
  
  inline size_t
  ScopePath::length()  const
  {
    return path_.size();
  }
  
  
  inline size_t
  ScopePath::size()  const
  {
    return path_.size();
  }
  
  /** an empty path doesn't even contain a root element.
   *  Many operations throw when invoked on such a path.
   *  Navigating up from an root path creates an empty path.
   */
  inline bool
  ScopePath::empty()  const
  {
    return path_.empty();
  }
  
  inline bool
  ScopePath::isRoot() const
  {
    return (1 == size())
#if NOBUG_MODE_ALPHA
        && path_[0].isRoot()
#endif
        ;
  }
  
  
  
  /** @note actually this is an Lumiera Forward Iterator,
   *  yielding the path up to root as a sequence of
   *  const Scope elements */
   inline ScopePath::iterator
  ScopePath::begin()  const
  {
    return iterator (path_.rbegin(), path_.rend());
  }
  
  inline ScopePath::iterator
  ScopePath::end()    const
  {
    return iterator();
  }
  
  
}}} // namespace mobject::session
#endif
