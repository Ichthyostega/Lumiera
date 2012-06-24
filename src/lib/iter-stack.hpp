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



#ifndef LIB_ITER_STACK_H
#define LIB_ITER_STACK_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/util.hpp"

#include <deque>


namespace lib {
  
  
  namespace { 
    using util::unConst;
    
    /** 
     * Wrapper to mark a std::stack instance for use
     * as "state core" within lib::IterStateWrapper
     */
    template<class TY>
    struct IterDequeStorage
      : std::deque<TY>
      {
        /* === Iteration control API for IterStateWrapper == */
        
        friend bool
        checkPoint (IterDequeStorage const& elements)
        {
          return !elements.empty();
        }
        
        friend TY &
        yield (IterDequeStorage const& elements)
        {
          REQUIRE (!elements.empty());
          return unConst(elements).back();
        }
        
        friend void
        iterNext (IterDequeStorage & elements)
        {
          REQUIRE (!elements.empty());
          elements.pop_back();
        }
      };
  }//(End) Wrapper/Helper
  
  
  
  /** 
   * A Stack which can be popped by iterating.
   * This is a simple helper built on top of std::stack.
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
    : IterStateWrapper<TY, IterDequeStorage<TY> >
    {
      
      // using default create and copy operations
      
      void
      push (TY const& elm)
        {
          this->stateCore().push_back (elm);
        }
      
      TY
      pop()
        {
          this->__throw_if_empty();
          TY frontElement (this->stateCore().back());
          this->stateCore().pop_back();
          return frontElement;
        }
    };
  
  
} // namespace lib
#endif
