/*
  ITER-ADAPTER-STL.hpp  -  helpers for building simple forward iterators

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file iter-adapter-stl.hpp
 ** Preconfigured adapters for some STL container standard usage situations.
 ** Especially, definitions for accessing views on common STL containers
 ** repackaged as <b>lumiera forward iterators</b>. Mostly the purpose
 ** is ease of use, we don't create an abstraction barrier or try to
 ** hide implementation details. (see iter-source.hpp for such an
 ** abstraction facility). As a benefit, these adapters can be
 ** considered low overhead.
 ** 
 ** @see iter-adapter-stl-test.cpp
 ** @see iter-adapter.hpp
 ** @see iter-source.happ
 ** @see intertools.hpp
 ** 
 */


#ifndef LIB_ITER_ADAPTER_STL_H
#define LIB_ITER_ADAPTER_STL_H


#include "lib/iter-adapter.hpp"
#include "lib/iter-adapter-ptr-deref.hpp"

#include <vector>


namespace lib {
namespace iter_stl {
  
  /**
   * Helper to filter repeated values
   * from a wrapped iterator (both STL or Lumiera)
   */
  template<typename IT>
  class DistinctIter
    {
    public:
      using value_type = IT::value_type;
      using reference  = IT::reference;
      using pointer    = IT::pointer;
      
    private:
      IT      i_;
      pointer prev_;
      
      void memorise() { if (i_) prev_ = &(*i_); }
      
    public:
      DistinctIter()            : i_(), prev_() { }
      DistinctIter(IT const& i) : i_(i),prev_() { memorise(); }
      
      pointer   operator->()  const { return i_;  }
      reference operator*()   const { return *i_; }
      bool      isValid()     const { return i_;  }
      
      explicit
      operator bool() const
        {
          return bool{i_};
        }
      
      
      DistinctIter&
      operator++()
        {
          do ++i_;
          while (i_ and prev_ and *prev_ == *i_ );
          memorise();
          return *this;
        }
      
      friend bool operator== (DistinctIter const& i1, DistinctIter const& i2) { return i1.i_ == i2.i_; }
      friend bool operator!= (DistinctIter const& i1, DistinctIter const& i2) { return i1.i_ != i2.i_; }
    };
  
  
  
  /**
   * @internal helper baseclass to simplify defining customised wrapped STL iterators.
   * @remark this is meant to be compliant to a STL input-iterator;
   *    it's _not_ a »Lumiera Forward Iterator« (lacks the bool check for iteration end)
   * @warning minimal definition, since it is only intended to be wrapped into lib::RangeIter
   */
  template<typename DEF>
  struct WrappedStlIter : DEF
    {
      using Iter       = DEF::Iter;
      using value_type = DEF::value_type;
      using reference  = DEF::reference;
      using pointer    = DEF::pointer;
      
      
      WrappedStlIter()              : i_()  { }
      WrappedStlIter(Iter const& i) : i_(i) { }
      
      pointer   operator->() const  { return DEF::get(i_);  }
      reference operator*()  const  { return *(DEF::get(i_)); }
      
      WrappedStlIter& operator++()  { ++i_; return *this; }
      
      friend bool operator== (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ == i2.i_; }
      friend bool operator!= (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ != i2.i_; }
      
      /** @note the following dummy definitions are required to fulfil the concept std::input_iterator */ 
      WrappedStlIter& operator++(int) { NOTREACHED("never used as STL iterator"); return *this; }
      using difference_type = ptrdiff_t;
      
    private:
      mutable Iter i_;
    };
  
  
  /* -- customisations for building concrete wrappers -- */
  
  /**
   * Wrapped-Iter-Policy: forwarding directly
   * with typing retained unaltered.
   */
  template<typename IT>
  struct Wrapped_Identity
    {
      using Iter = IT;
      using value_type = IT::value_type;
      using reference  = IT::reference;
      using pointer    = IT::pointer;
      
      static Iter get (Iter& it) { return & (*it); }
    };
  
  /**
   * Wrapped-Iter-Policy: picking the key part
   * of a pair iterator (map or multimap).
   */
  template<typename IT>
  struct Wrapped_PickKey
    {
      using Iter = IT;
      using value_type = IT::value_type::first_type;
      using reference  = value_type &;
      using pointer    = value_type *;
      
