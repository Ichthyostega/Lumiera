/*
  ITERTOOLS.hpp  -  collection of tools for building and combining iterators

   Copyright (C)
     2009,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file itertools.hpp
 ** Collection of tools for adapting and combining Lumiera iterators.
 ** These abstract from the underlying data container and provide the
 ** exposed data as a source to pull values from. Based on this model
 ** of on-demand processing, pipelines can be built with data filtering
 ** and transforming defined by functions. The templates and builders in
 ** his header support this approach, based on the **Lumiera Forward Iterator**
 ** concept. The implementation relies techniques of _generic programming_,
 ** so that any kind of _invocable_ can be used as processing function and
 ** actual type information is used at compile time to generate an optimised
 ** calculation scheme. The tools provided in this header are fairly basic
 ** however, and will typically be used to apply simple adaptations to be
 ** used inline.
 ** @remark A similar technique for building much more elaborate processing,
 **         search and evaluation schemes is provided by iter-explorer.hpp
 ** 
 ** # filtering Iterator
 ** The iter::Filter template can be used to build a filter into a pipeline,
 ** as it forwards only those elements from its source iterator, which pass
 ** the predicate evaluation. Anything invocable that yields a bool convertible
 ** result can be passed in as predicate. Please note, that — depending on the
 ** predicate — already the ctor or even a simple `bool` test might pull and
 ** exhaust the source iterator completely, in an attempt to find the
 ** first element to pass the predicate test.
 ** 
 ** # processing Iterator
 ** The iter::Trans template can be used as processing (or transforming)
 ** step within the pipeline. It is created with a functor, which, when
 ** pulling elements, is invoked for each element pulled from the
 ** source iterator. The functor need to accept the result type from
 ** the underlying source iterator and its result value determines
 ** the result (output) type of the resulting pipeline.
 ** 
 ** # convenience utilities
 ** In addition, this header provides an assortment of convenience helpers
 ** and utilities, that were created over time to simplify the formulation
 ** of common tasks, while the fundamental building blocks for definition and
 ** combination of Lumiera iterators can be found in \ref iter-adapter.hpp.
 ** @warning since this header is widely used in the Lumiera codebase,
 **   excessive complexity and rarely used includes should be avoided.
 ** 
 ** @see itertools-test.cpp
 ** @see iter-explorer.hpp
 ** @see iter-adapter.hpp
 ** @see gen-node.hpp
 */


#ifndef LIB_ITERTOOLS_H
#define LIB_ITERTOOLS_H


#include "lib/iter-adapter.hpp"
#include "lib/item-wrapper.hpp"
#include "lib/meta/trait.hpp"
#include "lib/util.hpp"

#include <functional>
#include <concepts>
#include <utility>


namespace lib {
  
  using std::move;
  using std::forward;
  using util::unConst;
  
  namespace iter { // Implementation building blocks
    using lib::meta::ValueTypeBinding;
    
    /**
     * Predicate to be applied to an iterator or container element.
     * Requires a non-strict function (side-effects acceptable)
     * that operates on the value yielded by the iterator.
     */
    template<typename FUN, typename IT, typename REF = ValueTypeBinding<IT>::reference>
    concept iter_predicate = requires (FUN fun, REF ref)
      {
        requires std::invocable<FUN,REF>;
        { fun(ref) } -> std::convertible_to<bool>;
      };
    
    template<typename FUN, typename IT, typename REF = ValueTypeBinding<IT>::reference>
    concept iter_transformer = std::invocable<FUN,REF>;
    
    
    
    /**
     * Pseudo-Iterator to yield just a single value.
     * When incremented, the value is destroyed and
     * the Iterator transitions to _exhausted state_.
     * @remark can be useful to integrate special cases
     *   into an established interface based on iterators.
     * @tparam VAL anything, value or reference to store
     * @note relies on ItemWrapper, which includes a flag.
     */
    template<class VAL>
    class Single
      {
        using Item = wrapper::ItemWrapper<VAL>;
        
        Item theValue_;
        
        void
        __throw_if_empty()  const
          {
            if (not isValid())
              _throwIterExhausted();
          }
        
      public:
        using value_type = meta::RefTraits<VAL>::value_type;
        using reference  = meta::RefTraits<VAL>::reference;
        using pointer    = meta::RefTraits<VAL>::pointer;
        
        
        template<typename X,     typename =meta::disable_if_self<Single, X>>
        Single (X&& val)
          : theValue_{forward<X>(val)}
          { }
        
        Single()  =default;
        // standard copy operations
        
        
        /* === Lumiera Forward Iterator concept === */
        
        operator bool()  const
          {
            return isValid();
          }
        
        reference
        operator*() const
          {
            __throw_if_empty();
            return *theValue_;
          }
        
        pointer
        operator->() const
          {
            return & operator*();
          }
        
        Single&
        operator++()
          {
            __throw_if_empty();
            theValue_.reset();
            return *this;
          }
        
        bool
        isValid()  const
          {
            return theValue_.isValid();
          }
        
        bool
        empty()    const
          {
            return not isValid();
          }
        
        ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (Single);
        
