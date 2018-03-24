/*
  ITER-STACK.hpp  -  a stack which can be popped by iterating 

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file iter-stack.hpp
 ** Conveniently iterable stack and queue containers.
 ** Implemented as thin wrapper on top of std::deque
 ** In addition to providing a simplified interface, these wrappers comply
 ** to the "Lumiera Forward Iterator" concept, allowing to retrieve and discharge the
 ** contents of the container in a standard fashion, especially by using util::for_each,
 ** or by feeding them into a iterator based pipeline.
 ** 
 ** \par the builder extension point
 ** while Lumiera Forward Iterators only allow to retrieve some elements once,
 ** the special extension point \c build(sequence) allows abstracted access to some
 ** limited manipulation of the sequence, without tying the client to a specific
 ** sequence or iterator implementation. This is intended for library templates,
 ** which typically can't make any assumptions regarding the iterator types
 ** used for instantiation.
 ** 
 ** @remarks contrary to the usual (STL) containers, this wrapper \em is an iterator
 **          and a container at the same time. Which opens the interesting possibility
 **          to intermix retrieval and feeding of new elements. It can be seen as a
 **          zero overhead adapter to ease the use within Lumiera's library framework.
 ** @note    EX_STRONG, since std::deque gives this guarantee for push and pop operations    
 ** @warning STL containers aren't thread safe. 
 ** 
 ** @see IterStack_test
 ** @see IterQueue_test
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_STACK_H
#define LIB_ITER_STACK_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <deque>
#include <utility>


namespace lib {
  
  using util::unConst;
  
  namespace iter {
    
    /**
     * Wrapper to mark a std::deque instance for use
     * as "state core" within lib::IterStateWrapper.
     * Iteration will pop elements from the back side.
     */
    template<class TY>
    struct IterDequeStorage
      : std::deque<TY>
      {
        /* === Iteration control API for IterStateWrapper == */
        
        bool
        checkPoint()  const
          {
            return not this->empty();
          }
        
        TY &
        yield()  const
          {
            REQUIRE (not this->empty());
            return unConst(this)->back();
          }
        
        void
        iterNext()
          {
            REQUIRE (not this->empty());
            this->pop_back();
          }
      };
  } // namespace lib::iter
  
  
  
  /** 
   * A Stack which can be popped by iterating.
   * This is a simple helper built by wrapping up
   * STL's double ended queue (deque).
   * Thus, each instance holds the full state, which is
   * actually kept in heap allocated storage. Pushing of
   * new elements and iterator use may be mixed.
   * 
   * Contrary to just using std::stack
   * - the iteration is compliant to the Lumiera Forward Iterator concept
   * - there is a simplified #pop() function which removes and returns at once
   */
  template<class TY>
  struct IterStack
    : IterStateWrapper<TY, iter::IterDequeStorage<TY>>
    {
      
      // using default create and copy operations
      
      IterStack&
      push (TY const& elm)
        {
          this->stateCore().push_back (elm);
          return *this;
        }
      
      IterStack&
      push (TY&& elm)
        {
          this->stateCore().emplace_back (std::move (elm));
          return *this;
        }
      
      IterStack&
      insert (TY const& elm)
        {
          return push(elm);
        }
      
      
      TY
      pop()
        {
          this->__throw_if_empty();
          TY topElement (std::move (this->stateCore().back()));
          this->stateCore().pop_back();
          return topElement;
        }
      
      
      /* == diagnostics == */
      
      size_t
      size()  const
        {
          return unConst(this)->stateCore().size();
        }
      
      bool
      empty() const
        {
          return 0 == size();
        }
    };
  
  
  
  
  /** 
   * A Queue which can be pulled by iterating.
   * As a variation of the IterStack frontend, here the
   * exposed additional interface works like a queue:
   * New elements can be fed, and the iteration will
   * consume the contents first-in first-out. The
   * implementation is based on std::deque, providing
   * both feed and the iterating operation in amortised
   * constant time, using chunk wise heap allocations.
   * Feeding of new elements into the queue and
   * retrieving old elements by iteration 
   * may be mixed freely.
   */
  template<class TY>
  struct IterQueue
    : IterStateWrapper<TY, iter::IterDequeStorage<TY>>
    {
      
      // using default create and copy operations
      
      IterQueue&
      feed (TY const& elm)
        {
          this->stateCore().push_front (elm);
          return *this;
        }
      
      IterQueue&
      feed (TY&& elm)
        {
          this->stateCore().emplace_front (std::move (elm));
          return *this;
        }
      
      IterQueue&
      insert (TY const& elm)
        {
          return feed(elm);
        }
      
      TY
      pop()
        {
          this->__throw_if_empty();
          TY firstElement (std::move (this->stateCore().back()));
          this->stateCore().pop_back();
          return firstElement;
        }
      
      
      /* == diagnostics == */
      
      size_t
      size()  const
        {
          return unConst(this)->stateCore().size();
        }
      
      bool
      empty() const
        {
          return 0 == size();
        }
      
      
      /** 
       * Adapter for use as opaque sequence.
       * This builder exposes generic operations
       * to prepare and pre-fill a sequence
       */
      struct Builder
        : util::Cloneable
        {
          Builder(IterQueue& initialElements)
            : queue_(initialElements)
            { }
           // standard copy operations allowed
          
          template<typename IT>
          IterQueue &
          usingSequence (IT src)
            {
              for ( ; src; ++src )
                queue_.feed (*src);
              return queue_;
            }
          
          IterQueue &
          wrapping (TY const& elm)
            {
              queue_.feed (elm);
              return queue_;
            }
          
        private:
          IterQueue& queue_;
        };
      
      /** Extension point to be picked up by ADL.
       *  The exposed Builder object allows client code
       *  to prepare and pre-fill the queue, without
       *  being tied to the implementation type
       * @param initial the initial contents of the queue
       *        to start with, maybe empty. This parameter
       *        also acts as type tag to pick this \c build()
       *        function by ADL 
       */
      friend IterQueue::Builder
      build (IterQueue& initial)
      {
        return Builder(initial);
      }
    };
  
  
  /** convenience free function to build an iterable sequence */
  template<typename T>
  inline IterQueue<T>
  elements (T const& elm)
  {
    return IterQueue<T>().feed(elm);
  }
  
  template<typename T>
  inline IterQueue<T>
  elements (T const& e0, T const& e1)
  {
    return IterQueue<T>().feed(e0).feed(e1);
  }
  
  template<typename T>
  inline IterQueue<T>
  elements (T const& e0, T const& e1, T const& e2)
  {
    return IterQueue<T>().feed(e0).feed(e1).feed(e2);
  }
  
  template<typename T>
  inline IterQueue<T>
  elements (T const& e0, T const& e1, T const& e2, T const& e3)
  {
    return IterQueue<T>().feed(e0).feed(e1).feed(e2).feed(e3);
  }
  
  template<typename T>
  inline IterQueue<T>
  elements (T const& e0, T const& e1, T const& e2, T const& e3, T const& e4)
  {
    return IterQueue<T>().feed(e0).feed(e1).feed(e2).feed(e3).feed(e4);
  }
  
  
  
} // namespace lib
#endif
