/*
  ITER-CURSOR.hpp  -  wrap bidirectional STL container iterators

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file iter-cursor.hpp
 ** An iterator with the ability to switch direction.
 ** This wrapper relies on the ability of typical STL container iterators
 ** to work in both directions, similar to std::reverse_iterator.
 ** Yet it is a single, self-contained element and in compliance to the
 ** ["Lumiera Forward Iterator"][iter-adapter.hpp] concept. But it has
 ** the additional ability to [switch the working direction][IterCursor::switchDir].
 ** 
 ** @note as of 12/2015 this is complete bs
 ** 
 ** @see IterCursor_test
 ** @see iter-adapter.hpp
 ** @see [usage example][event-log.hpp]
 ** 
 */


#ifndef SRC_LIB_ITER_CURSOR_H
#define SRC_LIB_ITER_CURSOR_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
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
        typedef typename iter::TypeBinding<IT>::pointer pointer;
        typedef typename iter::TypeBinding<IT>::reference reference;
        typedef typename iter::TypeBinding<IT>::value_type value_type;
        
        
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
        toggle()
          {
            if (start_ == end_) return;
            backwards_ = not backwards_;
            ++pos_;
          }
        
        /* === Iteration control API for IterStateWrapper == */
        
        friend bool
        checkPoint (CursorGear const& gear)
        {
          return gear.backwards_? gear.pos_ != gear.start_
                                : gear.pos_ != gear.end_;
        }
        
        friend reference
        yield (CursorGear const& gear)
        {
          return gear.backwards_? *(gear.pos_-1)
                                : *(gear.pos_);
        }
        
        friend void
        iterNext (CursorGear & gear)
        {
          if (gear.backwards_)
            --gear.pos_;
          else
            ++gear.pos_;
        }
      };
    
    
  } // namespace iter
  
  
  
  
  /**
   * A cursor-like iterator with the ability to switch iteration direction.
   * 
   */
  template<class IT>
  class IterCursor
    : public IterStateWrapper<typename iter::CursorGear<IT>::reference, iter::CursorGear<IT>>
    {
      using _Core = IterStateWrapper<typename iter::CursorGear<IT>::reference, iter::CursorGear<IT>>;
      
    public:
      IterCursor() { }
      
      template<class CON>
      explicit
      IterCursor (CON& container)
        : _Core(container.begin(), container.end())
        { }
      
      IterCursor (IT&& begin, IT&& end)
        : _Core(std::forward<IT>(begin), std::forward<IT>(end))
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
          this->stateCore().toggle();
          return *this;
        }
    };
  
  
} // namespace lib
#endif /*SRC_LIB_ITER_CURSOR_H*/
