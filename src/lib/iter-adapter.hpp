/*
  ITER-ADAPTER.hpp  -  helpers for building simple forward iterators

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>
     2017,2024,       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file iter-adapter.hpp
 ** Helper template(s) for creating *Lumiera Forward Iterators*.
 ** These are the foundation to build up iterator like types from scratch.
 ** Usually, these templates will be created and provided by a custom
 ** container type and accessed by the client through a typedef name
 ** "`iterator`" (similar to the usage within the STL). For more advanced
 ** usage, the providing container might want to subclass these iterators,
 ** e.g. to provide an additional, specialised API.
 ** 
 ** Depending on the concrete situation, several flavours are provided:
 ** - the IterAdapter retains an active callback connection to the
 **   controlling container, thus allowing arbitrary complex behaviour.
 ** - the IterStateWrapper uses a variation of that approach, where the
 **   representation of the current state is embedded as a *State Core*
 **   value element right into the iterator instance.
 ** - very similar is IterableDecorator, but this time directly as
 **   decorator to inherit from the »state core«, and without checks.
 ** - the RangeIter allows just to expose a range of elements defined
 **   by a STL-like pair of "start" and "end" iterators
 ** 
 ** Some more specific use cases are provided in the extension header
 ** iter-adapter-ptr-deref.hpp
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
 ** There are many further ways of building a Lumiera Forward Iterator.
 ** For example, lib::IterSource exposes an "iterable" source of data elements,
 ** while hiding the actual container or generator implementation behind a
 ** VTable call. Furthermore, complex processing chains with recursive
 ** expansion can be built with the \ref IterExporer builder function.
 ** Besides, there are adapters for the most common usages with STL
 ** containers, and such iterators can also be combined and
 ** extended with the help of \ref itertools.hpp
 ** 
 ** Basically every class in compliance with our specific iterator concept
 ** can be used as a building block within this framework.
 ** 
 ** 
 ** # Lumiera Forward Iterator concept
 ** 
 ** Similar to the STL, instead of using a common "Iterator" base class,
 ** we rather define a common set of functions and behaviour which can be
 ** expected from any such iterator. These rules are similar to STL's
 ** "forward iterator", with the addition of an bool check to detect
 ** iteration end. The latter is inspired by the \c hasNext() function
 ** found in many current languages supporting iterators. However, by
 ** inspiration from functional programming, we deliberately do not
 ** support the various extended iterator concepts from STL and boost
 ** (random access iterators, output iterators, arithmetics, difference
 ** between iterators and the like). According to this concept,
 ** _an iterator is a promise for pulling values once,_
 ** and nothing beyond that.
 ** 
 ** Notably,
 ** - any Lumiera forward iterator can be in a "exhausted" (invalid) state,
 **   which can be checked by the bool conversion. Especially, an instance
 **   created by the default ctor is always fixed to that state. This
 **   state is final and can not be reset, meaning that any iterator
 **   is a disposable one-way-off object.
 ** - iterators are copyable and equality comparable
 ** - when an iterator is _not_ in the exhausted state, it may be
 **   _dereferenced_ to yield the _current value_.
 ** - usually, the _current value_ is exposed by-ref (but by-val is possible)
 ** - moreover, iterators may be incremented until exhaustion.
 ** 
 ** Conceptually, a Lumiera Iterator represents a lazy stream of calculations
 ** rather than a target value considered to be »within« a container. And while
 ** the result is in may cases deliberately _exposed as a reference,_ in order
 ** to keep the door open for special-case manipulations, for the typical usage
 ** it is _discouraged_ to assume anything about the source, beyond the limited
 ** access to some transient state as exposed during active iteration. Together,
 ** these rules enable a _loose coupling_ to the source of data.
 ** 
 ** @see iter-adapter-test.cpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 ** @see value-type-binding.hpp
 **
 */


#ifndef LIB_ITER_ADAPTER_H
#define LIB_ITER_ADAPTER_H


#include "lib/error.hpp"
#include "lib/meta/value-type-binding.hpp"

#include <iterator>


namespace util {   // see lib/util.hpp
  template<class OBJ>
  OBJ* unConst (const OBJ*);
  template<class OBJ>
  OBJ& unConst (OBJ const&);
}

namespace lib {
  
  
  namespace { // internal helpers
    inline void
    _throwIterExhausted()
    {
      throw lumiera::error::Invalid ("Can't iterate further",
            lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
    }
  }
  