      static pointer get (Iter& it) { return & (it->first); }
    };
  
  /**
   * Wrapped-Iter-Policy: picking the value part
   * of a pair iterator (map or multimap).
   */
  template<typename IT>
  struct Wrapped_PickVal
    {
      using Iter = IT;
      using value_type = IT::value_type::second_type;
      using reference  = value_type &;
      using pointer    = value_type *;
      
      static pointer get (Iter& it) { return & (it->second); }
    };
  
  template<typename IT>
  struct Wrapped_PickConstVal
    {
      using Iter = IT;
      using value_type = const IT::value_type::second_type;
      using reference  = const value_type &;
      using pointer    = const value_type *;
      
      static pointer get (Iter& it) { return & (it->second); }
    };
  
  
  
  
  
  
  namespace { // traits and helpers...
    
    template<class MAP>
    struct _MapTypeSelector
      {
        using Key = MAP::value_type::first_type;
        using Val = MAP::value_type::second_type;
        using Itr = MAP::iterator;
      };
    
    template<class MAP>
    struct _MapTypeSelector<const MAP>
      {
        using Key = MAP::value_type::first_type;
        using Val = MAP::value_type::second_type const;
        using Itr = MAP::const_iterator;
      };
    
    /** helper to access the parts of the pair values correctly...*/
    template<class IT, typename SEL>
    struct _MapSubSelector
      {
        using PickKey = WrappedStlIter< Wrapped_PickKey<IT>>;
        using PickVal = WrappedStlIter< Wrapped_PickVal<IT>>;
      };
    
    /** especially for const iterators we need to use \c const& and \c const* */
    template<class IT, typename SEL>
    struct _MapSubSelector<IT, SEL const&>
      {
        using PickKey = WrappedStlIter< Wrapped_PickKey<IT>>;    // Key is always const for maps
        using PickVal = WrappedStlIter< Wrapped_PickConstVal<IT>>;
      };
    
    
    
    template<class MAP>
    struct _MapT
      {
        using KeyType   = _MapTypeSelector<MAP>::Key;
        using ValType   = _MapTypeSelector<MAP>::Val;
        using EntryIter = _MapTypeSelector<MAP>::Itr;
        
        using DetectConst =  EntryIter::reference;
        using PickKeyIter = _MapSubSelector<EntryIter,DetectConst>::PickKey;
        using PickValIter = _MapSubSelector<EntryIter,DetectConst>::PickVal;
        
        using KeyIter     = RangeIter<PickKeyIter>;
        using ValIter     = RangeIter<PickValIter>;
        
        using DistinctKeys = DistinctIter<KeyIter>;
      };
    
    
    template<class IT>
    struct _MapIterT
      {
        using EntryIter = IT;
        
        using KeyType = EntryIter::value_type::first_type;
        using ValType = EntryIter::value_type::second_type;
        
        using DetectConst =  EntryIter::reference;
        using PickKeyIter = _MapSubSelector<EntryIter,DetectConst>::PickKey;
        using PickValIter = _MapSubSelector<EntryIter,DetectConst>::PickVal;
          
        using KeyIter     = RangeIter<PickKeyIter>;
        using ValIter     = RangeIter<PickValIter>;
        
        using DistinctKeys = DistinctIter<KeyIter>;
      };
    
    
    
    template<class SEQ>
    struct _SeqT
      {
        using Iter  = SEQ::iterator;
        using Range = RangeIter<Iter>;
        using DistinctVals = DistinctIter<Range>;
        using Addrs = AddressExposingIter<Range>;
      };
    
