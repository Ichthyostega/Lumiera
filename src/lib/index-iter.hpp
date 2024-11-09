/*
  INDEX-ITER.hpp  -  iterator with indexed random-access to referred container

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file index-iter.hpp
 ** Iterator-style access handle to a referred container with subscript index.
 ** This wrapper packages a current index number and a back-link to some data container
 ** with subscript operator and range check. This allows to hand out a navigable access point
 ** to a processing algorithm while abstracting away the actual data storage. Besides usage
 ** as »Lumiera Forward Iterator«, the current access position can be retrieved directly
 ** and it can be relocated to another valid index position; this implies also the ability
 ** to re-set the iteration to the container's start. Optionally, a smart-ptr can be
 ** embedded, allowing the handle also to own and manage the data container.
 ** 
 ** @see IndexIter_test
 ** @see iter-adapter.hpp
 ** @see [usage example](\ref lib::TextTemplate::InstanceCore)
 */


#ifndef SRC_LIB_INDEX_ITER_H
#define SRC_LIB_INDEX_ITER_H


#include "lib/iter-adapter.hpp"


namespace lib {
  namespace iter {
    
    /**
     * Implementation of a »IterStateCore«
     * to access the container through an embedded index variable.
     */
    template<typename PTR>
    struct IndexAccessCore
      {
        PTR    data_{};
        size_t idx_{0};
        
        using ResVal = decltype(data_->operator[](0));
        
        using value_type = typename meta::RefTraits<ResVal>::Value;
        using reference  = typename meta::RefTraits<ResVal>::Reference;
        
        using IterWrapper = lib::IterStateWrapper<value_type, IndexAccessCore>;
        
        bool
        checkPoint() const
          {
            return isValidIDX(idx_);
          }
        
        reference
        yield()  const
          {
            return (*data_)[idx_];
          }
        
        void
        iterNext()
          {
            ++idx_;
          }
        
        
        bool
        isValidIDX (size_t idx)  const
          {
            return bool(data_)
               and idx < data_->size();
          }
        
        
        friend bool operator== (IndexAccessCore const& c1, IndexAccessCore const& c2)
        {
          return c1.data_ == c2.data_ and (not c1.data_ or c1.idx_ == c2.idx_);
        }
        friend bool operator!= (IndexAccessCore const& c1, IndexAccessCore const& c2)
        {
          return not (c1 == c2);
        }
      };
    //
  }//(End)Implementation
  
  
  
  /**
   * Subscript-index based access to a container, packaged as iterator.
   * This is a copyable and assignable value object (handle), referring to some
   * data container maintained elsewhere. The container must provide an `operator[]`.
   * This handle can be used as »Lumiera Forward Iterator«, but with the additional
   * capability to retrieve and re-set the current index position.
   * @tparam CON a container with `operator[]` and a function `size()`
   * @tparam PTR how to refer to this container; can be defined as smart-ptr,
   *             additionally allowing to manage this container automatically.
   * @remark while a default constructed IndexIter and some _exhausted_ IndexIter
   *             compare equal, only the latter can be re-set into active state.
   */
  template<class CON, typename PTR = CON*>
  class IndexIter
    : public iter::IndexAccessCore<PTR>::IterWrapper
    {
      using _Cor = iter::IndexAccessCore<PTR>;
      using _Par = typename _Cor::IterWrapper;
      
    public:
      IndexIter()  = default;
      IndexIter (CON& container)  : IndexIter{&container}{ };
      IndexIter (PTR pContainer)
        : _Par{_Cor{pContainer, 0}}
        { }
      
      
      size_t
      getIDX()  const
        {
          _Par::__throw_if_empty();
          return _Par::stateCore().idx_;
        }
      
      void
      setIDX (size_t newIDX)
        {
          auto& core = _Par::stateCore();
          if (not core.isValidIDX (newIDX))
            throw lumiera::error::Invalid ("Attempt to set index out of bounds",
                  lumiera::error::LUMIERA_ERROR_INDEX_BOUNDS);
          core.idx_ = newIDX;
        }
    };
  
  
} // namespace lib
#endif /*SRC_LIB_INDEX_ITER_H*/