  /** use a given Lumiera Forward Iterator in standard "range for loops" */
#define ENABLE_USE_IN_STD_RANGE_FOR_LOOPS(ITER)                              \
      friend ITER   begin (ITER const& it){ return it; }                      \
      friend ITER&& begin (ITER&& it)     { return static_cast<ITER&&> (it); } \
      friend ITER   end   (ITER const&)   { return ITER(); }                    \
      using iterator_category = std::input_iterator_tag;                         \
      using difference_type = size_t;

  /** define increment operator forwarding to baseclass but returning current */
#define LIFT_PARENT_INCREMENT_OPERATOR(_BASECLASS_)\
      auto&                       \
      operator++()                 \
        {                           \
          _BASECLASS_::operator++(); \
          return *this;               \
        }
  
  
  namespace iter {
    /** type binding helper: an iterato's actual result type */
    template<class IT>
    using Yield = decltype(std::declval<IT>().operator*());
    
    /** the _result type_ yielded by a »state core« */
    template<class COR>
    using CoreYield = decltype(std::declval<COR>().yield());
    
  }
  
  

  /**
   * Adapter for building an implementation of the »Lumiera Forward Iterator« concept.
   * The "current position" is represented as an opaque element (usually a nested iterator),
   * with callbacks into the controlling container instance to manage this position.
   * This allows to influence and customise the iteration process to a large extent.
   * Basically such an IterAdapter behaves like the similar concept from STL, but
   * - it is not just a disguised pointer (meaning, it's more expensive)
   * - it checks validity on every operation and may throw
   * - it has a distinct back-link to the source container
   * - the source container needs to support `checkPoint()` and `iterNext()` free functions.
   * - we may need friendship to implement those extension points on the container
   * - the end-of-iteration can be detected by bool check
   * @note it is possible to "hide" a smart-ptr within the CON template parameter.
   * 
   * @tparam POS pointer or similar mutable link to the _current value_.
   *             Will be `bool()` checked to detect iteration end, end else dereferenced.
   * @tparam CON type of the backing container, which needs to implement two
   *             extension point functions for iteration control
   * 
   * \par Stipulations
   * - POS refers to the current position within the data source of this iterator.
   *       -# it should be default constructible
   *       -# it should be copy constructible
   *       -# when IterAdapter is supposed to be assignable, then POS should be
   *       -# it should provide embedded typedefs for pointer, reference and value_type,
   *          or alternatively resolve these types through specialisation of meta::ValueTypeBinding.
   *       -# it should be convertible to the pointer type it declares
   *       -# dereferencing should yield a type that is convertible to the reference type
   * - CON points to the data source of this iterator (typically a data container type)
   *       We store a pointer-like backlink to invoke a special iteration control API:
   *       -# \c checkPoint yields true iff the source has yet more result values to yield
   *       -# \c iterNext advances the POS to the next element
   * 
   * @note
   *  - when POS is just a pointer, we use the pointee as value type
   *  - but when POS is a class, we expect the usual STL style nested typedefs
   *    `value_type`, `reference` and `pointer`
   * 
   * @see scoped-ptrvect.hpp usage example
   * @see value-type-binding.hpp
   * @see iter-adapter-test.cpp
   */
  template<class POS, class CON>
  class IterAdapter
    {
      CON source_;
      mutable POS pos_;
      
      using _ValTrait = meta::ValueTypeBinding<std::remove_pointer_t<POS>>;
      
    public:
      using value_type = typename _ValTrait::value_type;
      using reference = typename _ValTrait::reference;
      using pointer = typename _ValTrait::pointer;
      
      
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
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
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
          return & *pos_;
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
          return not isValid();
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
          iterNext (source_,pos_);                               // extension point: free function iterNext(...)
          check();
        }    // checkPoint() might mark end condition
            //  for comparison with IterAdapter{}
      
      
      
    protected:
      using ConRef = typename meta::RefTraits<CON>::Reference;
      
      /** allow derived classes to access backing container */
      ConRef       source()       { return                source_; }
      const ConRef source() const { return unConst(this)->source_; }
      
      void
      resetPos (POS otherPos)                                    ////////////////////////////////////////////TICKET #1125 : get rid of this function! it should not be there; rectify IterSource!
        {
          pos_ = otherPos;
          check();
        }
      
    private:
      void
      _maybe_throw()  const
        {
          if (not isValid())
            _throwIterExhausted();
        }
      
      
    public:
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (IterAdapter);
      
