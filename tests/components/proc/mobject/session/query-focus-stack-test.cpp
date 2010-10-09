/*
  QueryFocusStack(Test)  -  verify the stack of focus path frames
 
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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/query-focus-stack.hpp"
#include "proc/mobject/session/test-scope-invalid.hpp"
#include "lib/util.hpp"




namespace mobject {
namespace session {
namespace test    {
  
  using util::isnil;
  using util::isSameObject;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  /***************************************************************************
   * @test behaviour of the stack of focus location paths.
   *       Basically this is just a stack, but has a somewhat unusual behaviour
   *       on pop(), as it considers the (intrusive) ref-count maintained within
   *       the stack frames (ScopePath instances) and cleans up unused frames.
   *       Similar to the ScopePath_test, we use a pseudo-session to create
   *       some path frames to play with.
   * @note this test executes a lot of functionality in a manual by-hand way,
   *       which in the actual application is accessed and utilised through
   *       QueryFocus objects as frontend. 
   *       
   * @see  mobject::session::QueryFocusStack
   * @see  mobject::session::ScopePath
   */
  class QueryFocusStack_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index and
          // set up dummy session contents
          PPIdx index = build_testScopes();
          
          createStack();
          usePushedFrame();
          automaticFrameHandling();
          verify_errorHandling();
          clear();
        }
      
      
      void
      createStack ()
        {
          QueryFocusStack stack;
          
          ASSERT (!isnil (stack));
          ASSERT (!isnil (stack.top()));
          ASSERT (stack.top().isRoot());
        }
      
      
      void
      usePushedFrame ()
        {
          QueryFocusStack stack;
          PMO& startPoint = retrieve_startElm();
          
          ScopePath& firstFrame = stack.top();   // remember for later
          intrusive_ptr_add_ref (&firstFrame);
          stack.top().navigate(startPoint);
          stack.top().moveUp();
          ASSERT (Scope(startPoint).getParent() == stack.top().getLeaf());
          ASSERT (1 == stack.size());
          
          // now open a second path frame, pushing aside the initial one
          ScopePath& secondFrame = stack.push(startPoint);
          intrusive_ptr_add_ref (&secondFrame);
          ASSERT (2 == stack.size());
          ASSERT (secondFrame == stack.top());
          ASSERT (secondFrame.getLeaf() == startPoint);
          ASSERT (secondFrame.getLeaf() != firstFrame.getLeaf());
          
          // can still reach and manipulate the ref-count of the first frame
          intrusive_ptr_add_ref (&firstFrame);
          ASSERT (2 == firstFrame.ref_count());
          ASSERT (1 == secondFrame.ref_count());
          
          // can use/navigate the stack top frame
          stack.top().goRoot();
          ASSERT (!stack.top()); // now indeed at root == no path
          ASSERT (secondFrame.getLeaf().isRoot());
          ASSERT (secondFrame == stack.top());
          
          // now drop back to the first frame:
          ASSERT (1 == secondFrame.ref_count());
          intrusive_ptr_release (&secondFrame);
          ASSERT (0 == secondFrame.ref_count());
          stack.pop_unused();
          ASSERT (1 == stack.size());
          ASSERT (firstFrame == stack.top());
          
          // ...still pointing at the previous location
          ASSERT (Scope(startPoint).getParent() == stack.top().getLeaf());
          ASSERT (2 == firstFrame.ref_count());
        }
      
      
      void
      automaticFrameHandling ()
        {
          QueryFocusStack stack;
          PMO& startPoint = retrieve_startElm();
          
          ScopePath& firstFrame = stack.top();   // remember for later
          stack.top().navigate(startPoint);
          ASSERT (1 == stack.size());
          intrusive_ptr_add_ref (&firstFrame);
          
          // now open two new frames, but don't add ref-counts on them
          ScopePath& secondFrame = stack.push(startPoint);
          ScopePath& thirdFrame  = stack.push(startPoint);
          ASSERT (3 == stack.size());
          ASSERT (1 == firstFrame.ref_count());
          ASSERT (0 == secondFrame.ref_count());
          ASSERT (0 == thirdFrame.ref_count());
          
          // any ref to top detects the non-referred-to state (by ref count==0)
          // and will automatically pop and clean up...
          ScopePath& newTop = stack.top();
          ASSERT (1 == stack.size());
          ASSERT (firstFrame == stack.top());
          ASSERT (isSameObject(newTop, firstFrame));
          ASSERT (stack.top().getLeaf() == startPoint);
          
          // second exercise: a pop_unused may even completely empty the stack
          ScopePath& anotherFrame = stack.push(startPoint);
          ASSERT (0 == anotherFrame.ref_count());
          ASSERT (1 == firstFrame.ref_count());
          intrusive_ptr_release (&firstFrame);
          ASSERT (0 == firstFrame.ref_count());
          ASSERT (firstFrame.getLeaf() == startPoint);
          
          stack.pop_unused();
          ASSERT (1 == stack.size());
                          // Note: don't use previously taken pointers
                          //       or references anymore, after the stack
                          //       triggered a cleanup!
          ScopePath& anotherFrame2 = stack.top();
          ASSERT (0 == anotherFrame2.ref_count());
          ASSERT (anotherFrame2.getLeaf().isRoot());
          anotherFrame2.navigate(startPoint);
          ASSERT (anotherFrame2.getLeaf() == startPoint);
          
          stack.top();
          ASSERT (1 == stack.size());
          ASSERT (stack.top().getLeaf().isRoot());
        }
      
      
      void
      verify_errorHandling ()
        {
          QueryFocusStack stack;
          PMO& startPoint = retrieve_startElm();
          
          ScopePath& firstFrame = stack.top();   // remember for later
          stack.top().navigate(startPoint);
          ASSERT (1 == stack.size());
          intrusive_ptr_add_ref (&firstFrame);
          
          ScopePath beforeInvalidNavigation = firstFrame;
          Scope const& unrelatedScope = fabricate_invalidScope();
          
          // try to navigate to an invalid place
          VERIFY_ERROR (INVALID_SCOPE, stack.top().navigate (unrelatedScope) );
          ASSERT (1 == stack.size());
          ASSERT (1 == firstFrame.ref_count());
          ASSERT (stack.top().getLeaf() == startPoint);
          
          // try to push an invalid place
          VERIFY_ERROR (INVALID_SCOPE, stack.push (unrelatedScope) );
          ASSERT (1 == stack.size());
          ASSERT (1 == firstFrame.ref_count());
          ASSERT (stack.top().getLeaf() == startPoint);
        }
      
      
      void
      clear ()
        {
          QueryFocusStack stack;
          intrusive_ptr_add_ref (&stack.top());
          stack.top().moveUp();
          ASSERT (stack.top().empty());
          
          PMO& startPoint = retrieve_startElm();
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          intrusive_ptr_add_ref ( & stack.push(startPoint) );
          ASSERT (10 == stack.size());
          stack.pop_unused();
          ASSERT (10 == stack.size());
          ASSERT (1 == stack.top().ref_count());
          
          stack.clear();
          ASSERT (1 == stack.size());
          ASSERT (!stack.top().empty());
          ASSERT (stack.top().getLeaf().isRoot());
          ASSERT (0 == stack.top().ref_count());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryFocusStack_test, "unit session");
  
  
}}} // namespace mobject::session::test
