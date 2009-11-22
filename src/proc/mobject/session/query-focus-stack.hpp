/*
  QUERY-FOCUS-STACK.hpp  -  management of current scope within the Session
 
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


#ifndef MOBJECT_SESSION_QUERY_FOCUS_STACK_H
#define MOBJECT_SESSION_QUERY_FOCUS_STACK_H

//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/scope-path.hpp"

//#include <vector>
//#include <string>
#include <boost/noncopyable.hpp>
#include <list>

//using std::vector;
//using std::string;
using std::list;


namespace mobject {
namespace session {
  
  
  
  /**
   * TODO type comment
   */
  class QueryFocusStack
    : boost::noncopyable
    {
      
      std::list<ScopePath> paths_;
      
    public:
      QueryFocusStack()
        : paths_()
        {
          openDefaultFrame();
        }
      
      
      bool empty()  const;
      size_t size() const;
      
      ScopePath& push (Scope const&);
      ScopePath& top  ();
      void pop_unused ();
      
      
    private:
      void openDefaultFrame();
    };
///////////////////////////TODO currently just fleshing the API
  
  
  bool
  QueryFocusStack::empty()  const
  {
    return paths_.empty();
  }
  
  
  size_t
  QueryFocusStack::size()  const
  {
    return paths_.size();
  }
  
  
  /** Open a new path frame, pushing down the current frame.
   *  The new frame tries to locate the given start scope
   *  and navigates to this position.
   *  @note EXCEPTION_STRONG guarantee
   *  @return reference to the newly created path on top
   *  @throw error::Invalid if newStartPoint isn't locatable
   */
  ScopePath&
  QueryFocusStack::push (Scope const& newStartPoint)
  {
    ScopePath newPathFrame (newStartPoint); // may throw
    ENSURE (newPathFrame.isValid() || newStartPoint.isRoot());
    
    paths_.push_back (newPathFrame);
    ENSURE (0 < size());
    return paths_.back();
  }
  
  
  /** @return the topmost path frame actually in use
   *  @note may invoke #pop_unused()
   *  @note EXCEPTON_FREE    ///////TODO prove!
   */ 
  ScopePath&
  QueryFocusStack::top ()
  {
    if ( 0 == size()
      || 0 == paths_.back().ref_count()
       )
      pop_unused();
    
    ENSURE (!empty());
    ENSURE (!paths_.back().empty());
    return paths_.back();
  }
  
  
  /** investigate the stack top and discard any path frames
   *  which aren't referred anymore (as indicated by their
   *  ScopePath#use_count(). After executing this function
   *  the topmost frame is either in use, or a new default
   *  frame has been created at the bottom of an empty stack.
   *  @note EXCEPTON_FREE    ///////TODO prove!
   */
  void
  QueryFocusStack::pop_unused ()
  {
    while (size() && (0 == paths_.back().ref_count()))
      paths_.pop_back();
    
    if (0 == size())
      openDefaultFrame();
    ENSURE (!empty());
    ENSURE (!paths_.back().empty());
  }
  
  
  /** @internal open a default path frame at the bottom
   *  of an empty stack, locating to current model root
   *  @note EXCEPTON_FREE    ///////TODO prove!  
   */
  void
  QueryFocusStack::openDefaultFrame()
  {
    REQUIRE (0 == size());
    paths_.resize(1);
  }
  
  
  
  
}} // namespace mobject::session
#endif