      /// comparison is allowed to access impl iterator
      template<class P1, class P2, class CX>
      friend bool operator== (IterAdapter<P1,CX> const&, IterAdapter<P2,CX> const&);
    };
  
  
  /// Supporting equality comparisons...
  template<class P1, class P2, class CON>
  inline bool operator== (IterAdapter<P1,CON> const& il, IterAdapter<P2,CON> const& ir)  { return il.pos_ == ir.pos_; }
  
  template<class P1, class P2, class CON>
  inline bool operator!= (IterAdapter<P1,CON> const& il, IterAdapter<P2,CON> const& ir)  { return not (il == ir); }
  
  
  

  /**
   * Another Lumiera Forward Iterator building block, based on incorporating a state type
   * as »*State Core*«, right into the iterator. Contrast this to IterAdapter, which refers to
   * a managing container behind the scenes. To the contrary, here all of the state is assumed
   * to live in the custom type embedded into this iterator, accessed and manipulated through
   * a dedicated _iteration control API_ exposed as member functions.
   * 
   * # Requirements for a »State Core«
   * When building iterators with the help of IterStateWrapper or \ref IterableAdapter,
   * it is assumed that the adapted _state core_ type represents a process of state evolution,
   * which reaches a well defined end state eventually, but this end state is also the _bottom_
   * - the core is default constructible
   * - this default state represents the _bottom_ (final, invalid) state.
   * - copyable, because iterators are passed by value; ideally also assignable
   * - this type needs to provide an *iteration control API* with the following operations
   *   -# \c checkPoint establishes if the given state element represents a valid active state
   *   -# \c iterNext evolves this state by one step (sideeffect)
   *   -# \c yield realises the given state, yielding an element of _result type_ \a T
   * @tparam T nominal result type (maybe const, but without reference).
   *         The resulting iterator will yield a reference to this type T
   * @tparam ST type of the »state core«, defaults to T.
   *         The resulting iterator will hold an instance of ST, which thus
   *         needs to be copyable and default constructible to the extent
   *         this is required for the iterator as such.
   * @see IterableDecorator for variation of the same concept
   * @see iter-explorer-test.hpp
   * @see iter-adaptor-test.cpp
   */
  template<typename T, class ST>
  class IterStateWrapper
    {
      ST core_;
      
    public:
      typedef T* pointer;
      typedef T& reference;
      typedef T  value_type;
      
      IterStateWrapper (ST&& initialState)
        : core_(std::forward<ST>(initialState))
        { }
      
      IterStateWrapper (ST const& initialState)
        : core_(initialState)
        { }
      
      IterStateWrapper ()
        : core_()
        { }
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          __throw_if_empty();
          return core_.yield();      // core interface: yield
        }
      
      pointer
      operator->() const
        {
          __throw_if_empty();
          return & core_.yield();    // core interface: yield
        }
      
      IterStateWrapper&
      operator++()
        {
          __throw_if_empty();
          core_.iterNext();          // core interface: iterNext
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return core_.checkPoint(); // core interface: checkPoint
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
    protected:
      
      /** allow derived classes to
       *  access state representation */
      ST &      stateCore()       { return core_; }
      ST const& stateCore() const { return core_; }
      
      void
      __throw_if_empty()  const
        {
          if (not isValid())
            _throwIterExhausted();
        }
      
      
      
    public:
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (IterStateWrapper);
      
      /// comparison is allowed to access state implementation core
      template<class T1, class T2, class STX>
      friend bool operator== (IterStateWrapper<T1,STX> const&, IterStateWrapper<T2,STX> const&);
    };
  
  
  
  /// Supporting equality comparisons of equivalent iterators (same state type)...
  template<class T1, class T2, class ST>
  inline bool
  operator== (IterStateWrapper<T1,ST> const& il, IterStateWrapper<T2,ST> const& ir)
  {
    return (il.empty()   and ir.empty())
        or (il.isValid() and ir.isValid() and il.core_ == ir.core_);
  }
  
