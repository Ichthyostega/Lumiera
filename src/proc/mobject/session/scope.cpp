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
#include "lib/iter-source.hpp"                 ////////////////////TICKET #493 : using the IterSource adapters here
//#include "proc/mobject/session/track.hpp"

//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"
#include <vector>

using std::vector;
using lib::IterSource;
using lib::WrappedLumieraIterator;
using lib::iter_source::wrapIter;

namespace lib {
namespace iter{

//  using mobject::session::Scope;
//
//  template<>
//  struct TypeBinding<vector<Scope>::const_reverse_iterator>
//    {
//      typedef const Scope   value_type;
//      typedef Scope const&  reference;
//      typedef const Scope*  pointer;
//    };
}}

namespace mobject {
namespace session {
  
  
  LUMIERA_ERROR_DEFINE (INVALID_SCOPE, "Placement scope invalid and not locatable within model");

  
  
  /** conversion of a scope top (placement) into a Scope.
   *  only allowed if the given Placement is actually attached
   *  to the session, which will be checked by index access */
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
    TODO ("anything to initialise here?");
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
  ScopeLocator::locate (Scope scope)
  {
    ScopePath& currentPath = focusStack_->top();
    currentPath.navigate (scope);
    return wrapIter (currentPath.begin());
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
  
  
  
  
  
}} // namespace mobject::session
