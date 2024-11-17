/*
  Scope  -  nested search scope for properties of placement

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file scope.cpp
 ** Implementation of placement scopes and scope locator.
 ** This translation unit embeds the (hidden) link to the session implementation
 ** used to establish the position of a given placement within the hierarchy
 ** of nested scopes. The rest of the model implementation code mostly builds
 ** on top of this access point, when it comes to discovering contents or
 ** navigating within the model. Especially the ScopeLocator singleton
 ** defined here plays the role of linking together the system of nested scopes,
 ** the current QueryFocus and the actual session implementation and storage
 ** (PlacementIndex)
 ** 
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "steam/mobject/session/scope.hpp"
#include "steam/mobject/session/scope-locator.hpp"
#include "steam/mobject/session/query-focus-stack.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/mobject.hpp"
#include "lib/iter-source.hpp"                 ////////////////////TICKET #493 : only using the IterSource wrapIter adapter here
#include "common/query/query-resolver.hpp"

#include <vector>

using std::string;
using std::vector;
using lib::IterSource;
using lib::iter_source::wrapIter;


namespace lumiera {
namespace error {
  LUMIERA_ERROR_DEFINE (INVALID_SCOPE, "Placement scope invalid and not locatable within model");
  LUMIERA_ERROR_DEFINE (NO_PARENT_SCOPE, "Parent scope of root not accessible");
  LUMIERA_ERROR_DEFINE (EMPTY_SCOPE_PATH, "Placement scope not locatable (empty model path)");
}}

namespace steam {
namespace mobject {
namespace session {
  
  
  
  /** conversion of a scope top (placement) into a Scope.
   *  only allowed if the given Placement is actually attached
   *  to the session, which will be checked by index access */
  Scope::Scope (PlacementMO const& constitutingPlacement)
    : anchor_(constitutingPlacement)
  { }
  
  Scope::Scope (PlacementMO::ID const& constitutingPlacement)
    : anchor_(constitutingPlacement)
  { }
  
  
  Scope::Scope ()
    : anchor_()
  {
    REQUIRE (!anchor_.isValid());
  }
  
  
  Scope::Scope (Scope const& o)
    : anchor_(o.anchor_)
  {
    ENSURE (anchor_.isValid());
  }
  
  
  Scope&
  Scope::operator= (Scope const& o)
  {
    anchor_ = o.anchor_;  // note: actually we're just assigning an hash value
    ENSURE (o.isValid());
    return *this;
  }
  
  
  /** constant \em invalid scope token. */
  const Scope Scope::INVALID = Scope();
  
  
  
  ScopeLocator::ScopeLocator()
    : focusStack_(new QueryFocusStack)
  { }
  
  ScopeLocator::~ScopeLocator() { }
  
  
  /** Storage holding the single ScopeLocator instance */
  lib::Depend<ScopeLocator> ScopeLocator::instance;
  
  
  /** @internal the one (and only) access point
   *  actually to link the system of Scope and QueryFocus
   *  to the current session, by delegating resolution
   *  of contents discovery queries to the PlacementIndex
   *  managed within the session 
   */
  lumiera::QueryResolver const&
  ScopeLocator::theResolver()
  {
    return SessionServiceExploreScope::getResolver();
  }
  
  
  size_t
  ScopeLocator::stackSize()  const
  {
    return focusStack_->size();
  }
  
  
  /** establishes the \em current query focus location.
   *  Relies on the state of the QueryFocusStack.
   *  If there is no current focus location, a new
   *  one is created, referring to the root Scope.
   *  @return the current path corresponding to the
   *          most recently used QueryFocus, which is
   *          actually still referred from somewhere.
   *  @note may cause the QueryFocusStack to pop 
   *          path entries no longer in use.
   */
  ScopePath&
  ScopeLocator::currPath()
  {
    return focusStack_->top();
  }
  
  
  /** push aside the current focus location
   *  and open a new ScopePath frame, to serve
   *  as \em current location until released 
   */
  ScopePath&
  ScopeLocator::pushPath()
  {
    return focusStack_->push (SessionServiceExploreScope::getScopeRoot());
  }
  
  
  /** navigate the \em current QueryFocus scope location. The resulting
   *  access path to the new location is chosen such as to be most closely related
   *  to the original location; this includes picking a timeline or meta-clip
   *  attachment most similar to the one used in the original path. So effectively
   *  you'll see things through the same "scoping perspective" as given by the
   *  original path, if possible to the new location
   *  given as parameter. use the contents-resolving facility exposed by the session
   * @note changes the \em current QueryFocus as a sideeffect
   * @param scope the new target location to navigate
   * @return an iterator yielding the nested scopes from the new location
   *         up to root, in a way likely to be similar to the original location
   */
  IterSource<const Scope>::iterator
  ScopeLocator::locate (Scope const& scope)
  {
    ScopePath& currentPath = focusStack_->top();
    currentPath.navigate (scope);
    return wrapIter (currentPath.begin());
  }
  
  
  
  /** discover the enclosing scope of a given Placement */
  Scope
  Scope::containing (PlacementMO const& aPlacement)
  {
    return SessionServiceExploreScope::getScope (aPlacement);
  }
  
  
  Scope
  Scope::containing (RefPlacement const& refPlacement)
  {
    return containing (*refPlacement);
  }
  
  
  PlacementMO&
  Scope::getTop()  const
  {
    ASSERT (anchor_);
    return *anchor_;
  }
  
  
  /** retrieve the parent scope which encloses this scope.
   *  @throw error::Invalid if this is the root scope
   */
  Scope
  Scope::getParent()  const
  {
    if (isRoot())
        throw lumiera::error::Invalid ("can't get parent of root scope"
                                      , LERR_(NO_PARENT_SCOPE));
    
    return SessionServiceExploreScope::getScope (*anchor_);
  }
  
  
  /** @return true if this is the outmost (root) scope */
  bool
  Scope::isRoot()  const
  {
    return *anchor_ == SessionServiceExploreScope::getScopeRoot();
  }
  
  
  /** check if this scope can be located.
   *  An default constructed Scope (i.e without
   *  defining Placement) can't be located and returns false here */
  bool
  Scope::isValid()  const
  {
    return anchor_.isValid();
  }
  
  
  /** Scope diagnostic self display.
   *  Implemented based on the self-display of the MObject
   *  attached through the scope top placement. Usually this
   *  should yield a reasonably unique, descriptive string. */
  Scope::operator string()  const
  {
    string res("[");
    res += anchor_->shortID();
    res += "]";
    return res;
  }
  
  
  
  
  
}}} // namespace steam::mobject::session
