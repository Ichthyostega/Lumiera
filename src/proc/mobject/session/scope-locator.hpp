/*
  SCOPE-LOCATOR.hpp  -  management and registration point for the QueryFocus-system
 
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
 
*/


#ifndef MOBJECT_SESSION_SCOPE_LOCATOR_H
#define MOBJECT_SESSION_SCOPE_LOCATOR_H

#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/session/scope-query.hpp"
#include "proc/mobject/placement.hpp"
#include "lib/singleton.hpp"

#include <boost/scoped_ptr.hpp>


namespace mobject {
namespace session {
  
  using boost::scoped_ptr;
  
  class QueryFocusStack;
  class ScopePath;
  
  
  
  /**
   * Singleton service establishing a link to relate
   * any compound of nested placement scopes to the current session
   * and the \em current focus for querying and exploring this structure.
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
      scoped_ptr<QueryFocusStack> focusStack_;
      
    public:
      static lib::Singleton<ScopeLocator> instance;
      
      ScopePath& currPath();
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      explore (Scope);
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      query (Scope);
      
    protected:
      ScopeLocator();
      
      friend class lib::singleton::StaticCreate<ScopeLocator>;
      
    private:
      QueryResolver const& theResolver();
    };
  
  
  
  
  /** use the contents-resolving facility exposed by the session
   *  to enumerate the contents (children) of the given scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::explore (Scope scope)
  {
    return ScopeQuery<MO> (theResolver(), scope.getTop(), CHILDREN);
  }
  
  
  /** use the contents-resolving facility exposed by the session
   *  to discover depth-first any object within this scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::query (Scope scope)
  {
    return ScopeQuery<MO> (theResolver(), scope.getTop(), CONTENTS);
  }
  
  
}} // namespace mobject::session
#endif
