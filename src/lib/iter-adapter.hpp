/*
  ITER-ADAPTER.hpp  -  helpers for building simple forward iterators 

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

*/

/** @file iter-adapter.hpp
 ** Helper template(s) for creating <b>lumiera forward iterators</b>.
 ** These are the foundation to build up iterator like types from scratch.
 ** Usually, these templates will be created and provided by a custom
 ** container type and accessed by the client through a typedef name
 ** "iterator" (similar to the usage within the STL). For more advanced
 ** usage, the providing container might want to subclass these iterators,
 ** e.g. to provide an additional, specialised API.
 ** 
 ** Depending on the concrete situation, several flavours are provided:
 ** - the IterAdapter retains an active callback connection to the
 **   controlling container, thus allowing arbitrary complex behaviour.
 ** - the IterStateWrapper uses a variation of that approach, where the
 **   representation of the current state is embedded as an state value
 **   element right into the iterator instance.
 ** - the RangeIter allows just to expose a range of elements defined
 **   by a STL-like pair of "start" and "end" iterators
 ** - often, objects are managed internally by pointers, while allowing
 **   the clients to use direct references; to support this usage scenario,
 **   PtrDerefIter wraps an existing iterator, while dereferencing any value
 **   automatically on access.
 ** - for some (very specific) usage situations we intend to explore the
 **   contents of a stable and unmodifiable data structure through pointers.
 **   The AddressExposingIter wraps another Lumiera Forward Iterator and
 **   exposes addresses -- assuming the used source iterator is exposing
 **   references to pre-existing storage locations (not temporaries). 
 ** 
 ** There are many further ways of yielding a Lumiera forward iterator.
 ** For example, lib::IterSource builds a "iterable" source of data elements,
 ** while hiding the actual container or generator implementation behind a
 ** vtable call. Besides, there are adapters for the most common usages
 ** with STL containers, and such iterators can also be combined and
 ** extended with the help of itertools.hpp
 ** 
 ** Basically every class in compliance with our specific iterator concept
 ** can be used as a building block within this framework.
 ** 
 ** 
 ** \par Lumiera forward iterator concept
 ** 
 ** Similar to the STL, instead of using a common "Iterator" base class,
 ** we rather define a common set of functions and behaviour which can
 ** be expected from any such iterator. These rules are similar to STL's
 ** "forward iterator", with the addition of an bool check to detect
 ** iteration end. The latter is inspired by the \c hasNext() function
 ** found in many current languages supporting iterators. In a similar
 ** vein (inspired from functional programming), we deliberately don't
 ** support the various extended iterator concepts from STL and boost
 ** (random access iterators, output iterators, arithmetics, difference
 ** between iterators and the like). According to this concept,
 ** <i>an iterator is a promise for pulling values,</i>
 ** and nothing beyond that.
 ** 
 ** - Any Lumiera forward iterator can be in a "exhausted" (invalid) state,
 **   which can be checked by the bool conversion. Especially, any instance
 **   created by the default ctor is always fixed to that state. This
 **   state is final and can't be reset, meaning that any iterator is
 **   a disposable one-way-off object.
 ** - iterators are copyable and equality comparable
 ** - when an iterator is \em not in the exhausted state, it may be
 **   \em dereferenced to yield the "current" value.
 ** - moreover, iterators may be incremented until exhaustion.
 ** 
 ** @see iter-adapter-test.cpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 ** @see iter-type-binding.hpp
 **
 */


#ifndef LIB_ITER_ADAPTER_H
#define LIB_ITER_ADAPTER_H


#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/iter-type-binding.hpp"

#include <boost/type_traits/remove_const.hpp>


namespace lib {
  
  
  namespace { // internal helpers
    void
    _throwIterExhausted()
    {
      throw lumiera::error::Invalid ("Can't iterate further",
            lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
    }
  }
  
  