  template<class T1, class T2, class ST>
  inline bool
  operator!= (IterStateWrapper<T1,ST> const& il, IterStateWrapper<T2,ST> const& ir)
  {
    return not (il == ir);
  }
  
  
  
  
  /**
   * Adapter to dress up an existing »Lumiera Forward Iterator« as »state core«.
   * This building block achieves the complement of \ref IterStateWrapper by providing
   * the API functions expected by the latter's _state protocol;_ a combination of
   * IterStateCore and IterStateWrapper layered on top behaves identical to the
   * original iterator. This can be used to change some aspects of the behaviour.
   * @remark directly layered by inheritance, thus public functions of the
   *         wrapped iterator remain visible (contrary to IterStateWrapper)
   */
  template<class IT>
  class IterStateCore
    : public IT
    {
      static_assert (lib::meta::can_IterForEach<IT>::value
                    ,"Lumiera Iterator required as source");
    protected:
      IT&
      srcIter()  const
        {
          return util::unConst(*this);
        }
      
    public:
      using IT::IT;
      
      /* === state protocol API for IterStateWrapper === */
      bool
      checkPoint()  const
        {
          return bool(srcIter());
        }
      
      typename IT::reference
      yield()  const
        {
          return *srcIter();
        }
      
      void
      iterNext()
        {
          ++ srcIter();
        }
    };
  
  
  
  /**
   * Adapter to add sanity checks to a »state core«.
   * @remark It is recommended to perform this kind of sanity checking by default,
   *         since the performance overhead is minute compared even to a virtual function
   *         call. However, there might be high-performance usage scenarios, where it is
   *         essential for the optimiser to be able to "strip every wart".
   */
  template<class COR>
  class CheckedCore
    : public COR
    {
      static_assert (lib::meta::is_StateCore<COR>::value
                    ,"Adapted type must expose a »state core« API");
    protected:
      COR&
      _rawCore()  const
        {
          return util::unConst(*this);
        }
      
      void
      __throw_if_empty()  const
        {
          if (not checkPoint())
            _throwIterExhausted();
        }
      
    public:
      /** blindly pass-down any argument...
       * @remark allows slicing move-initialisation from decorated
       */
      template<typename...ARGS>
      CheckedCore (ARGS&& ...init)
        : COR(std::forward<ARGS>(init)...)
        { }
      
      CheckedCore()                               =default;
      CheckedCore (CheckedCore&&)                 =default;
      CheckedCore (CheckedCore const&)            =default;
      CheckedCore& operator= (CheckedCore&&)      =default;
      CheckedCore& operator= (CheckedCore const&) =default;
      
      using TAG_CheckedCore_Raw = COR;        ///< marker to allow unwrapping the raw core
      
      
      /* === state protocol API for IterStateWrapper === */
      bool
      checkPoint()  const
        {
          return _rawCore().checkPoint();
        }
      
      decltype(auto)
      yield()  const
        {
          __throw_if_empty();
          return _rawCore().yield();
        }
      
      void
      iterNext()
        {
          __throw_if_empty();
          _rawCore().iterNext();
        }
    };
  
  
  
