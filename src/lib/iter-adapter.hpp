/*
  ITER-ADAPTOR.hpp  -  helpers for building simple forward iterators 
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 ** Usually, these templates will be created and provided by a custom
 ** container type and accessed by the client through a typedef name
 ** "iterator" (similar to the usage within the STL). For more advanced
 ** usage, the providing container might want to subclass these iterators,
 ** e.g. to provide an additional, specialised API.
 ** 
 ** Depending on the concrete situation, there are several flavours
 ** - the IterAdapter retains an active callback connection to the
 **   controlling container, thus allowing arbitrary complex behaviour.
 ** - the RangeIter allows just to expose a range of elements defined
 **   by a STL-like pair of "start" and "end" iterators
 ** - often, objects are managed internally by pointers, while allowing
 **   the clients to use direct references; to support this usage scenario,
 **   PtrDerefIter wraps an existing iterator, while dereferencing any value
 **   automatically on access.  
 ** 
 ** 
 ** \par Lumiera forward iterator concept
 ** 
 ** Similar to the STL, instead of using a common "Iterator" base class,
 ** instead we define a common set of functions and behaviour which can
 ** be expected from any such iterator. These rules are similar to STL's
 ** "forward iterator", with the addition of an bool check to detect
 ** iteration end. The latter s inspired by the \c hasNext() function
 ** found in many current languages supporting iterators. In a similar
 ** vein (inspired from functional programming), we deliberately don't
 ** support the various extended iterator concepts from STL and boost
 ** (random access iterators, output iterators and the like). According
 ** to this concept, <i>an iterator is a promise for pulling values,</i>
 ** and nothing beyond that.
 ** 
 ** - Any Lumiera forward iterator can be in a "exhausted" (invalid) state,
 **   which can be checked by the bool conversion. Especially, any instance
 **   created by the default ctor is always fixed to that state. This
 **   state is final and can't be reset, meaning that any iterator is
 **   a disposable one-way-off object.
 ** - iterators are copyable and comparable
 ** - when an iterator is \em not in the exhausted state, it may be
 **   \em dereferenced to yield the "current" value.
 ** - moreover, iterators may be incremented until exhaustion.
 ** 
 ** @todo WIP WIP WIP
 ** @todo see Ticket #182
 ** 
 ** @see scoped-ptrvect.hpp
 */


#ifndef LIB_ITER_ADAPTOR_H
#define LIB_ITER_ADAPTOR_H


#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"

#include <boost/type_traits/remove_const.hpp>



namespace lib {
  
  
  namespace {
    /** 
     * Helper for creating nested typedefs
     * within the iterator adaptor, similar to what the STL does.
     */
    template<typename TY>
    struct IterTraits
      {
        typedef typename TY::pointer pointer;
        typedef typename TY::reference reference;
        typedef typename TY::value_type value_type;
      };
    
    template<typename TY>
    struct IterTraits<TY *>
      {
        typedef TY value_type;
        typedef TY& reference;
        typedef TY* pointer;
      };
    
    template<typename TY>
    struct IterTraits<const TY *>
      {
        typedef TY value_type;
        typedef const TY& reference;
        typedef const TY* pointer;
      };
    
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
   * - the source container needs to implement iterStart() and iterInc()
   * - we need friendship to access the callbacks on the container 
   * - the end-of-iteration can be detected by bool check
   * 
   * \par Stipulations
   * - POS refers to the current position within the data source of this iterator.
   *       -# it should be default constructible
   *       -# it should be copy constructible
   *       -# when IterAdapter is supposed to be assignable, then POS should be
   *       -# it should provide embedded typedefs for pointer, reference and value_type,
   *          or alternatively resolve these types through a specialisation if IterTraits.
   *       -# it should be convertible to the pointer type it declares
   *       -# dereferencing it should yield type that is convertible to the reference type
   * - CON points to the data source of this iterator (typically a data container type)
   *       We store a pointer-like backlink to invoke a special iteration control API:
   *       -# \c hasNext yields true iff the source has yet more result values to yield
   *       -# \c iterNext advances the POS to the next element 
   * 
   * @see scoped-ptrvect.hpp usage example
   * @see iter-adaptor-test.cpp
   */
  template<class POS, class CON>
  class IterAdapter
    : public lib::BoolCheckable<IterAdapter<POS,CON> >
    {
      CON source_;
      mutable POS pos_;
      
    public:
      typedef typename IterTraits<POS>::pointer pointer;
      typedef typename IterTraits<POS>::reference reference;
      typedef typename IterTraits<POS>::value_type value_type;
      
      IterAdapter (CON src, POS const& startpos)
        : source_(src)
        , pos_(startpos)
        { 
          checkPos();
        }
      
      IterAdapter ()
        : source_(0)
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
      
      IterAdapter
      operator++(int)
        {
          _maybe_throw();
          IterAdapter oldPos(*this);
          iterate();
          return oldPos;
        }
      
      bool
      isValid ()  const
        {
          return checkPos();
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
      checkPos()  const
      {
        return source_ && CON::hasNext (source_,pos_);
      }
      
      /** ask the controlling container to yield the next position.
       *  The call is dispatched only if the current position is valid;
       *  any new position returned is again validated, so to detect
       *  the iteration end as soon as possible.
       */
      bool
      iterate ()
      {
        if (!checkPos()) return false;
        
        CON::iterNext (source_,pos_);
        return checkPos();
      }
      
      
    private:
      
      void
      _maybe_throw()  const
        {
          if (!isValid())
            throw lumiera::error::Invalid ("Can't iterate further",
                  lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
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
      typedef typename IterTraits<IT>::pointer pointer;
      typedef typename IterTraits<IT>::reference reference;
      typedef typename IterTraits<IT>::value_type value_type;
      
      RangeIter (IT const& start, IT const& end)
        : p_(start)
        , e_(end)
        { }
      
      RangeIter ()
        : p_(0)
        , e_(0)
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
      
      RangeIter
      operator++(int)
        {
          _maybe_throw();
          return RangeIter (p_++,e_);
        }
      
      bool
      isValid ()  const
        {
          return (p_!= IT(0)) && (p_ != e_);
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
            throw lumiera::error::Invalid ("Can't iterate further",
                  lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
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
      
      typedef typename RemovePtr<TY>::Type ValueType;
      
      template<class T2>
      struct SimilarIter
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
      
      // the purpose of the following typedefs is to ease building a correct "const iterator"
      
      typedef typename boost::remove_const<value_type>::type ValueTypeBase; // value_type without const
      
      typedef typename IterType<IT>::template SimilarIter<      ValueTypeBase* * >::Type WrappedIterType;
      typedef typename IterType<IT>::template SimilarIter<const ValueTypeBase* * >::Type WrappedConstIterType;
      
      typedef PtrDerefIter<WrappedIterType>      IterType;
      typedef PtrDerefIter<WrappedConstIterType> ConstIterType;
      
      
      
      /** PtrDerefIter is always created 
       *  by wrapping an existing iterator.
       */
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
      
      PtrDerefIter
      operator++(int)
        {
          return PtrDerefIter (i_++);
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
  
  
} // namespace lib
#endif
