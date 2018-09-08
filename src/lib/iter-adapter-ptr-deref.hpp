/*
  ITER-ADAPTER-PTR-DEREF.hpp  -  wrapping iterator to dereference pointers automatically 

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


/** @file iter-adapter-ptr-deref.hpp
 ** Extension adapter for Lumiera Forward Iterators to dereference
 ** any pointer values automatically. Sometimes, the iteration of some
 ** container naturally just yields pointers to contained values. But,
 ** from a viewpoint of interface design, we'd prefer the iterator to
 ** expose direct references (NULL values can be excluded). This helper
 ** template does precisely this: it wraps up any other entity conforming
 ** to the »Lumiera Forward Iterator« concept and, on access, automatically
 ** dereferences the exposed pointer.
 ** 
 ** In addition, the reversed operation is also supported through another
 ** helper template: to take the address of any value exposed by the given
 ** underlying iterator
 ** 
 ** @see iter-adapter-test.cpp
 ** @see iter-adapter.hpp basic iterator adapters
 ** @see itertools.hpp
 **
 */


#ifndef LIB_ITER_ADAPTER_PTR_DEREF_H
#define LIB_ITER_ADAPTER_PTR_DEREF_H


#include "lib/iter-adapter.hpp"

#include <boost/type_traits/remove_const.hpp>


namespace lib {
  
  namespace {
    
    /** helper to remove pointer,
     *  while retaining const */
    template<typename T>
    struct RemovePtr  { typedef T Type; };
    
    template<typename T>
    struct RemovePtr<T*> { typedef T Type; };
    
    template<typename T>
    struct RemovePtr<const T*> { typedef const T Type; };
    
    template<typename T>
    struct RemovePtr<T* const> { typedef const T Type; };
    
    template<typename T>
    struct RemovePtr<const T* const> { typedef const T Type; };
    
  }
  
  
  
  /**
   * wrapper for an existing Iterator type,
   * automatically dereferencing the output of the former.
   * For this to work, the "source" iterator is expected
   * to be declared on \em pointers rather than on values.
   * @note bool checkable if and only if source is...
   */
  template<class IT>
  class PtrDerefIter
    {
      IT i_;  ///< nested source iterator
      
      
    public:
      typedef typename IT::value_type           pointer;
      typedef typename RemovePtr<pointer>::Type value_type;
      typedef value_type&                       reference;
      
      // for use with STL algorithms
      typedef void difference_type;
      typedef std::forward_iterator_tag iterator_category;
      
      
      // the purpose of the following typedefs is to ease building a correct "const iterator"
      
      typedef typename boost::remove_const<value_type>::type ValueTypeBase; // value_type without const
      
      typedef typename IterType<IT>::template SimilarIter<      ValueTypeBase* * >::Type WrappedIterType;
      typedef typename IterType<IT>::template SimilarIter<const ValueTypeBase* * >::Type WrappedConstIterType;
      
      typedef PtrDerefIter<WrappedIterType>      IterType;
      typedef PtrDerefIter<WrappedConstIterType> ConstIterType;
      
      
      
      /** PtrDerefIter is always created 
       *  by wrapping an existing iterator.
       */
      explicit
      PtrDerefIter (IT srcIter)
        : i_(srcIter)
        { }
      
      
      /** allow copy initialisation also when
       *  the wrapped iterator is based on some variation of a pointer.
       *  Especially, this includes initialisation of the "const variant"
       *  from the "normal variant" of PtrDerefIter. Actually, we need to convert
       *  in this case by brute force, because indeed (const TY *)* is not assignable
       *  from (TY *)* -- just we know that our intention is to dereference both levels
       *  of pointers, and then the resulting conversion is correct.
       *  @note in case IT == WrappedIterType, this is just a redefinition of the
       *        default copy ctor. In all other cases, this is an <i>additional
       *        ctor besides the default copy ctor</i> */
      PtrDerefIter (PtrDerefIter<WrappedIterType> const& oIter)
        : i_(reinterpret_cast<IT const&> (oIter.getBase()))
        { }
      
