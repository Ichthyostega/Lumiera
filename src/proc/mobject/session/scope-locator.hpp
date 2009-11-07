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
      shared_ptr<QueryResolver> index_;
      
    public:
      static lib::Singleton<ScopeLocator> instance;
      
      void
      activate (shared_ptr<QueryResolver> resolvingFacility);
      
      template<typename MO>
      typename ContentsQuery<MO>::iterator
      explore (Scope);
      
    protected:
      ScopeLocator();
      
      friend class lib::singleton::StaticCreate<ScopeLocator>;
    };
///////////////////////////TODO currently just fleshing the API
  
  
  
  /** activate or de-activate the QueryFocus system.
   *  This is done by a link to a contents-query resolving facility,
   *  typically the PlacementIndex within the current session.  
   */
  inline void
  ScopeLocator::activate (shared_ptr<QueryResolver> resolvingFacility)
  {
    index_ = resolvingFacility;
    
    if (index_)
      INFO (config, "Enabling Scope resolution by %s.", cStr(*index_));
    else
      INFO (config, "Disabling Scope resolution.");
  }
  
  
  /** use the currently installed contents-resolving facility
   *  to enumerate the contents of the given scope
   */
  template<typename MO>
  inline typename ContentsQuery<MO>::iterator
  ScopeLocator::explore (Scope scope)
  {
    REQUIRE (index_);
    return ContentsQuery<MO> (*index_, scope.getTop());
  }
  
  
}} // namespace mobject::session
#endif
