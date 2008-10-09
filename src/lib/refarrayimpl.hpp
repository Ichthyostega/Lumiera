/*
  REFARRAYIMPL.hpp  -  some implementations of the ref-array interface
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef LIB_REFARRAYIMPL_H
#define LIB_REFARRAYIMPL_H


#include "lib/refarray.hpp"
#include "proc/nobugcfg.hpp"

#include <vector>
using std::vector;



namespace lib {
  
  /**
   * Wrap a vector holding objects of a subtype
   * and provide array-like access using the interface type.
   */
  template<class E, class IM = E>
  class RefArrayVectorWrapper
    : public RefArray<E>
    {
      typedef vector<IM> const& Tab;
      Tab table_;
      
    public:
      
      RefArrayVectorWrapper (Tab toWrap)
        : table_(toWrap)
        { }
      
      virtual size_t size()  const
        {
          return table_.size();
        }
      
      virtual E const& operator[] (uint i)  const 
        {
          REQUIRE (i < size());
          return table_[i];
        }
    };
  
  
  /**
   * This variation of the wrapper actually \em is
   * a vector, but can act as a RefArray
   */
  template<class E, class IM = E>
  class RefArrayVector
    : public vector<IM>,
      public RefArrayVectorWrapper<E,IM>
    {
      typedef RefArrayVectorWrapper<E,IM> Wrap;
      typedef vector<IM> Vect;
      typedef typename Vect::size_type Size_t;
      typedef typename Vect::value_type Val_t;
      
    public:
      RefArrayVector()                                   : Vect(),    Wrap(*this) {}
      RefArrayVector(Size_t n, Val_t const& v = Val_t()) : Vect(n,v), Wrap(*this) {}
      RefArrayVector(Vect const& ref)                    : Vect(ref), Wrap(*this) {}
      
      using Vect::size;
      using Wrap::operator[];
    };
  
  
  /**
   * RefArray implementation based on a fixed sized array,
   * i.e. the storage is embedded. Embedded value type
   * either needs to be default- or copy-constructible.
   */
  template<class E, size_t n, class IM = E>
  class RefArrayTable
    : public RefArray<E>
    {
      IM array_[n];
      
    public:
      RefArrayTable()
        {
          size_t i=0;
          try
            {
              while (i<n)  new(&array_[i++]) IM();
            }
          catch(...) { cleanup(i); throw; }
        }
      
      template<class FAC>
      RefArrayTable(FAC& factory)
        {
          size_t i=0;
          try
            {
              while (i<n)  new(&array_[i]) IM (factory(i++));
            }
          catch(...) { cleanup(i); throw; }
        }
      
      ~RefArrayTable() { cleanup(); }
      
    private:
      void cleanup(size_t top=n) throw()
        {
          while (top) array_[--top].~IM();
        }
      
      
    public: //----RefArray-Interface----------
      
      virtual size_t size()  const
        { 
          return n; 
        }
      
      virtual E const& operator[] (uint i)  const 
        {
          REQUIRE (i < size());
          return array_[i];
        }

    };
  
} // namespace lib
#endif
