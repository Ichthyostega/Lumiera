/*
  INDEX-TABLE.hpp  -  helper for lookup and membership check of sequence like data

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


/** @file index-table.hpp
 ** Generic lookup table for a sequence of unique values.
 ** This helper facility for detecting differences in data sequences
 ** takes a snapshot of the data at construction time and builds a lookup tree.
 ** This allows to find the index position of a given key element, and to detect
 ** membership.
 ** 
 ** @see diff-index-table-test.cpp
 ** @see diff-list-generation-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_INDEX_TABLE_H
#define LIB_DIFF_INDEX_TABLE_H


#include "lib/error.hpp"

#include <vector>
#include <map>


namespace lib {
namespace diff{
  
  using std::vector;
  using std::map;
  
  
  template<typename VAL>
  class IndexTable
    {
    public:
      template<class SEQ>
      IndexTable(SEQ const& seq)
        {
          UNIMPLEMENTED("build index");
        }
      
      size_t
      size()  const
        {
          UNIMPLEMENTED("sequence size");
        }
      
      VAL const&
      getElement (size_t i)  const
        {
          UNIMPLEMENTED("indexed value access");
        }
      
      bool
      contains (VAL const& elm)  const
        {
          return size() == pos(elm);
        }
      
      size_t
      pos (VAL const& elm)  const
        {
          UNIMPLEMENTED("index lookup");
        }
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_INDEX_TABLE_H*/
