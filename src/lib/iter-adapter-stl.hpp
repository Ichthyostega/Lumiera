/*
  ITER-ADAPTER-STL.hpp  -  helpers for building simple forward iterators 

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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
      typedef typename IT::value_type value_type;
      typedef typename IT::reference reference;
      typedef typename IT::pointer  pointer;
      
    private:
      IT      i_;
      pointer prev_;
      
      void memorise() { if (i_) prev_ = &(*i_); }
      
    public:
      DistinctIter()            : i_(), prev_() { }
      DistinctIter(IT const& i) : i_(i),prev_() { memorise(); }
      
      pointer   operator->() const { return i_; }
      reference operator*()  const { return *i_;}
      bool      isValid()    const { return i_; }
      
      operator bool()        const { return i_; }
      
      
      DistinctIter&
      operator++()
        {
          do ++i_;
          while (i_ && prev_ && *prev_ == *i_ );
          memorise();
          return *this; 
        }
      
      friend bool operator== (DistinctIter const& i1, DistinctIter const& i2) { return i1.i_ == i2.i_; }
      friend bool operator!= (DistinctIter const& i1, DistinctIter const& i2) { return i1.i_ != i2.i_; }
    };
  
  
  
  /**
   * helper baseclass to simplify
   * defining customised wrapped STL iterators
   */
  template<typename DEF>
  struct WrappedStlIter : DEF
    {
      typedef typename DEF::Iter Iter;
      typedef typename DEF::reference reference;
      typedef typename DEF::pointer  pointer;
      
      
      WrappedStlIter()              : i_()  { }
      WrappedStlIter(Iter const& i) : i_(i) { }
      
      pointer   operator->() const  { return DEF::get(i_);  }
      reference operator*()  const  { return *(DEF::get(i_)); }
      
      WrappedStlIter& operator++()  { ++i_; return *this; }
      
      friend bool operator== (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ == i2.i_; }
      friend bool operator!= (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ != i2.i_; }
      
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
      typedef IT Iter;
      typedef typename IT::value_type value_type;
      typedef typename IT::reference reference;
      typedef typename IT::pointer  pointer;
      
      static Iter get (Iter& it) { return & (*it); }
    };
  
  /**
   * Wrapped-Iter-Policy: picking the key part
   * of a pair iterator (map or multimap).
   */
  template<typename IT>
  struct Wrapped_PickKey
    {
      typedef IT Iter;
      typedef typename IT::value_type::first_type value_type;
      typedef value_type & reference;
      typedef value_type * pointer;
      
      static pointer get (Iter& it) { return & (it->first); }
    };
  
  /**
   * Wrapped-Iter-Policy: picking the value part
   * of a pair iterator (map or multimap).
   */
  template<typename IT>
  struct Wrapped_PickVal
    {
      typedef IT Iter;
      typedef typename IT::value_type::second_type value_type;
      typedef value_type & reference;
      typedef value_type * pointer;
      
      static pointer get (Iter& it) { return & (it->second); }
    };
  
  template<typename IT>
  struct Wrapped_PickConstVal
    {
      typedef IT Iter;
      typedef typename IT::value_type::second_type value_type;
      typedef value_type const& reference;
      typedef value_type const* pointer;
      
      static pointer get (Iter& it) { return & (it->second); }
    };
  
  
  
  
  
  
  namespace { // traits and helpers...
    
    template<class MAP>
    struct _MapTypeSelector
      {
        typedef typename MAP::value_type::first_type  Key;
        typedef typename MAP::value_type::second_type Val;
        typedef typename MAP::iterator                Itr;
      };
    
    template<class MAP>
    struct _MapTypeSelector<const MAP>
      {
        typedef typename MAP::value_type::first_type        Key;
        typedef typename MAP::value_type::second_type const Val;
        typedef typename MAP::const_iterator                Itr;
      };
    
    /** helper to access the parts of the pair values correctly...*/
    template<class IT, typename SEL>
    struct _MapSubSelector
      {
        typedef WrappedStlIter< Wrapped_PickKey<IT>> PickKey;
        typedef WrappedStlIter< Wrapped_PickVal<IT>> PickVal;
      };
    
    /** especially for const iterators we need to use \c const& and \c const* */
    template<class IT, typename SEL>
    struct _MapSubSelector<IT, SEL const&>
      {
        typedef WrappedStlIter< Wrapped_PickKey<IT>>      PickKey; // Key is always const for maps
        typedef WrappedStlIter< Wrapped_PickConstVal<IT>> PickVal;
      };
    
    
    
    template<class MAP>
    struct _MapT
      {
        typedef typename _MapTypeSelector<MAP>::Key KeyType;
        typedef typename _MapTypeSelector<MAP>::Val ValType;
        typedef typename _MapTypeSelector<MAP>::Itr EntryIter;
        
        typedef typename EntryIter::reference DetectConst;
        typedef typename _MapSubSelector<EntryIter,DetectConst>::PickKey PickKeyIter;
        typedef typename _MapSubSelector<EntryIter,DetectConst>::PickVal PickValIter;
        
        typedef RangeIter<PickKeyIter> KeyIter;
        typedef RangeIter<PickValIter> ValIter;
        
        typedef DistinctIter<KeyIter> DistinctKeys;
      };
    
    
    template<class IT>
    struct _MapIterT
      {
        typedef IT EntryIter;
        
        typedef typename EntryIter::value_type::first_type KeyType;
        typedef typename EntryIter::value_type::second_type ValType;
        
        typedef typename EntryIter::reference DetectConst;
        typedef typename _MapSubSelector<EntryIter,DetectConst>::PickKey PickKeyIter;
        typedef typename _MapSubSelector<EntryIter,DetectConst>::PickVal PickValIter;
          
        typedef RangeIter<PickKeyIter> KeyIter;
        typedef RangeIter<PickValIter> ValIter;
        
        typedef DistinctIter<KeyIter> DistinctKeys;
      };
    
    
    
    template<class SEQ>
    struct _SeqT
      {
        typedef typename SEQ::iterator Iter;
        typedef RangeIter<Iter> Range;
        typedef DistinctIter<Range> DistinctVals;
        typedef AddressExposingIter<Range> Addrs;
      };
    
    template<class SEQ>
    struct _SeqT<const SEQ>
      {
        typedef typename SEQ::const_iterator Iter;
        typedef RangeIter<Iter> Range;
        typedef DistinctIter<Range> DistinctVals;
        typedef AddressExposingIter<Range> Addrs;
      };
    
  }//(End) traits/helpers
  
  
  
  
  /** @return Lumiera Forward Iterator
   *           to yield each Element from a STL container
   */
  template<class CON>
  inline typename _SeqT<CON>::Range
  eachElm (CON& coll)
  {
    typedef typename _SeqT<CON>::Range Range;
    return Range (coll.begin(), coll.end());
  }
  
  
  /** @return Lumiera Forward Iterator
   *          exposing the address of each Element within a STL
   */
  template<class CON>
  inline typename _SeqT<CON>::Addrs
  eachAddress (CON& coll)
  {
    typedef typename _SeqT<CON>::Addrs Addresses;
    return Addresses (eachElm (coll));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          each key of a map/multimap
   */
  template<class MAP>
  inline typename _MapT<MAP>::KeyIter
  eachKey (MAP& map)
  {
    typedef typename _MapT<MAP>::KeyIter Range;
    typedef typename _MapT<MAP>::PickKeyIter PickKey;
    
    return Range (PickKey (map.begin()), PickKey (map.end()));
  }
  
  
  /** @return Lumiera Forward Iterator extracting the keys
   *          from a given range of (key,value) pairs
   */
  template<class IT>
  inline typename _MapIterT<IT>::KeyIter
  eachKey (IT const& begin, IT const& end)
  {
    typedef typename _MapIterT<IT>::KeyIter Range;
    typedef typename _MapIterT<IT>::PickKeyIter PickKey;
    
    return Range (PickKey (begin), PickKey (end));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          each value within a map/multimap
   */
  template<class MAP>
  inline typename _MapT<MAP>::ValIter
  eachVal (MAP& map)
  {
    typedef typename _MapT<MAP>::ValIter Range;
    typedef typename _MapT<MAP>::PickValIter PickVal;
    
    return Range (PickVal (map.begin()), PickVal (map.end()));
  }
  
  
  /** @return Lumiera Forward Iterator extracting the values
   *          from a given range of (key,value) pairs
   */
  template<class IT>
  inline typename _MapIterT<IT>::ValIter
  eachVal (IT const& begin, IT const& end)
  {
    typedef typename _MapIterT<IT>::ValIter Range;
    typedef typename _MapIterT<IT>::PickValIter PickVal;
    
    return Range (PickVal (begin), PickVal (end));
  }
  
  
  /** build a Lumiera Forward Iterator to suppress
   *  any repetitions in the given sequence.
   */
  template<class SEQ>
  inline typename _SeqT<SEQ>::DistinctVals
  eachDistinct (SEQ& seq)
  {
    typedef typename _SeqT<SEQ>::Range Range;
    typedef typename _SeqT<SEQ>::DistinctVals DistinctValues;
    
    return DistinctValues (Range (seq.begin(), seq.end()));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          the distinct keys from a multimap
   *  @warning full scan of all keys, dropping repetitions
   */
  template<class MAP>
  inline typename _MapT<MAP>::DistinctKeys
  eachDistinctKey (MAP& map)
  {
    return typename _MapT<MAP>::DistinctKeys (eachKey (map));
  }
  
  
  /** @return Lumiera Forward Iterator to yield
   *          the distinct keys from a multimap
   *  @warning full scan of all keys, dropping repetitions
   */
  template<class MMAP, typename KEY>
  inline typename _MapT<MMAP>::ValIter
  eachValForKey (MMAP& multimap, KEY key)
  {
    typedef typename _MapT<MMAP>::EntryIter Pos;
    typedef typename _MapT<MMAP>::ValIter Range;
    typedef typename _MapT<MMAP>::PickValIter PickVal;
    
    std::pair<Pos,Pos> valRange = multimap.equal_range (key);
    
    return Range (PickVal (valRange.first), PickVal (valRange.second));
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
      typedef std::vector<VAL> Sequence;
      
      mutable
      Sequence buffer_;
      size_t   pos_;
      
      
    public:
      /** create empty snapshot */
      IterSnapshot()
        : buffer_()
        , pos_(0)
        { }
      
      /** take snapshot by discharging a copy
       *  of the given Lumiera Forward iterator
       *  @warning depending on the implementation
       *           backing the source iterator, this
       *           might or might not yield side-effects.
       */
      template<class IT>
      IterSnapshot (IT const& src)
        : buffer_()
        , pos_(0)
        {
          for (IT copy(src); copy; ++copy)
            buffer_.push_back(*copy);
        }
      
      /** take snapshot from STL iterator */
      template<class IT>
      IterSnapshot (IT const& begin, IT const& end)
        : buffer_()
        , pos_(0)
        {
          for (IT p(begin); p!=end; ++p)
            buffer_.push_back(*p);
        }
      
      operator bool() const { return isValid(); }
      
      
      
      /* === lumiera forward iterator concept === */
      
      typedef VAL* pointer;
      typedef VAL& reference;
      typedef VAL  value_type;
      
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
      
      
      /** equality is based both on the actual contents of the snapshots
       *  and the current iterator position */
      friend bool
      operator== (IterSnapshot const& snap1, IterSnapshot const& snap2)
      {
        return snap1.buffer_ == snap2.buffer_
               && snap1.pos_ == snap2.pos_ ;
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
      return ContentSnapshot<CON>(begin(con), end(con));
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
