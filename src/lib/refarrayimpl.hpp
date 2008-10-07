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
  
  
} // namespace lib
#endif