  /**
   * Adapter to »piggy-back« a STL iterable container inline and expose it as »state core«.
   * @warning be sure to understand the implications of this setup
   *  - when initialised by reference, the container's contents will be copied
   *  - when move-initialised, the container will be destroyed with this iterator
   *  - the container API remains visible (baseclass), which could confuse trait detection
   */
  template<class CON>
  class ContainerCore
    : public CON
    {
      using Iter = typename CON::iterator;
      
      Iter p_;
      
    public:
      ContainerCore (CON&& container)
        : CON(std::forward<CON>(container))
        , p_{CON::begin()}
        { }
      
      // copy and assignment acceptable (warning!)
      
      
      /* === »state core« protocol API === */
      bool
      checkPoint()  const
        {
          return p_ != CON::end();
        }
      
      decltype(auto)
      yield()  const
        {
          return *p_;
        }
      
      void
      iterNext()
        {
          ++p_;
        }
    };
  
  
  
  
  /**
   * Decorator-Adapter to make a »*State Core*« iterable as Lumiera Forward Iterator.
   * This is a fundamental (and low-level) building block and works essentially the
   * same as IterStateWrapper — with the significant difference however that the
   * _Core is mixed in by inheritance_ and thus its full interface remains publicly
   * accessible. Another notable difference is that this adapter deliberately
   * *performs no sanity-checks*. This can be dangerous, but allows to use this
   * setup even in performance critical code.
   * @warning be sure to understand the consequences of _using_ ´core.yield()´ without
   *          checks; an iterator built this way _must be checked_ before each use, unless
   *          it is guaranteed to be valid (by contextual knowledge). It might be a good idea
   *          to build some safety checks into the Core API functions instead, maybe even just
   *          as assertions, or to wrap the Core into \ref CheckedCore for most usages.
   * @tparam T nominal result type (maybe const, but without reference).
   * @tparam COR type of the »state core«. The resulting iterator will _mix-in_ this type,
   *         and thus inherit properties like copy, move, compare, VTable, „POD-ness“.
   *         The COR must implement the following _iteration control API:_
   *          -# `checkPoint` establishes if the given state element represents a valid state
   *          -# ´iterNext` evolves this state by one step (sideeffect)
   *          -# `yield` realises the given state, exposing a result of type `T&`
   *         Furthermore, COR must be default-constructible in _disabled_ state
   * @note the resulting iterator will attempt to yield a reference to type \a T when possible;
   *         but when the wrapped `COR::yield()` produces a value, this is passed as such, moreover
   *         #operator-> becomes disabled then, to prevent taking the address of the (temporary) value!
   * @see IterExplorer a pipeline builder framework on top of IterableDecorator
   * @see iter-explorer-test.hpp
   * @see iter-adaptor-test.cpp
   */
  template<typename T, class COR>
  class IterableDecorator
    : public COR
    {
      COR &      _core()       { return static_cast<COR&>       (*this); }
      COR const& _core() const { return static_cast<COR const&> (*this); }
      
    protected:
      void
      __throw_if_empty()  const
        {
          if (not isValid())
            _throwIterExhausted();
        }
      
    public:
      using CoreYield  = iter::CoreYield<COR>;
      using _CommonT   = meta::CommonResultYield<T&, CoreYield>;
      using YieldRes   = typename _CommonT::ResType;
      using value_type = typename _CommonT::value_type;
      using reference  = typename _CommonT::reference;
      using pointer    = typename _CommonT::pointer;
      
      
      /** by default, pass anything down for initialisation of the core.
       * @note especially this allows move-initialisation from an existing core.
       * @remarks to prevent this rule from "eating" the standard copy operations,
       *          and the no-op default ctor, we need to declare them explicitly below.
       */
      template<typename...ARGS>
      IterableDecorator (ARGS&& ...init)
        : COR(std::forward<ARGS>(init)...)
        { }
      
      IterableDecorator()                                     =default;
      IterableDecorator (IterableDecorator&&)                 =default;
      IterableDecorator (IterableDecorator const&)            =default;
      IterableDecorator& operator= (IterableDecorator&&)      =default;
      IterableDecorator& operator= (IterableDecorator const&) =default;
      
      
      /* === lumiera forward iterator concept === */
      
      explicit operator bool() const { return isValid(); }
      
      YieldRes
      operator*() const
        {
          return _core().yield();          // core interface: yield
        }
      
      pointer
      operator->() const
        {
          if constexpr (_CommonT::isRef)
            return & _core().yield();      // core interface: yield
          else
            static_assert (_CommonT::isRef,
               "can not provide operator-> "
               "since iterator pipeline generates a value");
        }
      
      IterableDecorator&
      operator++()
        {
          _core().iterNext();              // core interface: iterNext
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return _core().checkPoint();     // core interface: checkPoint
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
      
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (IterableDecorator);
      
      
      /// Supporting equality comparisons of equivalent iterators (equivalent state core)...
      template<class T1, class T2>
      friend bool
      operator== (IterableDecorator<T1,COR> const& il, IterableDecorator<T2,COR> const& ir)
      {
        return (il.empty()   and ir.empty())
            or (il.isValid() and ir.isValid() and il._core() == ir._core());
      }
      
      template<class T1, class T2>
      friend bool
      operator!= (IterableDecorator<T1,COR> const& il, IterableDecorator<T2,COR> const& ir)
      {
        return not (il == ir);
      }
    };
  
  
  
  
  
  
  
  /**
   * Accessing a STL element range through a Lumiera forward iterator,
   * An instance of this iterator adapter is completely self-contained
   * and allows to iterate once over the range of elements, until
   * `pos==end`. Thus, a custom container may expose a range of
   * elements of an embedded STL container, without controlling
   * the details of the iteration (as is possible using the
   * more generic IterAdapter).
   * @note
   *  - when IT is just a pointer, we use the pointee as value type
   *  - but when IT is a class, we expect the usual STL style nested typedefs
   *    `value_type`, `reference` and `pointer`
   */
  template<class IT>
  class RangeIter
    {
      IT p_;
      IT e_;
      
      using _ValTrait = meta::ValueTypeBinding<meta::remove_pointer_t<IT>>;
      
    public:
      using pointer    = typename _ValTrait::pointer;
      using reference  = typename _ValTrait::reference;
      
      /// @note special twist, since a STL const_iterator would yield a non-const `value_type`
      using value_type = typename std::remove_reference<reference>::type;
      
      
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
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
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
          return not isValid();
        }
      
      
      /** access wrapped STL iterator */
      const IT&  getPos()  const { return p_; }
      const IT&  getEnd()  const { return e_; }
      
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (RangeIter);
      
      
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
  inline bool operator== (RangeIter<I1> const& il, RangeIter<I2> const& ir)  { return (!il && !ir) || (il.getPos() == ir.getPos()); }
  
  template<class I1, class I2>
  inline bool operator!= (RangeIter<I1> const& il, RangeIter<I2> const& ir)  { return !(il == ir); }
  
  
  
  
  
  /**
   * Enumerate all "numbers" within a range.
   * This allows to build pipelines based on all
   * numbers "for `i` from `1...N`". This range is _half open_,
   * i.e. the start is inclusive and the end point is exclusive.
   * @remark default constructed iters are empty and compare equal with
   *         any other exhausted NumIter; essential requirement for a
   *         Lumiera Forward Iterator (allows use in range-for-loops).
   * @tparam INT a number like type, which can be incremented and compared.
   * @note deliberately yields by-value and has no `operator->`
   */
  template<typename INT>
  class NumIter
    {
      INT i_;
      INT e_;
      
    public:
      typedef INT* pointer;
      typedef INT& reference;
      typedef INT  value_type;
      
      NumIter (INT start, INT end)
        : i_(start)
        , e_(end)
        { }
      
      template<typename X>
      NumIter (X&& start, X&& end)
        : i_(std::forward<X>(start))
        , e_(std::forward<X>(end))
        { }
      
      NumIter ()
        : i_()
        , e_()
        { }
      
      // standard copy operations acceptable
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
      
      /* === lumiera forward iterator concept === */
      
      value_type
      operator*() const
        {
          _maybe_throw();
          return i_;
        }
      
      NumIter&
      operator++()
        {
          _maybe_throw();
          ++i_;
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return i_ < e_;     // NOTE: use comparison to detect iteration end
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
      
      /** access wrapped index elements */
      INT&  getPos()  const { return i_; }
      INT&  getEnd()  const { return e_; }
      
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (NumIter);
      
      
      /// Supporting equality comparisons...
      bool operator!= (NumIter const& o) const  { return not operator==(o); }
      bool operator== (NumIter const& o) const  { return (empty() and o.empty())    // empty iters must be equal (Lumiera iter requirement)
                                                      or (i_ == o.i_ and e_ == o.e_); }
      
    private:
      void
      _maybe_throw()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
    };
  
  
  
  
  
  
  /** convenience function to iterate "each number" */
  template<typename INT>
  inline NumIter<INT>
  eachNum (INT start = std::numeric_limits<INT>::min()
          ,INT end   = std::numeric_limits<INT>::max())
  {
    return NumIter<INT> (start, end);
  }
  
  
  
  
  
  
  
  /**
   * Helper for type rewritings:
   * get the element type for an iterator like entity
   */
  template<class TY>
  struct IterType;
  
  template<template<class,class> class Iter, class TY, class CON>
  struct IterType<Iter<TY,CON>>
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
  struct IterType<RangeIter<IT>>
    : IterType<IT>
    {
      template<class T2>
      struct SimilarIter  ///< rebind to rewritten Iterator wrapped into RangeIter
        {
          typedef typename IterType<IT>::template SimilarIter<T2>::Type WrappedIter;
          typedef RangeIter<WrappedIter> Type;
        };
    };
  
  
  
  /** wrapper to expose values as const */
  template<class IT>
  class ConstIter
    {
      IT i_;  ///< nested source iterator
      
      
    public:
      typedef const typename IT::value_type value_type;
      typedef const typename IT::pointer    pointer;
      typedef const typename IT::reference  reference;
      
      ConstIter (IT srcIter)
        : i_(srcIter)
        { }
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      
      
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
          return not isValid();
        }
      
      
      /** access the wrapped implementation iterator */
      IT const&
      getBase()  const
        {
          return i_;
        }
      
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (ConstIter);
    };
  
  
  /// Supporting equality comparisons...
  template<class I1, class I2>
  inline bool operator== (ConstIter<I1> const& il, ConstIter<I2> const& ir)  { return il.getBase() == ir.getBase(); }
    
  template<class I1, class I2>
  inline bool operator!= (ConstIter<I1> const& il, ConstIter<I2> const& ir)  { return not (il == ir); }
  
  
  
}// namespace lib
#endif /*LIB_ITER_ADAPTER_H*/
