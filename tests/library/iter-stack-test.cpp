/*
  IterStack(Test)  -  verify stack-like iterator

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-stack-test.cpp
 ** unit test \ref IterStack_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/iter-stack.hpp"



namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using LERR_(ITER_EXHAUST);
  
  
  
  
  typedef IterStack<int> IStack;
  
  
  
  
  /*****************************************************************//**
   * @test cover behaviour of a convenience class built by using a
   *       \c std::deque as "state core" of an Lumiera IterStateWrapper.
   *       - iterable according to the Lumiera Forward Iterator concept
   *       - can push and pop values repeatedly
   *       - iterating and push / pop can be mixed
   *       - empty state detected consistently
   * 
   * @see IterExplorer
   * @see IterAdapter
   */
  class IterStack_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          IStack stack;
          CHECK (isnil (stack));
          
          VERIFY_ERROR (ITER_EXHAUST,  *stack );
          VERIFY_ERROR (ITER_EXHAUST, ++stack );
          
          stack.push (1);
          stack.push (3);
          stack.push (5);
          stack.push (7);
          stack.push (9);
          
          CHECK (!isnil (stack));
          CHECK (9 == *stack);
          
          ++stack;
          CHECK (7 == *stack);
          
          CHECK (7 == stack.pop());
          CHECK (5 == *stack);
          
          ++++stack;
          CHECK (1 == *stack);
          CHECK (1 == stack.pop());
          
          CHECK (isnil (stack));
          VERIFY_ERROR (ITER_EXHAUST,  *stack );
          VERIFY_ERROR (ITER_EXHAUST, ++stack );
          VERIFY_ERROR (ITER_EXHAUST, stack.pop() );
          
          
          stack.push(23);
          CHECK (23 == *stack);
          
          int i = stack.pop();
          CHECK (i == 23);
          VERIFY_ERROR (ITER_EXHAUST,  *stack );
          CHECK (isnil (stack));
        }
      
    };
  
  
  LAUNCHER (IterStack_test, "unit common");
  
  
}} // namespace lib::test