  /**
   * Adapter for building an implementation of the lumiera forward iterator concept.
   * The "current position" is represented as an opaque element (usually an nested iterator),
   * with callbacks to the controlling container instance for managing this position.
   * This allows to influence and customise the iteration process to a large extent.
   * Basically such an IterAdapter behaves like the similar concept from STL, but
   * - it is not just a disguised pointer (meaning, it's more expensive)
   * - it checks validity on every operation and may throw
   * - it has a distinct back-link to the source container
   * - the source container needs to provide hasNext() and iterNext() free functions.
   * - we may need friendship to implement those extension points on the container 
   * - the end-of-iteration can be detected by bool check
   * @note it is possible to "hide" a smart-ptr within the CON template parameter.
   * 
   * \par Stipulations
   * - POS refers to the current position within the data source of this iterator.
   *       -# it should be default constructible
   *       -# it should be copy constructible
   *       -# when IterAdapter is supposed to be assignable, then POS should be
   *       -# it should provide embedded typedefs for pointer, reference and value_type,
   *          or alternatively resolve these types through specialisation of iter::TypeBinding.
   *       -# it should be convertible to the pointer type it declares
   *       -# dereferencing should yield a type that is convertible to the reference type
   * - CON points to the data source of this iterator (typically a data container type)
   *       We store a pointer-like backlink to invoke a special iteration control API:
   *       -# \c checkPoint yields true iff the source has yet more result values to yield
   *       -# \c iterNext advances the POS to the next element 
   * 
   * @see scoped-ptrvect.hpp usage example
   * @see iter-type-binding.hpp
   * @see iter-adaptor-test.cpp
   */
  template<class POS, class CON>
  class IterAdapter
    : public lib::BoolCheckable<IterAdapter<POS,CON> >
    {
      CON source_;
      mutable POS pos_;
      
    public:
      typedef typename iter::TypeBinding<POS>::pointer pointer;
      typedef typename iter::TypeBinding<POS>::reference reference;
      typedef typename iter::TypeBinding<POS>::value_type value_type;
      
      IterAdapter (CON src, POS const& startpos)
        : source_(src)
        , pos_(startpos)
        { 
          check();
        }
      
      IterAdapter ()
        : source_()
        , pos_()
        { }
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          _maybe_throw();
          return *pos_;
        }
      
      pointer
      operator->() const
        {
          _maybe_throw();
          return pos_;
        }
      
      IterAdapter&
      operator++()
        {
          _maybe_throw();
          iterate();
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return check();
        }
      
      bool
      empty ()    const
        {
          return !isValid();
        }
      
      
    protected: /* === iteration control interface === */
      
      /** ask the controlling container if this position is valid.
       *  @note this function is called before any operation,
       *        thus the container may adjust the position value,
       *        for example setting it to a "stop iteration" mark.
       */ 
      bool
      check()  const
        {
          return source_ && checkPoint (source_,pos_);           // extension point: free function checkPoint(...)
        }
      
      /** ask the controlling container to yield the next position.
       *  The call is dispatched only if the current position is valid;
       *  any new position reached will typically be validated prior
       *  to any further access, through invocation of #check.
       */
      void
      iterate()
        {
          if (check())
            iterNext (source_,pos_);                             // extension point: free function iterNext(...)
        }
      
      
    private:
      void
      _maybe_throw()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
      
      /// comparison is allowed to access impl iterator
      template<class P1, class P2, class CX>
      friend bool operator== (IterAdapter<P1,CX> const&, IterAdapter<P2,CX> const&);
    };
  
  
  /// Supporting equality comparisons...
  template<class P1, class P2, class CON>
  bool operator== (IterAdapter<P1,CON> const& il, IterAdapter<P2,CON> const& ir)  { return il.pos_ == ir.pos_; }
  
  template<class P1, class P2, class CON>
  bool operator!= (IterAdapter<P1,CON> const& il, IterAdapter<P2,CON> const& ir)  { return !(il == ir); }
  
  
  

