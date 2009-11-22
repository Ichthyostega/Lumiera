/*
  Scope  -  nested search scope for properties of placement
 
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


#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/session/scope-locator.hpp"
#include "proc/mobject/session/query-focus-stack.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"
//#include "proc/asset/track.hpp"

namespace mobject {
namespace session {
  
  
  
  /** TODO??? */
  Scope::Scope (PlacementMO const& constitutingPlacement)
    : anchor_(constitutingPlacement)
  {
    
  }
  
  
  Scope::Scope ()
    : anchor_()
  {
    REQUIRE (!anchor_.isValid());
  }
  
  
  Scope::Scope (Scope const& o)
    : anchor_(o.anchor_)
  { }
  
  
  Scope&
  Scope::operator= (Scope const& o)
  {
    anchor_ = o.anchor_;  ////////////////////////////TODO verify correctness
    return *this;
  }
  
  
  
  ScopeLocator::ScopeLocator()
    : focusStack_(new QueryFocusStack)
  {
    TODO ("anything in initialise here?");
  }
  
  ScopeLocator::~ScopeLocator() { }
  
  
  /** Storage holding the single ScopeLocator instance */
  lib::Singleton<ScopeLocator> ScopeLocator::instance;
  
  
  /** @internal the one (and only) access point
   *  actually to link the system of Scope and QueryFocus
   *  to the current session, by delegating resolution
   *  of contents discovery queries to the PlacementIndex
   *  managed within the session 
   */
  QueryResolver const&
  ScopeLocator::theResolver()
  {
    return SessionServiceExploreScope::getResolver();
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
    UNIMPLEMENTED ("how to access and handle the current focus");
  }
  
  
  /** discover the enclosing scope of a given Placement */
  Scope const&
  Scope::containing (PlacementMO const& aPlacement)
  {
    UNIMPLEMENTED ("scope discovery");
  }
  
  
  Scope const&
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
  Scope const&
  Scope::getParent()  const
  {
    UNIMPLEMENTED ("retrieve the enclosing parent scope");
  }
  
  
  /** @return true if this is the outmost (root) scope */
  bool
  Scope::isRoot()  const
  {
    UNIMPLEMENTED ("detection of root scope");
  }
  
  
  /** check if this scope can be located.
   *  An default constructed Scope (i.e without
   *  defining Placement) can't be located and returns false here */
  bool
  Scope::isValid()  const
  {
    return anchor_.isValid();
  }
  
  
  /** enumerate the path of nested scopes up to root scope.
   *  @return an iterator which starts with this scope and
   *          successively yields outer scopes, stopping at root.
   */
  Scope::IterType_
  Scope::ascend()  const
  {
    UNIMPLEMENTED ("ascend scope hierarchy up to root");
  }
  
  
  
  
  
}} // namespace mobject::session
