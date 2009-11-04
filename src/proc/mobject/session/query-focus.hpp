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
      ScopePath scopes_;
      /////////////////////////////////////////////////////////////////////////////////////TODO how to integrate the ref-counting handle?
      
    public:
      QueryFocus();
      
      QueryFocus& reset ();
      QueryFocus& attach (Scope const&);
      static QueryFocus push (Scope const&);
      QueryFocus pop();
      
      operator Scope()        const { return scopes_.getLeaf(); }      
      ScopePath currentPath() const { return scopes_; }
      
      template<class MO>
      void query()  const; ////////////////////////////////////////////////////////////////TODO obviously needs to return an Iterator
    };
///////////////////////////TODO currently just fleshing the API

  
  template<class MO>
  void
  QueryFocus::query()  const
  {
    UNIMPLEMENTED ("how the hell do we issue typed queries?????");  ///////////////////////TICKET #352
    
    ////////////yeah! we know now ---> ScopeLocator::explore(..)
  }
  
  
}} // namespace mobject::session
#endif
