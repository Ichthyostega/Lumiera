/*
  QueryFocus  -  management of current scope within the Session

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

* *****************************************************/


/** @file query-focus.cpp
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/query-focus.hpp"
#include "steam/mobject/mobject.hpp"
#include "lib/format-string.hpp"

using util::_Fmt;

namespace steam {
namespace mobject {
namespace session {
  
  
  
  /** 
   * create a new QueryFocus (handle)
   * linked to the current focus for discovery queries.
   * The existence of this QueryFocus instance keeps this
   * current focus alive, but multiple instances share a
   * common focus location and may change this location.
   * 
   */
  QueryFocus::QueryFocus()
    : focus_( & currPath())
    { }
  
  
  /**
   * @internal build a new QueryFocus
   * as located to an existing path.
   */
  QueryFocus::QueryFocus(ScopePath& path_to_attach)
    : focus_( &path_to_attach)
    { }
  
  
  /** @internal access the path designating
   *  the current focus location */
  ScopePath&
  QueryFocus::currPath()
  {
    return ScopeLocator::instance().currPath();
  }
  
  
  /** discard any state and navigate
   *  current focus path to model root
   */
  QueryFocus&
  QueryFocus::reset ()
  {
    REQUIRE (focus_);
    focus_->clear();
    return *this;
  }
  
  
  
  
  
  namespace {// error check shortcut....
    
    using lumiera::error::Invalid;
    
    void
    ___check_validTaget (Scope const& target)
    {
      if (!target.isValid())
        throw Invalid{"Invalid target location for QueryFocus"
                     , LERR_(INVALID_SCOPE)};             /////////////////////////////////////////TICKET #197
    }
  }//(End) shortcut
  
  
  
  /** shift this QueryFocus to a container-like scope,
   *  causing it to \em navigate, changing the
   *  current ScopePath as a side-effect
   *  @throw error::Invalid if the given container is
   *         invalid or can't be located within the model 
   */
  QueryFocus&
  QueryFocus::shift (Scope const& container)
  {
    ___check_validTaget (container);
    
    REQUIRE (focus_);
    focus_->navigate (container);
    return *this;
  }
  
  
  /** push the "current QueryFocus" aside and open a new focus frame.
   *  This new QueryFocus will act as "current" until going out of scope
   *  @throw error::Invalid in case of invalid or unlocatable target scope
   */
  QueryFocus
  QueryFocus::push (Scope const& otherContainer)
  {
    ___check_validTaget (otherContainer);
    
    QueryFocus newFocus (ScopeLocator::instance().pushPath());
    newFocus.shift (otherContainer);
    return newFocus;
  }
  
  
  /** push the "current QueryFocus" aside and open a new focus frame,
   *  which starts out at the same location as the original */
  QueryFocus
  QueryFocus::push ()
  {
    Scope currentLocation (ScopeLocator::instance().currPath().getLeaf());
    ENSURE (currentLocation.isValid());
    
    QueryFocus newFocus (ScopeLocator::instance().pushPath());
    newFocus.shift (currentLocation);
    return newFocus;
  }
  
  
  
  /** cease to use \em this specific reference to the current frame.
   *  This operation immediately tries to re-attach to what is "current"
   *  and readjusts the internal handle. But when the previously released
   *  reference was the last one, releasing it will cause the QueryFocusStack
   *  to pop, in which case we'll re-attach to the now uncovered previous stack top.
   */
  QueryFocus&
  QueryFocus::pop()
  {
    focus_ = 0;
    focus_ = & currPath();
    
    return *this;
  }
  
  
  
  /** diagnostic self-display based on the ScopePath */
  QueryFocus::operator string()  const 
  {
    static _Fmt display("Focus(%d)--->%s");
    return display % ScopeLocator::instance().stackSize()
                   % *focus_;
  }
  
  
  
}}} // namespace steam::mobject::session
