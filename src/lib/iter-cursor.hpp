/*
  ITER-CURSOR.hpp  -  wrap bidirectional STL container iterators

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file iter-cursor.hpp
 ** An iterator with the ability to switch direction.
 ** This wrapper relies on the ability of typical STL container iterators
 ** to work in both directions, similar to std::reverse_iterator.
 ** Yet it is a single, self-contained element and in compliance to the
 ** ["Lumiera Forward Iterator"](\ref iter-adapter.hpp) concept. But it has
 ** the additional ability to [switch the working direction](\ref IterCursor<IT>::switchDir).
 ** 
 ** @see IterCursor_test
 ** @see iter-adapter.hpp
 ** @see [usage example](\ref event-log.hpp)
 ** 
 */


#ifndef SRC_LIB_ITER_CURSOR_H
#define SRC_LIB_ITER_CURSOR_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"

#include <type_traits>
#include <utility>


namespace lib {
  namespace iter {
    
    /**
     * @internal implementation for a "gear switching" iterator,
     * based on STL container iterators.
     */
    template<class IT>
    class CursorGear
      {
        bool backwards_{false};
        const IT start_;
        const IT end_;
        
        IT pos_;
        
        
      public:
        using pointer    = typename meta::ValueTypeBinding<IT>::pointer;
        using reference  = typename meta::ValueTypeBinding<IT>::reference;
        using value_type = typename std::remove_reference<reference>::type;   ///< @note will be const for const iterators (while const_iterator::value_type isn't)
        
        
        CursorGear()
          : start_()
          , end_()
          , pos_()
          { }
        
        CursorGear (IT&& begin, IT&& end)
          : start_(std::forward<IT>(begin))
          , end_(std::forward<IT>(end))
          , pos_(start_)
          { }
        
        // using default copy/assignment
        
        
        void
        reverse(bool backwards)
          {
            if (backwards != backwards_) reverse();
          }
        
        void
        reverse()
          {
            if (start_ == end_) return;
            if (backwards_)
              {
                if (pos_ != start_) --pos_;
                backwards_ = false;
              }
            else
              {
                if (pos_ != end_) ++pos_;
                backwards_ = true;
              }
          }
        
        /* === Iteration control API for IterStateWrapper == */
        
        bool
        checkPoint()  const
          {
            return backwards_? pos_ != start_
                             : pos_ != end_;
          }
        
        reference
        yield()  const
          {
            return backwards_? *(pos_-1)
                             : *(pos_);
          }
        
        void
        iterNext()
          {
            if (backwards_)
              --pos_;
            else
              ++pos_;
          }
        
        
        friend bool
        operator== (CursorGear const& g1, CursorGear const& g2)
        {
          return (not g1.checkPoint() and not g2.checkPoint()) // note: all exhausted iters are equal 
              or (   g1.pos_ == g2.pos_
                 and g1.backwards_ == g2.backwards_
                 and g1.start_ == g2.start_
                 and g1.end_ == g2.end_
                 );
        }
      };
    
    
  } // namespace iter
  
  
  
  
  /**
   * A cursor-like iterator with the ability to switch iteration direction.
   * It can be built on top of any bidirectional STL iterator or similar entity,
   * which has an `--` operator. Initially, IterCursor will operate in forward
   * direction; irrespective of the current direction, it always fulfils the
   * ["Lumiera Forward Iterator"](\ref iter-adapter.hpp) concept, i.e. it can be
   * iterated until exhaustion, in which case it will evaluate to bool(false).
   * @note IterCursor instances can be equality compared, also taking the
   *       current direction into account. As a special case, all
   *       exhausted iterators are treated as equal.
   */
  template<class IT>
  class IterCursor
    : public IterStateWrapper<iter::CursorGear<IT>>
    {
      using _Core = iter::CursorGear<IT>;
      using _Parent = IterStateWrapper<_Core>;
      
    public:
      IterCursor()                              =default;
      IterCursor (IterCursor&&)                 =default;
      IterCursor (IterCursor const&)            =default;
      IterCursor& operator= (IterCursor&&)      =default;
      IterCursor& operator= (IterCursor const&) =default;
      
      
      template<class CON>
      explicit
      IterCursor (CON& container)
        : _Parent(_Core(container.begin(), container.end()))
        { }
      
      IterCursor (IT&& begin, IT&& end)
        : _Parent(_Core(std::forward<IT>(begin), std::forward<IT>(end)))
        { }
      
      
      /**
       * change the orientation of iteration.
       * A forward oriented iteration will continue backwards,
       * and vice versa. This operation can even be invoked
       * on an already exhausted iterator, in which case
       * it will turn back in reversed direction.
       */
      IterCursor&
      switchDir()
        {
          this->stateCore().reverse();
          return *this;
        }
      
      IterCursor&
      switchForwards()
        {
          this->stateCore().reverse(false);
          return *this;
        }
      
      IterCursor&
      switchBackwards()
        {
          this->stateCore().reverse(true);
          return *this;
        }
    };
  
  
} // namespace lib
#endif /*SRC_LIB_ITER_CURSOR_H*/
