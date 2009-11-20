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

//#include "proc/mobject/mobject.hpp"
#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/scope-query.hpp"
#include "lib/singleton.hpp"
#include "lib/util.hpp"

#include <boost/scoped_ptr.hpp>
#include <tr1/memory>
//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  using util::cStr;
  
  
  class QueryFocusStack; //////TODO better include?
  

  
  class ScopeLocator
    {
      scoped_ptr<QueryFocusStack> focusStack_;
      
    public:
      static lib::Singleton<ScopeLocator> instance;
      
      QueryFocus currFocus();
      
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
///////////////////////////TODO currently just fleshing the API
  
  
  
  
  /** use the currently installed contents-resolving facility
   *  to enumerate the contents (children) of the given scope
   */
  template<typename MO>
  inline typename ScopeQuery<MO>::iterator
  ScopeLocator::explore (Scope scope)
  {
    return ScopeQuery<MO> (theResolver(), scope.getTop(), CHILDREN);
  }
  
  
  /** use the currently installed contents-resolving facility
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