      PtrDerefIter&
      operator= (PtrDerefIter<WrappedIterType> const& ref)
        {
          i_ = reinterpret_cast<IT const&> (ref.getBase());
          return *this;
        }
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
      
      /** explicit builder to allow creating a const variant from the basic srcIter type. 
       *  Again, the reason necessitating this "backdoor" is that we want to swallow one level
       *  of indirection. Generally speaking `const T **` is not the same as `T * const *`,
       *  but in our specific case the API ensures that a `PtrDerefIter<WrappedConstIterType>`
       *  only exposes const elements. 
       */
      static PtrDerefIter
      build_by_cast (WrappedIterType const& srcIter)
        {
          return PtrDerefIter (reinterpret_cast<IT const&> (srcIter));
        }
      
      static PtrDerefIter
      nil()
        {
          return PtrDerefIter (IT());
        }
      
      
      
      
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          return *(*i_);
        }
      
      pointer
      operator->() const
        {
          return *i_;
        }
      
      PtrDerefIter&
      operator++()
        {
          ++i_;
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return bool(i_);
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
      
      /** access the wrapped implementation iterator */
      IT const&
      getBase()  const
        {
          return i_;
        }
    };
  
  
  /// Supporting equality comparisons...
  template<class I1, class I2>
  bool operator== (PtrDerefIter<I1> const& il, PtrDerefIter<I2> const& ir)  { return il.getBase() == ir.getBase(); }
    
  template<class I1, class I2>
  bool operator!= (PtrDerefIter<I1> const& il, PtrDerefIter<I2> const& ir)  { return not (il == ir); }
  
  
  
  
  
  /** 
   * wrapper for an existing Iterator type to expose the address of each value yielded.
   * Typically this can be used to build visitation sequences based on values living
   * within a stable data structure (e.g. unmodifiable STL vector)
   * @warning use of this wrapper might lead to taking the address of temporaries.
   *          The continued existence of the exposed storage locations must be guaranteed.
   * @note bool checkable if and only if source is...
   */
  template<class IT>
  class AddressExposingIter
    {
      typedef typename IT::pointer _Ptr;
      
      IT i_;  ///< nested source iterator
      
      mutable _Ptr currPtr_;
      
      
      void
      takeAddress()
        {
          if (i_.isValid())
            currPtr_ = & (*i_);
          else
            currPtr_ = 0;
        }
      
      
    public:
      typedef typename IT::pointer const* pointer;
      typedef typename IT::pointer const& reference;
      typedef typename IT::pointer const  value_type;
      
      
      /** AddressExposingIter is always created 
       *  by wrapping an existing iterator.
       */
      explicit
      AddressExposingIter (IT srcIter)
        : i_(srcIter)
        {
          takeAddress();
        }
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
      
      
      /* === lumiera forward iterator concept === */
      
      /** @return address of the source iteraor's current result
       * @warning exposing a reference to an internal pointer for sake of compatibility.
       *          Clients must not store that reference, but rather use it to initialise
       *          a copy. The internal pointer exposed here will be changed on increment.  
       */
      reference
      operator*() const
        {
          return currPtr_;
        }
      
      _Ptr
      operator->() const
        {
          return currPtr_;
        }
      
      AddressExposingIter&
      operator++()
        {
          ++i_;
          takeAddress();
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return bool(i_);
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
      
      /** access the wrapped implementation iterator */
      IT const&
      getBase()  const
        {
          return i_;
        }
    };
  
  
  /// Supporting equality comparisons...
  template<class I1, class I2>
  bool operator== (AddressExposingIter<I1> const& il, AddressExposingIter<I2> const& ir)  { return il.getBase() == ir.getBase(); }
    
  template<class I1, class I2>
  bool operator!= (AddressExposingIter<I1> const& il, AddressExposingIter<I2> const& ir)  { return not (il == ir); }
  
  
  
  
  
  
}// namespace lib
#endif /*LIB_ITER_ADAPTER_PTR_DEREF_H*/
