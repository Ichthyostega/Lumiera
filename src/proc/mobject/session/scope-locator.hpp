/*
  SCOPE-LOCATOR.hpp  -  management and registration point for the QueryFocus-system

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


/** @file scope-locator.hpp
 ** Service to build the notion of a _current location_ within the Sesison model
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef PROC_MOBJECT_SESSION_SCOPE_LOCATOR_H
#define PROC_MOBJECT_SESSION_SCOPE_LOCATOR_H

#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/session/scope-query.hpp"
#include "proc/mobject/placement.hpp"
#include "lib/iter-source.hpp"                 ////////////////////TICKET #493 : the bare interface would be sufficient here
#include "lib/depend.hpp"

#include <memory>


namespace proc {
namespace mobject {
namespace session {
  
  class QueryFocusStack;
  class ScopePath;
  
  
  
  /**
   * Singleton service establishing a link to relate
   * any compound of nested placement scopes to the current session
   * and the _current focus_ for querying and exploring this structure.
   * While it is OK to use this service directly, clients usually would
   * prefer to use QueryFocus as a frontend.
   * 
   * ScopeLocator is the access point both to the current query scope location
   * (as maintained with the help of the QueryFocusStack) and allows to explore
   * the current session data structures (building on a QueryResolver service
   * exposed by the session).
   * 
   * @note in its current form (11/09), ScopeLocator is deliberately <b>not threadsafe</b>
   */
  class ScopeLocator
    {
      std::unique_ptr<QueryFocusStack> focusStack_;
      
    public:
      static lib::Depend<ScopeLocator> instance;
      
      ScopePath& currPath();
      ScopePath& pushPath();
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      explore (Scope const&);
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      query (Scope const&);
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      getRawPath (Scope const&);
      
      ScopeQuery<MObject>::iterator
      getRawPath (Scope const&);
      
      lib::IterSource<const Scope>::iterator
      locate (Scope const& target);
      
      size_t stackSize()  const;
      
      
     ~ScopeLocator();
      
    protected:
      ScopeLocator();
      
      friend class lib::DependencyFactory<ScopeLocator>;
      
    private:
      lumiera::QueryResolver const& theResolver();
    };
  
  
  
  
  /** use the contents-resolving facility exposed by the session
   *  to enumerate the contents (children) of the given scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::explore (Scope const& scope)
  {
    return ScopeQuery<MO> (scope.getTop(), CHILDREN).resolveBy (theResolver());
  }
  
  
  /** use the contents-resolving facility exposed by the session
   *  to discover depth-first any object within this scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::query (Scope const& scope)
  {
    return ScopeQuery<MO> (scope.getTop(), CONTENTS).resolveBy (theResolver());
  }
  
  
  /** use the contents-resolving facility exposed by the session
   *  to discover the path up from the given scope to model root.
   *  @note this yields the \em raw path (basic containment hierarchy),
   *        as opposed to an effective or virtual path, which should reflect
   *        the attachment of Sequences to Timelines or meta-clips. That is,
   *        you'll always get the top-level fork root of any sequence as direct
   *        child of the root node and timelines (BindingMO) just appear
   *        to be "dead ends"
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::getRawPath (Scope const& scope)
  {
    return ScopeQuery<MO> (scope.getTop(), PATH).resolveBy (theResolver());
  }

  inline ScopeQuery<MObject>::iterator
  ScopeLocator::getRawPath (Scope const& scope)
  {
    return ScopeQuery<MObject> (scope.getTop(), PATH).resolveBy (theResolver());
  }
  
  
}}} // namespace proc::mobject::session
#endif