    template<class SEQ>
    struct _SeqT<const SEQ>
      {
        using Iter  = SEQ::const_iterator;
        using Range = RangeIter<Iter>;
        using DistinctVals = DistinctIter<Range>;
        using Addrs = AddressExposingIter<Range>;
      };
    
  }//(End) traits/helpers
  
  
  
  
  /** @return Lumiera Forward Iterator
   *           to yield each Element from a STL container
   */
  template<class CON>
  inline _SeqT<CON>::Range
  eachElm (CON& coll)
  {
    using Range = _SeqT<CON>::Range;
    return Range (coll.begin(), coll.end());
  }
  
  
  /** @return Lumiera Forward Iterator
   *          exposing the address of each Element within a STL
   */
  template<class CON>
  inline _SeqT<CON>::Addrs
  eachAddress (CON& coll)
  {
    using Addresses = _SeqT<CON>::Addrs;
    return Addresses (eachElm (coll));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          each key of a map/multimap
   */
  template<class MAP>
  inline _MapT<MAP>::KeyIter
  eachKey (MAP& map)
  {
    using Range   = _MapT<MAP>::KeyIter;
    using PickKey = _MapT<MAP>::PickKeyIter;
    
    return Range (PickKey (map.begin()), PickKey (map.end()));
  }
  
  
  /** @return Lumiera Forward Iterator extracting the keys
   *          from a given range of (key,value) pairs
   */
  template<class IT>
  inline _MapIterT<IT>::KeyIter
  eachKey (IT const& begin, IT const& end)
  {
    using Range   = _MapIterT<IT>::KeyIter;
    using PickKey = _MapIterT<IT>::PickKeyIter;
    
    return Range (PickKey (begin), PickKey (end));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          each value within a map/multimap
   */
  template<class MAP>
  inline _MapT<MAP>::ValIter
  eachVal (MAP& map)
  {
    using Range   = _MapT<MAP>::ValIter;
    using PickVal = _MapT<MAP>::PickValIter;
    
    return Range (PickVal (map.begin()), PickVal (map.end()));
  }
  
  
  /** @return Lumiera Forward Iterator extracting the values
   *          from a given range of (key,value) pairs
   */
  template<class IT>
  inline _MapIterT<IT>::ValIter
  eachVal (IT const& begin, IT const& end)
  {
    using Range   = _MapIterT<IT>::ValIter;
    using PickVal = _MapIterT<IT>::PickValIter;
    
    return Range (PickVal (begin), PickVal (end));
  }
  
  
  /** build a Lumiera Forward Iterator to suppress
   *  any repetitions in the given sequence.
   */
  template<class SEQ>
  inline _SeqT<SEQ>::DistinctVals
  eachDistinct (SEQ& seq)
  {
    using Range          = _SeqT<SEQ>::Range;
    using DistinctValues = _SeqT<SEQ>::DistinctVals;
    
    return DistinctValues (Range (seq.begin(), seq.end()));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          the distinct keys from a multimap
   *  @warning full scan of all keys, dropping repetitions
   */
  template<class MAP>
  inline _MapT<MAP>::DistinctKeys
  eachDistinctKey (MAP& map)
  {
    return typename _MapT<MAP>::DistinctKeys (eachKey (map));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          the distinct keys from a multimap
   *  @warning full scan of all keys, dropping repetitions
   */
  template<class MMAP, typename KEY>
  inline _MapT<MMAP>::ValIter
  eachValForKey (MMAP& multimap, KEY key)
  {
    using Pos     = _MapT<MMAP>::EntryIter;
    using Range   = _MapT<MMAP>::ValIter;
    using PickVal = _MapT<MMAP>::PickValIter;
    
    std::pair<Pos,Pos> valRange = multimap.equal_range (key);
    
    return Range (PickVal{valRange.first}, PickVal{valRange.second});
  }
  
  
  
  
  /**
   * materialised iterator contents.
   * At construction, the given source iterator
   * is immediately discharged into an internal buffer (vector).
   * This captured value sequence can then be retrieved _once_ as
   * Lumiera Forward Iterator
   */
  template<typename VAL>
  class IterSnapshot
    {
      using Sequence = std::vector<VAL>;
      
      mutable
      Sequence buffer_;
      size_t   pos_ = 0;
      
      
    public:
      /** create empty snapshot */
      IterSnapshot() { }
      
      /** take snapshot by discharging the given Lumiera Forward iterator
       *  @warning depending on the implementation backing the source iterator,
       *           this might or might not yield side-effects.
       */
      template<class IT>
      IterSnapshot (IT&& src)
        {
          for ( ; src; ++src)
            buffer_.emplace_back (*src);
        }
      
      /** build snapshot from a copy of the Lumiera Iterator
       *  @warning depending on the implementation backing the source iterator,
       *           this might or might not yield side-effects.
       */
      template<class IT>
      IterSnapshot (IT const& src)
        {
          for (IT copy{src}; copy; ++copy)
            buffer_.emplace_back (*copy);
        }
      
      /** take snapshot by consuming a STL iterator */
      template<class IT>
      IterSnapshot (IT&& pos, IT const& end)
        {
          for ( ; pos!=end; ++pos)
            buffer_.emplace_back (*pos);
        }
      
      /** take snapshot from STL iterator */
      template<class IT>
      IterSnapshot (IT const& begin, IT const& end)
        {
          for (IT pos{begin}; pos!=end; ++pos)
            buffer_.emplace_back (*pos);
        }
      
      IterSnapshot(IterSnapshot &&)                 = default;
      IterSnapshot(IterSnapshot const&)             = default;
      IterSnapshot& operator= (IterSnapshot const&) = default;
      IterSnapshot& operator= (IterSnapshot &&)     = default;
      
      explicit
      operator bool() const
        {
          return isValid();
        }
      
      size_t
      size()  const
        {
          return buffer_.size();
        }
      
      
      
      /* === lumiera forward iterator concept === */
      
      using pointer    = VAL*;
      using reference  = VAL&;
      using value_type = VAL ;
      
      reference
      operator*() const
        {
          _maybe_throw();
          return buffer_[pos_];
        }
      
      pointer
      operator->() const
        {
          _maybe_throw();
          return &buffer_[pos_];
        }
      
      IterSnapshot&
      operator++()
        {
          _maybe_throw();
          ++pos_;
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return pos_ < buffer_.size();
        }
      
      bool
      empty ()  const
        {
          return not isValid();
        }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (IterSnapshot)

      
      /** equality is based first on the _valid state_ (to support `pos != end`)
       *  and then on the actual position and contents of the snapshots */
      friend bool
      operator== (IterSnapshot const& s1, IterSnapshot const& s2)
      {
        return (s1.empty()   and  s2.empty())
            or (s1.isValid() and  s2.isValid()
                and s1.pos_    == s2.pos_
                and s1.buffer_ == s2.buffer_
               );
      }
      
      friend bool
      operator!= (IterSnapshot const& snap1, IterSnapshot const& snap2)
      {
        return not (snap1 == snap2);
      }
      
      
    private:
      void
      _maybe_throw()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
    };
  
  namespace {
    template<class CON>
    using ContentSnapshot = IterSnapshot<typename CON::value_type>;
  }
  
  
  
  /** Take a snapshot of the given STL compliant container
   * @return Lumiera Forward Iterator to yield each Element from this snapshot
   * @note the snapshot is stored within a vector, i.e. heap allocated.
   * @warning copying the returned iterator object copies the snapshot vector
   */
  template<class CON>
  inline ContentSnapshot<CON>
  snapshot(CON const& con)
  {
    return ContentSnapshot<CON>{begin(con), end(con)};
  }

  /** Take a snapshot of the given LumieraIterator, which is thereby consumed
   * @return Lumiera Forward Iterator to yield each Element from this snapshot
   */
  template<class IT>
  inline ContentSnapshot<IT>
  dischargeToSnapshot(IT& ii)
  {
    return ContentSnapshot<IT>{ii};
  }
  
  /** Take a snapshot of the given \c std::initializer_list
   * @return Lumiera Forward Iterator to yield each Element from this snapshot
   * @remarks this can be a easy workaround for passing on a sequence of literal
   *          values defined inline in a brace expression; the typical implementation
   *          of brace initialiser lists allocates a temporary array on the stack.
   *          By using this helper, we copy the elements from this local array
   *          into a vector on the heap. Of course this isn't efficient,
   *          but it's convenient, e.g. for testing.
   */
  template<class VAL>
  inline iter_stl::IterSnapshot<VAL>
  snapshot(std::initializer_list<VAL> const&& ili)
  {
    using OnceIter = iter_stl::IterSnapshot<VAL>;
    return OnceIter(begin(ili), end(ili));
  }
  
  
}} // namespace lib::iter_stl
#endif
