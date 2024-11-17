/*
  QUERY-FOCUS-STACK.hpp  -  management of current scope within the Session

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file query-focus-stack.hpp
 ** Implementation facility to work with and navigate nested scopes
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef MOBJECT_SESSION_QUERY_FOCUS_STACK_H
#define MOBJECT_SESSION_QUERY_FOCUS_STACK_H

#include "steam/mobject/session/scope-path.hpp"
#include "lib/nocopy.hpp"

#include <list>

using std::list;


namespace steam {
namespace mobject {
namespace session {
  
  
  
  /**
   * A custom stack holding ScopePath »frames«.
   * It is utilised by the ScopeLocator to establish the
   * \em current query focus location. Client code should
   * access this mechanism through QueryFocus instances 
   * used as frontend. These QueryFocus objects incorporate
   * a boost::intrusive_ptr, which stores the ref-count within
   * the mentioned ScopePath frames located in the stack.
   * 
   * \par automatic cleanup of unused frames
   * The stack is aware of this ref-counting mechanism and will --
   * on each access -- automatically clean up any unused frames starting
   * from stack top, until encountering the first frame still in use.
   * This frame, by definition, is the <b>current focus location</b>.
   * The stack ensures there is always at least one ScopePath frame,
   * default-creating a new one if necessary.
   * 
   * @see query-focus-stack-test.cpp
   * @see ScopeLocator
   * @see QueryFocus access point for client code
   */
  class QueryFocusStack
    : util::NonCopyable
    {
      
      std::list<ScopePath> paths_;
      
    public:
      QueryFocusStack ()
        : paths_()
        {
          openDefaultFrame();
        }
      
      
      bool empty ()  const;
      size_t size () const;
      
      ScopePath& push (Scope const&);
      ScopePath& top  ();
      void pop_unused ();
      void clear ();
      
      
    private:
      void openDefaultFrame ();
    };
  
  
  
  
  
  
  
  /* __implementation__ */ 
  
  bool
  QueryFocusStack::empty ()  const
  {
    return paths_.empty();
  }
  
  
  size_t
  QueryFocusStack::size ()  const
  {
    return paths_.size();
  }
  
  
  void
  QueryFocusStack::clear ()
  {
    paths_.clear();
    openDefaultFrame();
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
    return paths_.back();
  }
  
  
  /** investigate the stack top and discard any path frames
   *  which aren't referred anymore (as indicated by their
   *  ScopePath#use_count(). After executing this function
   *  the topmost frame is either in use, or a new default
   *  frame has been created at the bottom of an empty stack.
   *  @note EXCEPTION_FREE    ///////TODO prove!
   */
  void
  QueryFocusStack::pop_unused ()
  {
    if (1 == size() && !paths_.front().isValid())
      return; // unnecessary to evict a base frame repeatedly
    
    while (size() && (0 == paths_.back().ref_count()))
      paths_.pop_back();
    
    if (0 == size())
      openDefaultFrame();
    ENSURE (!empty());
  }
  
  
  /** @internal open a default path frame at the bottom
   *  of an empty stack, locating to current model root
   *  @note EXCEPTION_FREE    ///////TODO prove!
   */
  void
  QueryFocusStack::openDefaultFrame ()
  {
    REQUIRE (0 == size());
    
    paths_.resize(1);
    
    ENSURE (!paths_.front().empty());
    ENSURE (!paths_.front().isValid()); // i.e. just root scope
  }
  
  
  
  
}}} // namespace mobject::session
#endif /*MOBJECT_SESSION_QUERY_FOCUS_STACK_H*/