        friend bool
        operator== (Single const& s1, Single const& s2)
        {
          return s1.theValue_ == s2.theValue_;
        }
      };
    
    
    
    /**
     * Iterator adapter that applies filtering to a source iterator.
     * Any invocable that can be used on the iterator's result to
     * yield something bool-convertable can be employed here, and
     * will be embedded as-is (which might be by reference).
     * The source iterator will be pulled immediately on construction,
     * until the first result is found that satisfies the filter predicate.
     * @note the filter is thus invoked _at most once_ per value yielded.
     * @warning this eager pull can be insidious when the predicate happens
     *   to store a reference to the filtered value, and the adapter object
     *   is then moved to another location after construction.
     */
    template<class SRC, typename PRED>
    class Filter
      {
        SRC srcIter_;
        PRED predicate_;
        
        void
        __throw_if_empty()  const
          {
            if (not isValid())
              _throwIterExhausted();
          }
        
        /** @note establishes the **invariant**:
         *        whatever the source yields as current element,
         *        has already been approved by our predicate */
        void
        pullFilter()
          {
            while (srcIter_ and not predicate_(*srcIter_))
              ++srcIter_;
          }
        
      public:
        using value_type = ValueTypeBinding<SRC>::value_type;
        using reference  = ValueTypeBinding<SRC>::reference;
        using pointer    = ValueTypeBinding<SRC>::pointer;
        
        
        template<class IT, iter_predicate<IT> FUN>
        Filter (IT&& src, FUN&& filter)
          : srcIter_{forward<IT> (src)}
          , predicate_{forward<FUN> (filter)}
          {
            pullFilter(); // initially pull to establish the invariant
          }
        
        Filter()  =default;
        // standard copy operations
        
        
        /* === Lumiera Forward Iterator concept === */
        
        operator bool()  const
          {
            return isValid();
          }
        
        reference
        operator*() const
          {
            __throw_if_empty();
            return *srcIter_;
          }
        
        pointer
        operator->() const
          {
            return & operator*();
          }
        
        Filter&
        operator++()
          {
            __throw_if_empty();
            ++srcIter_;
            pullFilter();
            return *this;
          }
        
        bool
        isValid()  const
          {
            return bool(srcIter_);
          }
        
        bool
        empty()    const
          {
            return not isValid();
          }
        
        ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (Filter);
        
        friend bool
        operator== (Filter const& s1, Filter const& s2)
        {
          return s1.srcIter_ == s2.srcIter_;
        }
      };
    
    
    /**
     * Iterator adapter to map a function onto each result
     * of the given source iterator. The result of this function
     * invocation is captured into an ItemWrapper storage buffer.
     * Consecutive value access will thus use this cached result.
     */
    template<class SRC, typename PROC>
    class Trans
      {
        using Res = std::invoke_result_t<PROC, Yield<SRC>>;
        using Item = wrapper::ItemWrapper<Res>;
        
        SRC srcIter_;
        PROC transformer_;
        Item transformed_;
        
        void
        __throw_if_empty()  const
          {
            if (not isValid())
              _throwIterExhausted();
          }
        
        decltype(auto)
        process()
          {
            if (not transformed_) // invoke transform function once per src item
              transformed_ = transformer_(*srcIter_);
            return *transformed_;
          }
        
      public:
        using value_type = ValueTypeBinding<Res>::value_type;
        using reference  = ValueTypeBinding<Res>::reference;
        using pointer    = ValueTypeBinding<Res>::pointer;
        
        
        template<class IT, iter_transformer<IT> FUN>
        Trans (IT&& src, FUN&& processor)
          : srcIter_{forward<IT> (src)}
          , transformer_{forward<FUN> (processor)}
          , transformed_{/* initially empty */}
          { }
        
        Trans()   = default;
        // standard copy operations
        
        
        /* === Lumiera Forward Iterator concept === */
        
        operator bool()  const
          {
            return isValid();
          }
        
        reference
        operator*() const
          {
            __throw_if_empty();
            return unConst(this)->process();
          }
        
        pointer
        operator->() const
          {
            return & operator*();
          }
        
        Trans&
        operator++()
          {
            __throw_if_empty();
            ++srcIter_;
            transformed_.reset();
            return *this;
          }
        
        bool
        isValid()  const
          {
            return bool(srcIter_);
          }
        
        bool
        empty()    const
          {
            return not isValid();
          }
        
        ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (Trans);
        
        friend bool
        operator== (Trans const& s1, Trans const& s2)
        {
          return s1.srcIter_ == s2.srcIter_
             and (not s1 or not s2
                  or *s1 == *s2);
        }      // even same type might hold different function
      };
    
    
    /**
     * Iterator adapter to yield a value,
     * initialised from the result of an underlying iterator.
     * @tparam SRC a Lumiera Forward Iterator to wrap and decorate
     * @tparam forceMove if the value shall be initialised by a forced `std::move`;
     *                   otherwise it will be initialised by `std::forward`
     * @warning using this adapter may lead to _consuming_ of results; beware!
     */
    template<class SRC, bool forceMove =false>
    class ValueAdapter
      : public SRC
      {
        static_assert (can_IterForEach<SRC>::value, "Lumiera Iterator required as source.");
        
      public:
        SRC::value_type
        operator*() const
          {
            using Val = SRC::value_type;
            if constexpr (forceMove)
                return {move (SRC::operator*())};
            else
                return {forward<Val> (SRC::operator*())};
          }
        
        SRC::pointer operator->() const =delete;
        
        using SRC::SRC;
      };
    
    
    /**
     * Helper: predicate returning `true`
     * whenever the argument value changes
     * during a sequence of invocations.
     */
    template<typename VAL>
    class ChangeDetector
      {
        using Item = wrapper::ItemWrapper<VAL>;
        
