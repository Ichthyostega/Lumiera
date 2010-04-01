/*
  ITER-ADAPTER-STL.hpp  -  helpers for building simple forward iterators 
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/////////////////////////TODO remove
#include "lib/itertools.hpp"
/////////////////////////TODO remove



namespace lib {
namespace iter_stl {
  
  
  /** 
   * helper baseclass to simplify
   * defining customised wrapped STL iterators
   */
  template<typename IT>
  class WrappedStlIter
    {
      IT i_;
      
    public:
      typedef typename IT::value_type value_type;
      typedef typename IT::reference reference;
      typedef typename IT::pointer  pointer;
      
      
      WrappedStlIter()            : i_()  { }
      WrappedStlIter(IT const& i) : i_(i) { }
      
      IT const& get()        const  { return i_;  }
      pointer   operator->() const  { return i_;  }
      reference operator*()  const  { return *i_; }
      
      WrappedStlIter& operator++()  { ++i_; return *this; }
      
      friend bool operator== (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ == i2.i_; }
      friend bool operator!= (WrappedStlIter const& i1, WrappedStlIter const& i2) { return i1.i_ != i2.i_; }
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
    
    
    
    template<class MAP>
    struct _MapT
      {
        typedef typename _MapTypeSelector<MAP>::Key KeyType;
        typedef typename _MapTypeSelector<MAP>::Val ValType;
        typedef typename _MapTypeSelector<MAP>::Itr EntryIter;
        
        typedef WrappedStlIter<EntryIter> WrapI;
        
        struct PickKeyIter : WrapI
          {
            typedef KeyType  value_type;
            typedef KeyType& reference;
            typedef KeyType* pointer;
            
            PickKeyIter ()               : WrapI()     {}
            PickKeyIter (EntryIter iter) : WrapI(iter) {}
            
            pointer   operator->() const { return &(WrapI::get()->first); }
            reference operator* () const { return  (WrapI::get()->first); }
          };
        
        struct PickValIter : WrapI
          {
            typedef ValType  value_type;
            typedef ValType& reference;
            typedef ValType* pointer;
            
            PickValIter ()               : WrapI()     {}
            PickValIter (EntryIter iter) : WrapI(iter) {}
            
            pointer   operator->() const { return &(WrapI::get()->second); }
            reference operator* () const { return  (WrapI::get()->second); }
          };
          
          typedef RangeIter<PickKeyIter> KeyIter;
          typedef RangeIter<PickValIter> ValIter;
      };
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
  
  
  /**
   */
  template<class MAP>
  inline typename _MapT<MAP>::KeyIter
  eachDistinctKey (MAP)
  {
    UNIMPLEMENTED ("each distinct key a multimap as Lumiera Forward Iterator");
  }
  
  
}} // namespace lib::iter_stl
#endif
