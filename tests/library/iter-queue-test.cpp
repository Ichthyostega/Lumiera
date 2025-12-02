/*
  IterQueue(Test)  -  verify queue-like iterator and builder function

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-queue-test.cpp
 ** unit test \ref IterQueue_test
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
  
  
  
  
  
  
  /*****************************************************************//**
   * @test cover an easy to use queue, built as convenience wrapper
   *       on top of \c std::deque and allowing element retrieval
   *       by iteration.
   *       - iterable according to the Lumiera Forward Iterator concept
   *       - can enqueue and dequeue elements repeatedly
   *       - can be loaded through an generic builder API.
   * 
   * @see IterExplorer
   * @see IterAdapter
   */
  class IterQueue_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          IterQueue<int> queue;
          CHECK (isnil (queue));
          
          VERIFY_ERROR (ITER_EXHAUST,  *queue );
          VERIFY_ERROR (ITER_EXHAUST, ++queue );
          
          queue.feed (1);
          queue.feed (3);
          queue.feed (5);
          
          CHECK (!isnil (queue));
          CHECK (1 == *queue);
          
          ++queue;
          CHECK (3 == *queue);
          
          CHECK (3 == queue.pop());
          CHECK (5 == *queue);
          
          ++queue;
          CHECK (isnil (queue));
          VERIFY_ERROR (ITER_EXHAUST,  *queue );
          VERIFY_ERROR (ITER_EXHAUST, ++queue );
          VERIFY_ERROR (ITER_EXHAUST, queue.pop() );
          
          
          // use the generic builder API to feed
          // the contents of another iterator into the queue
          queue = build(queue).usingSequence (elements (23,45));
          
          int i = queue.pop();
          CHECK (i == 23);
          CHECK (45 == *queue);
          
          // feeding new elements and pulling / iteration can be mixed
          queue.feed(67);
          CHECK (45 == *queue);
          ++queue;
          CHECK (67 == *queue);
          ++queue;
          CHECK (isnil (queue));
          queue.feed(89);
          CHECK (89 == *queue);
          queue.pop();
          VERIFY_ERROR (ITER_EXHAUST,  *queue );
        }
      
    };
  
  
  LAUNCHER (IterQueue_test, "unit common");
  
  
}} // namespace lib::test
