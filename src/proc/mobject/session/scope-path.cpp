/*
  ScopePath  -  logical access path down from Session root
 
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
 
* *****************************************************/


#include "include/logging.h"
#include "proc/mobject/session/scope-path.hpp"
#include "proc/mobject/session/scope-locator.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/mobject.hpp"
#include "lib/util-foreach.hpp"
#include "lib/itertools.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"

#include <tr1/functional>
#include <algorithm>

namespace mobject {
namespace session {
  
  using std::reverse;
  
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  using lib::append_all;
  using util::and_all;
  
  using namespace lumiera;
  
  
  LUMIERA_ERROR_DEFINE (EMPTY_SCOPE_PATH, "Placement scope not locatable (empty model path)");
  
  
  
  namespace { // Helpers and shortcuts....
    
    /** issue a query to discover the path to root,
     *  starting with the given scope */
    inline ScopeQuery<MObject>::iterator
    discoverScopePath (Scope const& leaf)
    {
      return ScopeLocator::instance().locate<MObject> (leaf);   /////////////////////////////TICKET #663   create a single extension point to add meta-clip support later
    }
    
    
    void
    ___check_notBottom (const ScopePath *path, lib::Literal operation_descr)
    {
      REQUIRE (path);
      if (path->empty())
        throw error::Invalid (operation_descr+" an empty placement scope path"
                             , LUMIERA_ERROR_EMPTY_SCOPE_PATH);
    }
  }//(End) helpers
  
  
  
  
  /**
   * Create an \em empty path.
   * By default, a scope path just contains
   * the root scope of the current session (PlacementIndex).
   * @note invoking this function accesses the session and thus
   *       may cause an empty default session to be created.
   */
  ScopePath::ScopePath ()
    : refcount_(0)
    , path_()
  {
    clear();
  }
  
  
  /**
   * When creating a path to a given (leaf) scope,
   * the complete sequence of nested scopes leading to
   * this special scope is discovered, using the query service
   * exposed by the session (through ScopeLocator).
   * @note when locating the default (invalid) scope,
   *        a special empty ScopePath is created
   * @throw error::Invalid if the given target scope
   *        can't be connected to the (implicit) root
   */
  ScopePath::ScopePath (Scope const& leaf)
    : refcount_(0)
    , path_()
  {
    if (!leaf.isValid()) return; // invalid leaf defines invalid path....
    
    append_all (discoverScopePath(leaf), path_);   /////////////////////////////TICKET #663   extension point for meta-clip support
    reverse (path_.begin(), path_.end());
  }
  
  
  ScopePath::~ScopePath()
  {
    WARN_IF (refcount_, session, "Destroying a scope path frame with ref-count=%lu", refcount_);
  }
  
  
  /** constant \em invalid path token. Created by locating an invalid scope */
  const ScopePath ScopePath::INVALID = ScopePath(Scope());
  
  
  /** a \em valid path consists of more than just the root element.
   *  @note contrary to this, an \em empty path doesn't even contain a root element
   */
  bool
  ScopePath::isValid()  const
  {
    return (1 < length())
#ifndef NDEBUG
        && hasValidRoot()
#endif      
           ;
  }
  
  bool
  ScopePath::hasValidRoot()  const
  {
    REQUIRE (0 < length());
    return path_[0] == currModelRoot();
  }
  
  PlacementMO const&
  ScopePath::currModelRoot()  const
  {
    return SessionServiceExploreScope::getScopeRoot();
  }
  
  
  
  
  
  /* == Relations == */
  
  Scope const&
  ScopePath::getLeaf()  const
  {
    ___check_notBottom (this, "Inspecting");
    return path_.back();
  }
  
  
  /** verify the scope in question is equivalent
   *  to our leaf scope. Equivalence of scopes means
   *  they are defined by the same scope top placement,
   *  i.e. registered with the same Placement-ID.
   */
  bool
  ScopePath::endsAt(Scope const& aScope)  const
  {
    return aScope == getLeaf();
  }
  
  
  bool
  ScopePath::contains (Scope const& aScope)  const
  {
    for (iterator ii = this->begin(); ii; ++ii)
      if (aScope == *ii)
        return true;
    
    return false;
  }
  
  
  bool
  ScopePath::contains (ScopePath const& otherPath)  const
  {
    if ( empty())             return false;
    if (!otherPath.isValid()) return true;
    if (!isValid())           return false;
    
    ASSERT (1 < length());
    ASSERT (1 < otherPath.length());
    
    for (iterator ii = otherPath.begin(); ii; ++ii)
      if (!this->contains (*ii))
        return false;
    
    return true;
  }
  
  
  ScopePath
  commonPrefix (ScopePath const& path1, ScopePath const& path2)
  {
    typedef std::vector<Scope>::iterator VIter;
    ScopePath prefix (ScopePath::INVALID);
    uint len = std::min (path1.length(), path2.length());
    for (uint pos = 0; pos<len; ++pos)
      if (path1.path_[pos] == path2.path_[pos])
        prefix.appendScope (path1.path_[pos]);
    
    return prefix;
  }
  
  bool
  disjoint (ScopePath const& path1, ScopePath const& path2)
  {
    if (path1.empty() || path2.empty()) return false;
    
    return (path1.isValid() && path2.isValid())
        && (path1.path_[1]  != path2.path_[1]) // no common prefix
         ; 
  }
  
  
  
  
  /* == Mutations == */
  
  void
  ScopePath::clear()
  {
    path_.clear();
    path_.push_back (currModelRoot());
    
    ENSURE (!empty());
    ENSURE (!isValid());
    ENSURE ( hasValidRoot());
  }
  
  
  Scope&
  ScopePath::moveUp()
  {
    ___check_notBottom (this, "Navigating");
    static Scope invalidScope;
    
    path_.resize (length()-1);
    
    if (empty()) return invalidScope;
    else         return path_.back();
  }
  
  
  Scope&
  ScopePath::goRoot()
  {
    ___check_notBottom (this, "Navigating");
    ENSURE (hasValidRoot());
    
    path_.resize (1);
    return path_.back();
  }
  
  
  void
  ScopePath::navigate (Scope const& target)
  {
    ___check_notBottom (this, "Navigating");
    *this = ScopePath(target);             //////////////////////////////TICKET #424
                                          ///////////////////////////////TICKET #663   extension point for meta-clip support
  }
  
  
  void
  ScopePath::appendScope (Scope const& child)  ///< @internal backdoor for #commonPrefix
  {
    path_.push_back (child);
  }
  
  
  
  
  
}} // namespace mobject::session
