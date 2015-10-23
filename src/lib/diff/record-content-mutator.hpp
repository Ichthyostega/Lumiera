/*
  RECORD-CONTENT-MUTATOR.hpp  -  helper to remould record contents

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


/** @file record-content-mutator.hpp
 ** Implementation helper for reshaping the contents of a lib::diff::Record.
 ** This technical helper is necessary to apply one level of a "Tree Diff"
 ** to an object represented as #Record::Mutator. Since records as such are
 ** designed as immutable value objects, we build a dedicated #Record::Mutator
 ** when it comes to reordering the contents of a given record. The technical
 ** details of doing so are highly coupled to the actual storage implementation
 ** of #Record, as well as to the actual procedure to apply a diff message, as
 ** implemented in lib::diff::DiffApplicationStrategy.
 ** 
 ** @see generic-record-update-test.cpp
 ** @see tree-diff-application-test.cpp
 ** @see Record::Mutator
 ** @see diff-language.hpp
 ** 
 */


#ifndef LIB_DIFF_RECORD_CONTENT_MUTATOR_H
#define LIB_DIFF_RECORD_CONTENT_MUTATOR_H


#include "lib/error.hpp"

#include <boost/noncopyable.hpp>
#include <utility>

namespace lib {
namespace diff{
  
  using std::move;
  using std::swap;
  
  
  /** @internal helper for DiffApplicationStrategy<Rec::Mutator> */
  template<class STO>
  class RecordContentMutator
    : boost::noncopyable
    {
    public:
      explicit
      RecordContentMutator()
        {
          swap (attribs_, target_.attribs());
          swap (children_, target_.children());
          
          // heuristics for storage pre-allocation
          target_.attribs().reserve (attribs_.size() * 120 / 100);
          target_.children().reserve (children_.size() * 120 / 100);
        }
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_RECORD_CONTENT_MUTATOR_H*/
