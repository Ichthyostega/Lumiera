/*
  INDEX-TABLE.hpp  -  helper for lookup and membership check of sequence like data

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include "lib/util.hpp"
#include "lib/format-string.hpp"

#include <vector>
#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using util::_Fmt;
  
  
  
  /** data snapshot and lookup table */
  template<typename VAL>
  class IndexTable
    {
      std::vector<VAL>    data_;
      std::map<VAL,size_t> idx_;
      
    public:
      template<class SEQ>
      IndexTable(SEQ const& seq)
        {
          size_t i = 0;
          for (auto const& elm : seq)
            {
              __rejectDuplicate(elm);
              data_.push_back (elm);
              idx_[elm] = i++;
            }
        }
      
      /* === forwarded sequence access === */
      
      using iterator = typename std::vector<VAL>::iterator;
      using const_iterator = typename std::vector<VAL>::const_iterator;
      
      iterator       begin()       { return data_.begin(); }
      iterator       end()         { return data_.end();   }
      const_iterator begin() const { return data_.begin(); }
      const_iterator end()   const { return data_.end();   }
      
      size_t         size()  const { return data_.size();  }
      
      
      
      
      VAL const&
      getElement (size_t i)  const
        {
          REQUIRE (i < size());
          return data_[i];
        }
      
      
      bool
      contains (VAL const& elm)  const
        {
          return pos(elm) != size();
        }
      
      
      size_t
      pos (VAL const& elm)  const
        {
          auto entry = idx_.find (elm);
          return entry==idx_.end()? size()
                                  : entry->second;
        }
      
    private:
      void
      __rejectDuplicate (VAL const& elm)
        {
          if (util::contains (idx_, elm))
              throw error::Logic(_Fmt("Attempt to add duplicate %s to index table") % elm);
        }
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_INDEX_TABLE_H*/