        Item prev_;
        
      public:
        bool
        operator() (VAL const& elm)
          {
             if (prev_ and
                (*prev_ == elm))
               return false;
             
             // element differs from predecessor
             prev_ = elm;
             return true;
          }
      };
    
  }// namespace iter (building blocks)
  
  
  
  
  
  /**
   * Build a filtering iterator that wraps a source iterator.
   * Only elements classified as `true` will appear as results.
   * @param filterPredicate to be invoked for each source element
   * @note Both the source iterator and the predicate are _embodied_
   *       into the generated filter-adapter object, yet both can be
   *       initialised by RValue. Furthermore this implies that both
   *       types should be default constructible and assignable, to
   *       support the typical usage pattern of an iterator.
   */
  template<class IT, typename FUN>
  inline auto
  filterIter (IT&& src, FUN&& filterPredicate)
  {
    using SrcIT  = std::decay_t<IT>;
    using Predi  = std::decay_t<FUN>;
    return iter::Filter<SrcIT,Predi>{forward<IT> (src)
                                    ,forward<FUN>(filterPredicate)};
  }
  
  
  /**
   * Build filtering adapter that removes
   * consecutive identical values emitted by source iterator
   */
  template<class IT>
  inline auto
  filterRepeat (IT&& source)
  {
    using Val = meta::ValueTypeBinding<IT>::value_type;
    return filterIter (forward<IT> (source)
                      ,iter::ChangeDetector<Val>{} );
  }
  
  
  /**
   * A »singleton« iterator that yields a single value and is empty after iteration.
   * @warning Be sure to understand that we literally pick up and wrap anything
   *          provided as argument. If you pass a reference, we wrap a reference.
   *          If you want to wrap a copy, you have to create a copy value argument.
   */
  template<class VAL>
  inline auto
  singletonIter (VAL&& something)
  {
    return iter::Single<VAL>{forward<VAL> (something)};
  }
  
  /** not-anything-at-all iterator */
  template<class VAL>
  inline auto
  nilIter()
  {
    return iter::Single<VAL>();
  }
  
  
  /** 
   * Build a transforming iterator that maps a function to the source.
   * @tparam processingFun to be invoked for each source element
   * @note both the source iterator and the transforming functor
   *       are embodied, together with a result cache buffer.
   */
  template<class IT, typename FUN>
  inline auto
  transformIter (IT&& src, FUN&& processingFun)
  {
    using SrcIT  = std::decay_t<IT>;
    using ProcF  = std::decay_t<FUN>;
    return iter::Trans<SrcIT,ProcF>{forward<IT> (src)
                                   ,forward<FUN>(processingFun)};
  }
  
  
  
  /**
   * Build an adapter that wraps the given iterator
   * and returns by-value, taking a copy from each result.
   */
  template<class IT>
  auto
  copyIt (IT&& iterator)
  {
    using Iter = meta::RefTraits<IT>::value_type;
    using CopyAdapter = iter::ValueAdapter<Iter>;
    return CopyAdapter{std::forward<IT> (iterator)};
  }
  
  /**
   * Build an adapter that wraps the given iterator
   * and returns by-value, thereby **forcibly moving** the result.
   * @warning the resulting behaviour breaks the usual assumptions
   *          regarding iterators, since the first access **consumes** the result.
   *          Use with extreme care and be sure to understand all ramifications;
   *          the forced move might even disrupt the underlying source.
   */
  template<class IT>
  auto
  moveIt (IT&& iterator)
  {
    using Iter = meta::RefTraits<IT>::value_type;
    using ForceMoveAdapter = iter::ValueAdapter<Iter,true>;
    return ForceMoveAdapter{std::forward<IT> (iterator)};
  }
  
  
  
  
  
  /* === utility functions === */
  
  template<class IT, class CON>
  inline void
  append_all (IT iter, CON& container)
  {
    for ( ; iter; ++iter )
      container.push_back (*iter);
  }
  
  template<class IT>
  inline meta::ValueTypeBinding<IT>::value_type
  pull_last (IT iter)
    {
      using Val = meta::ValueTypeBinding<IT>::value_type;
      using Item = wrapper::ItemWrapper<Val>;
      
      Item lastElm;
      
      while (iter)
        {
          lastElm = *iter;
          ++iter;
        }
      
      if (lastElm)
        return *lastElm;
      else
        throw lumiera::error::State ("attempt to retrieve the last element "
                                     "of an exhausted or empty iterator"
                                    ,lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
    }
  
  
} // namespace lib
#endif
