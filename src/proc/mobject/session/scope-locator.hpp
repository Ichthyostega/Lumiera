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
      ScopePath& pushPath();
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      explore (Scope);
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      query (Scope);
      
      template<typename MO>
      typename ScopeQuery<MO>::iterator
      locate (Scope scope);
      
      ScopeQuery<MObject>::iterator
      getRawPath (Scope);


     ~ScopeLocator();
     
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
    return ScopeQuery<MO> (scope.getTop(), CHILDREN).resolveBy (theResolver());
  }
  
  
  /** use the contents-resolving facility exposed by the session
   *  to discover depth-first any object within this scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::query (Scope scope)
  {
    return ScopeQuery<MO> (scope.getTop(), CONTENTS).resolveBy (theResolver());
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
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::locate (Scope scope)
  {
    UNIMPLEMENTED ("virtual navigation");
    ///////////////////////////////////////////TODO: see scope-query.hpp
    ///////////////////////////////////////////TODO: its hard to come up with a generic implementation which yields a compatible iterator
    ///////////////////////////////////////////TODO: *alternatively* just expose an Iterator of Scopes?
  }
  
  
  /** use the contents-resolving facility exposed by the session
   *  to discover the path up from the given scope to model root.
   *  @note this yields the \em raw path (basic containment hierarchy),
   *        as opposed to an effective or virtual path, which should reflect
   *        the attachment of Sequences to Timelines or meta-clips. That is,
   *        you'll always get the top-level track of any sequence as direct
   *        child of the root node and timelines (BindingMO) just appear
   *        to be "dead ends"
   */
  inline ScopeQuery<MObject>::iterator
  ScopeLocator::getRawPath (Scope scope)
  {
    return ScopeQuery<MObject> (scope.getTop(), PATH).resolveBy (theResolver());
  }


}} // namespace mobject::session
#endif