  /**
   * Another Lumiera Forward Iterator building block, based on incorporating a state type 
   * right into the iterator. Contrast this to IterAdapter referring to an controlling
   * container behind the scenes. Here, all of the state is assumed to live in the
   * custom type embedded into this iterator, accessed and manipulated through
   * a set of free function to be resolved by ADL.
   * 
   * \par Assumptions when building iterators based on IterStateWrapper
   * There is a custom state representation type ST.
   * - default constructible
   * - this default state represents the \em bottom (invalid) state.
   * - copyable, because iterators are passed by value
   * - this type needs to provide an <b>iteration control API</b> through free functions
   *   -# \c checkPoint establishes, if the given state element represents a valid state
   *   -# \c iterNext evolves this state by one step (sideeffect)
   *   -# \c yield realises the given state, yielding an element of result type T
   * 
   * @see IterExplorer an iterator monad built on top of IterStateWrapper
   * @see iter-explorer-test.hpp
   * @see iter-adaptor-test.cpp
   */
  template<typename T, class ST =T>
  class IterStateWrapper
    : public lib::BoolCheckable<IterStateWrapper<T,ST> >
    {
      ST core_;
      
    public:
      typedef T* pointer;
      typedef T& reference;
      typedef T  value_type;
      
      IterStateWrapper (ST const& initialState)
        : core_(initialState)
        { 
          checkPoint (core_);       // extension point: checkPoint
        }
      
      IterStateWrapper ()
        : core_()
        { }
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          __throw_if_empty();
          return yield (core_);     // extension point: yield
        }
      
      pointer
      operator->() const
        {
          __throw_if_empty();
          return & yield(core_);    // extension point: yield
        }
      
      IterStateWrapper&
      operator++()
        {
          __throw_if_empty();
          iterNext (core_);         // extension point: iterNext
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return checkPoint(core_); // extension point: checkPoint
        }
      
      bool
      empty ()    const
        {
          return !isValid();
        }
      
    protected:
      
      /** allow derived classes to
       *  access state representation */
      ST &
      stateCore()
        {
          return core_;
        }
      
