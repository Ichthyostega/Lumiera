/*
  QueryFocus  -  management of current scope within the Session
 
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


#include "proc/mobject/session/query-focus.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"
//#include "proc/asset/track.hpp"

namespace mobject {
namespace session {
  
  
  
  /** TODO??? */
  QueryFocus::QueryFocus()
  { }
  
  
  /** discard any state and clear
      the current focus path */
  QueryFocus&
  QueryFocus::reset ()
  {
    scopes_.clear();
    return *this;
  }
  
  
  /** attach this QueryFocus to a container-like scope,
      causing it to \em navigate, changing the
      current ScopePath as a side-effect 
  */
  QueryFocus&
  QueryFocus::attach (Scope const& container)
  {
    UNIMPLEMENTED ("navigate this focus to attach to the given container scop");
    return *this;
  }
  
  
  /** push the "current QueryFocus" aside and open a new focus frame.
      This new QueryFocus will act as "current" until going out of scope
   */
  QueryFocus
  QueryFocus::push (Scope const& otherContainer)
  {
    UNIMPLEMENTED ("push current, open a new QueryFocus frame");
    QueryFocus newFocus; // = do push and open new frame
    newFocus.attach (otherContainer);
    return newFocus;
  }
  
  
  /** cease to use \em this specific reference to the current frame.
      This operation immediately tries to re-access what is "current"
      and returns a new handle. But when the previously released reference
      was the last one, releasing it will cause the QueryFocusStack to pop,
      in which case we'll re-attach to the now uncovered previous stack top.
  */
  QueryFocus
  QueryFocus::pop()
  {
    
  }
  
  
  
}} // namespace mobject::session
