/*
  ScopePath  -  logical access path down from Session root

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file scope-path.cpp
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "include/logging.h"
#include "steam/mobject/session/scope-path.hpp"
#include "steam/mobject/session/scope-locator.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/mobject.hpp"
#include "lib/itertools.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <algorithm>

namespace steam {
namespace mobject {
namespace session {
  
  using std::reverse;
  using lib::append_all;
  using util::isSameObject;
  using util::isnil;
  
  
  namespace { // Helpers and shortcuts....
    
    /** issue a query to discover the (raw) path to root,
     *  starting with the given scope */
    inline ScopeQuery<MObject>::iterator
    discoverScopePath (Scope const& leaf)
    {
      return ScopeLocator::instance().getRawPath (leaf);
    }
    
    
    void
    ___check_notBottom (const ScopePath *path, lib::Literal operation_descr)
    {
      REQUIRE (path);
      if (path->empty())
        throw error::Logic (operation_descr+" an empty placement scope path"
                           , LERR_(EMPTY_SCOPE_PATH));
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
    if (leaf == Scope::INVALID) return; // invalid leaf defines invalid path....
    
    clear();
    navigate (leaf);
  }
  
  
  ScopePath::ScopePath (ScopePath const& o)
    : refcount_(0)
    , path_(o.path_)
  { }
  
  /**
   * Copy from existing path
   * @throw error::Logic when current path has nonzero refcount
   */
  ScopePath&
  ScopePath::operator= (ScopePath const& ref)
  {
    if (0 < refcount_)
      throw error::Logic ("Attempt to overwrite a ScopePath with nonzero refcount");
    
    if (!isSameObject (*this, ref))
      {
        path_ = ref.path_;
        ENSURE (0 == refcount_);
      }
    return *this;
  }
  
  
  ScopePath::~ScopePath()
  {
    WARN_IF (refcount_, session, "Destroying a scope path frame with ref-count=%zu", refcount_);
  }
  
  
  /** constant \em invalid path token. Created by locating an invalid scope */
  const ScopePath ScopePath::INVALID = ScopePath(Scope());
  
  
  /** ScopePath diagnostic self display.
   *  Implemented similar to a filesystem path, where the
   *  path elements are based on the self-display of the MObject
   *  attached through the respective scope top placement. */
  ScopePath::operator string()  const
  {
    if (isnil (path_)) return "!";
    if (1 == length()) return "/";
    
    string res;
    vector<Scope>::const_iterator node (path_.begin());
    while (++node != path_.end())
      {
        res += "/";
        res += string (*node);
      }
    return res;
  }
  
  
  /** a \em valid path consists of more than just the root element.
   *  @note contrary to this, an \em empty path doesn't even contain a root element
   */
  bool
  ScopePath::isValid()  const
  {
    return (1 < length())
#if NOBUG_MODE_ALPHA
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
    if (aScope == Scope::INVALID) return true; // bottom is contained everywhere
    
    for (iterator ii = this->begin(); ii; ++ii)
      if (aScope == *ii)
        return true;
    
    return false;
  }
  
  
  bool
  ScopePath::contains (ScopePath const& otherPath)  const
  {
    if (!otherPath.isValid()) return true;
    if ( empty())             return false;
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
  
  
  Scope const&
  ScopePath::moveUp()
  {
    ___check_notBottom (this, "Navigating");
    
    path_.resize (length()-1);
    
    if (empty()) return Scope::INVALID;
    else         return path_.back();
  }
  
  
  Scope const&
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
    if (!target.isValid())
      throw error::Invalid ("can't navigate to a target scope outside the model"
                           , LERR_(INVALID_SCOPE));
    
    std::vector<Scope> otherPath;
    append_all (discoverScopePath(target), otherPath);
    reverse (otherPath.begin(), otherPath.end());
                                        ////////////////////////////TICKET #663   extension point for meta-clip support
    ASSERT (path_[0] == otherPath[0]); // sharing the root element
    this->path_ = otherPath;          //  TODO really relate the two paths, including a treatment for meta-clips
                                     //   - if both are in the same sequence (same head element): just attach the tail of the other
                                    //    - if the other path points into a sequence which is attached as meta-clip to the current sequence,
                                   //          then attach the other path below that meta-clip (problem: resolve multiple attachments)
                                  //      - otherwise use the first timeline, to which the other path's sequence is attached
                                 //       - otherwise, if all else fails, use the raw otherPath
  }                             ////////////////////////////////////TICKET #672
  
  
  void
  ScopePath::appendScope (Scope const& child)  ///< @internal backdoor for #commonPrefix
  {
    path_.push_back (child);
  }
  
  
  
  
  
}}} // namespace mobject::session