      void
      __throw_if_empty()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
      
      
      /// comparison is allowed to access state implementation core
      template<class T1, class T2, class STX>
      friend bool operator== (IterStateWrapper<T1,STX> const&, IterStateWrapper<T2,STX> const&);
    };
  
  
  
  /// Supporting equality comparisons of equivalent iterators (same state type)...
  template<class T1, class T2, class ST>
  bool operator== (IterStateWrapper<T1,ST> const& il, IterStateWrapper<T2,ST> const& ir)
  {
    return (il.empty() && ir.empty())
        || (il.isValid() && ir.isValid() && il.core_ == ir.core_);
  }
  
  template<class T1, class T2, class ST>
  bool operator!= (IterStateWrapper<T1,ST> const& il, IterStateWrapper<T2,ST> const& ir)
  { 
    return ! (il == ir);
  }
  
  
  
  
  
  
  
  
  /** 
   * Accessing a STL element range through a Lumiera forward iterator,
   * An instance of this iterator adapter is completely self-contained
   * and allows to iterate once over the range of elements, until 
   * \c pos==end . Thus, a custom container may expose a range of
   * elements of an embedded STL container, without controlling
   * the details of the iteration (as is possible using the
   * more generic IterAdapter).
   */
  template<class IT>
  class RangeIter
    : public lib::BoolCheckable<RangeIter<IT> >
    {
      IT p_;
      IT e_;
      
    public:
      typedef typename iter::TypeBinding<IT>::pointer pointer;
      typedef typename iter::TypeBinding<IT>::reference reference;
      typedef typename iter::TypeBinding<IT>::value_type value_type;
      
      RangeIter (IT const& start, IT const& end)
        : p_(start)
        , e_(end)
        { }
      
      RangeIter ()
        : p_()
        , e_()
        { }
      
      
      /** allow copy,
       *  when the underlying iterators
       *  are compatible or convertible */
      template<class I2>
      RangeIter (I2 const& oIter)
        : p_(oIter.getPos())
        , e_(oIter.getEnd())
        { }
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          _maybe_throw();
          return *p_;
        }
      
      pointer
      operator->() const
        {
          _maybe_throw();
          return &(*p_);
        }
      
      RangeIter&
      operator++()
        {
          _maybe_throw();
          ++p_;
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return (p_!= IT()) && (p_ != e_);
        }
      
      bool
      empty ()    const
        {
          return !isValid();
        }
      
      
      /** access wrapped STL iterator */
      const IT&  getPos()  const { return p_; }
      const IT&  getEnd()  const { return e_; }
      
      
    private:
      
      void
      _maybe_throw()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
    };
  
  
  
  /// Supporting equality comparisons...
  template<class I1, class I2>
  bool operator== (RangeIter<I1> const& il, RangeIter<I2> const& ir)  { return (!il && !ir) || (il.getPos() == ir.getPos()); }
    
  template<class I1, class I2>
  bool operator!= (RangeIter<I1> const& il, RangeIter<I2> const& ir)  { return !(il == ir); }
  
  
  
  
  
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
   * Helper for type rewritings:
   * get the element type for an iterator like entity
   */
  template<class TY>
  struct IterType;
  
  template<template<class,class> class Iter, class TY, class CON>
  struct IterType<Iter<TY,CON> >
    {
      typedef CON Container;
      typedef TY  ElemType;
      
      template<class T2>
      struct SimilarIter  ///< rebind to a similarly structured Iterator with value type T2
        {
          typedef Iter<T2,CON> Type;
        };
    };
  
  template<class IT>
  struct IterType<RangeIter<IT> >
    : IterType<IT>
    {
      template<class T2>
      struct SimilarIter  ///< rebind to rewritten Iterator wrapped into RangeIter
        {
          typedef typename IterType<IT>::template SimilarIter<T2>::Type WrappedIter;
          typedef RangeIter<WrappedIter> Type;
        };
    };
  
  
  
  /** 
   * wrapper for an existing Iterator type,
   * automatically dereferencing the output of the former.
   * For this to work, the "source" iterator is expected
   * to be declared on \em pointers rather than on values.
   * @note bool checkable if and only if source is...
   */
  template<class IT>
  class PtrDerefIter
    : public lib::BoolCheckable<PtrDerefIter<IT> >
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
          return !isValid();
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
  bool operator!= (PtrDerefIter<I1> const& il, PtrDerefIter<I2> const& ir)  { return !(il == ir); }
  
  
  
  
  
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
    : public lib::BoolCheckable<AddressExposingIter<IT> >
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
          return !isValid();
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
  bool operator!= (AddressExposingIter<I1> const& il, AddressExposingIter<I2> const& ir)  { return !(il == ir); }
  
  
  
  
  
  
  /** wrapper to declare exposed values const */
  template<class IT>
  class ConstIter
    : public lib::BoolCheckable<ConstIter<IT> >
    {
      IT i_;  ///< nested source iterator
      
      
    public:
      typedef const typename IT::value_type value_type;
      typedef const typename IT::pointer    pointer;
      typedef const typename IT::reference  reference;
      
      ConstIter (IT srcIter)
        : i_(srcIter)
        { }
      
      
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          return *i_;
        }
      
      pointer
      operator->() const
        {
          return i_.operator->();
        }
      
      ConstIter&
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
          return !isValid();
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
  bool operator== (ConstIter<I1> const& il, ConstIter<I2> const& ir)  { return il.getBase() == ir.getBase(); }
    
  template<class I1, class I2>
  bool operator!= (ConstIter<I1> const& il, ConstIter<I2> const& ir)  { return !(il == ir); }
  
  
  
} // namespace lib
#endif
