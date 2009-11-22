/*
  QUERY-FOCUS.hpp  -  management of current scope within the Session
 
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


#ifndef MOBJECT_SESSION_QUERY_FOCUS_H
#define MOBJECT_SESSION_QUERY_FOCUS_H

//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/scope-path.hpp"
#include "proc/mobject/session/scope-locator.hpp"

#include <boost/intrusive_ptr.hpp>
//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  
  
  /**
   * TODO type comment
   */
  class QueryFocus
    {
      boost::intrusive_ptr<ScopePath> focus_;
      
    public:
      QueryFocus();
      
      QueryFocus& reset ();
      QueryFocus& attach (Scope const&);
      static QueryFocus push (Scope const&);
      QueryFocus& pop();
      
      operator Scope()        const { return currPath().getLeaf(); }      
      ScopePath currentPath() const { return currPath(); }       ///< @note returns a copy
      
      template<class MO>
      typename ScopeQuery<MO>::iterator
      query()  const;
      
      template<class MO>
      typename ScopeQuery<MO>::iterator
      explore()  const;
      
    private:
      QueryFocus (ScopePath&);
      
      static ScopePath& currPath();
    };
///////////////////////////TODO currently just fleshing the API

  

  
  /** discover depth-first any matching object
   *  within \em current focus. Resolution is 
   *  delegate to the \em current session */
  template<class MO>
  typename ScopeQuery<MO>::iterator
  QueryFocus::query()  const
  {
    ScopeLocator::instance().query<MO> (*this);
  }
      
  
  /** discover any matching object contained 
   *  as immediate Child within \em current focus. 
   *  Resolution through \em current session */
  template<class MO>
  typename ScopeQuery<MO>::iterator
  QueryFocus::explore()  const
  {
    ScopeLocator::instance().explore<MO> (*this);
  }
      

  
}} // namespace mobject::session
#endif
