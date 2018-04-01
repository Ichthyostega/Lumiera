/*
  QueryFocusStack(Test)  -  verify the stack of focus path frames

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file query-focus-stack-test.cpp
 ** unit test \ref QueryFocusStack_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/query-focus-stack.hpp"
#include "proc/mobject/session/test-scope-invalid.hpp"
#include "lib/util.hpp"




namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using util::isnil;
  using util::isSameObject;
  using lumiera::error::LERR_(INVALID);
  
  
  /***********************************************************************//**
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
          
          CHECK (!isnil (stack));
          CHECK (!isnil (stack.top()));
          CHECK (stack.top().isRoot());
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
          CHECK (Scope(startPoint).getParent() == stack.top().getLeaf());
          CHECK (1 == stack.size());
          
          // now open a second path frame, pushing aside the initial one
          ScopePath& secondFrame = stack.push(startPoint);
          intrusive_ptr_add_ref (&secondFrame);
          CHECK (2 == stack.size());
          CHECK (secondFrame == stack.top());
          CHECK (secondFrame.getLeaf() == startPoint);
          CHECK (secondFrame.getLeaf() != firstFrame.getLeaf());
          
          // can still reach and manipulate the ref-count of the first frame
          intrusive_ptr_add_ref (&firstFrame);
          CHECK (2 == firstFrame.ref_count());
          CHECK (1 == secondFrame.ref_count());
          
          // can use/navigate the stack top frame
          stack.top().goRoot();
          CHECK (!stack.top()); // now indeed at root == no path
          CHECK (secondFrame.getLeaf().isRoot());
          CHECK (secondFrame == stack.top());
          
          // now drop back to the first frame:
          CHECK (1 == secondFrame.ref_count());
          intrusive_ptr_release (&secondFrame);
          CHECK (0 == secondFrame.ref_count());
          stack.pop_unused();
          CHECK (1 == stack.size());
          CHECK (firstFrame == stack.top());
          
          // ...still pointing at the previous location
          CHECK (Scope(startPoint).getParent() == stack.top().getLeaf());
          CHECK (2 == firstFrame.ref_count());
        }
      
      
      void
      automaticFrameHandling ()
        {
          QueryFocusStack stack;
          PMO& startPoint = retrieve_startElm();
          
          ScopePath& firstFrame = stack.top();   // remember for later
          stack.top().navigate(startPoint);
          CHECK (1 == stack.size());
          intrusive_ptr_add_ref (&firstFrame);
          
          // now open two new frames, but don't add ref-counts on them
          ScopePath& secondFrame = stack.push(startPoint);
          ScopePath& thirdFrame  = stack.push(startPoint);
          CHECK (3 == stack.size());
          CHECK (1 == firstFrame.ref_count());
          CHECK (0 == secondFrame.ref_count());
          CHECK (0 == thirdFrame.ref_count());
          
          // any ref to top detects the non-referred-to state (by ref count==0)
          // and will automatically pop and clean up...
          ScopePath& newTop = stack.top();
          CHECK (1 == stack.size());
          CHECK (firstFrame == stack.top());
          CHECK (isSameObject(newTop, firstFrame));
          CHECK (stack.top().getLeaf() == startPoint);
          
          // second exercise: a pop_unused may even completely empty the stack
          ScopePath& anotherFrame = stack.push(startPoint);
          CHECK (0 == anotherFrame.ref_count());
          CHECK (1 == firstFrame.ref_count());
          intrusive_ptr_release (&firstFrame);
          CHECK (0 == firstFrame.ref_count());
          CHECK (firstFrame.getLeaf() == startPoint);
          
          stack.pop_unused();
          CHECK (1 == stack.size());
                          // Note: don't use previously taken pointers
                          //       or references anymore, after the stack
                          //       triggered a cleanup!
          ScopePath& anotherFrame2 = stack.top();
          CHECK (0 == anotherFrame2.ref_count());
          CHECK (anotherFrame2.getLeaf().isRoot());
          anotherFrame2.navigate(startPoint);
          CHECK (anotherFrame2.getLeaf() == startPoint);
          
          stack.top();
          CHECK (1 == stack.size());
          CHECK (stack.top().getLeaf().isRoot());
        }
      
      
      void
      verify_errorHandling ()
        {
          QueryFocusStack stack;
          PMO& startPoint = retrieve_startElm();
          
          ScopePath& firstFrame = stack.top();   // remember for later
          stack.top().navigate(startPoint);
          CHECK (1 == stack.size());
          intrusive_ptr_add_ref (&firstFrame);
          
          ScopePath beforeInvalidNavigation = firstFrame;
          Scope const& unrelatedScope = fabricate_invalidScope();
          
          // try to navigate to an invalid place
          VERIFY_ERROR (INVALID_SCOPE, stack.top().navigate (unrelatedScope) );
          CHECK (1 == stack.size());
          CHECK (1 == firstFrame.ref_count());
          CHECK (stack.top().getLeaf() == startPoint);
          
          // try to push an invalid place
          VERIFY_ERROR (INVALID_SCOPE, stack.push (unrelatedScope) );
          CHECK (1 == stack.size());
          CHECK (1 == firstFrame.ref_count());
          CHECK (stack.top().getLeaf() == startPoint);
        }
      
      
      void
      clear ()
        {
          QueryFocusStack stack;
          intrusive_ptr_add_ref (&stack.top());
          stack.top().moveUp();
          CHECK (stack.top().empty());
          
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
          CHECK (10 == stack.size());
          stack.pop_unused();
          CHECK (10 == stack.size());
          CHECK (1 == stack.top().ref_count());
          
          stack.clear();
          CHECK (1 == stack.size());
          CHECK (!stack.top().empty());
          CHECK (stack.top().getLeaf().isRoot());
          CHECK (0 == stack.top().ref_count());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryFocusStack_test, "unit session");
  
  
}}}} // namespace proc::mobject::session::test
