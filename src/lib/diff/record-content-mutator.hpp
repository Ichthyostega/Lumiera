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
 ** @warning this struct is marked "internal" for a reason;
 **          it serves the purpose to remove technicalities from usage site,
 **          yet it is \em not a proper abstraction. Be sure you understand
 **          the storage layout, especially when testing for iteration end.
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
  
  namespace error = lumiera::error;
  using std::move;
  using std::swap;
  
  
  /** @internal helper for DiffApplicationStrategy<Rec::Mutator> */
  template<class STO>
  struct RecordContentMutator
    : boost::noncopyable
    {
      using Iter = typename STO::iterator;
      
      STO attribs;
      STO children;
      Iter pos;
      
      RecordContentMutator()
        : attribs()
        , children()
        , pos(attribs.begin())
        { }
      
      
      bool
      empty()  const
        {
          return attribs.empty()
             and children.empty();
        }
      
      bool
      currIsAttrib()  const
        {
          return & *pos >= & *attribs.begin()
             and & *pos <  & *attribs.end();
        }
      
      bool
      currIsChild()  const
        {
          return & *pos >= & *children.begin()
             and & *pos <  & *children.end();
        }
      
      Iter end()        { return children.end(); }
      Iter end()  const { return children.end(); }
      
      RecordContentMutator&
      operator++()
        {
          if (pos == children.end())
            throw error::State ("attempt to iterate beyond end of scope"
                               ,error::LUMIERA_ERROR_ITER_EXHAUST);
          if (pos == attribs.end())
            pos = children.begin();
          else
            ++pos;
          if (pos == attribs.end())
            pos = children.begin();
          return *this;
        }
      
      void
      resetPos()
        {
          if (attribs.empty())
            jumpToChildScope();
          else
            jumpToAttribScope();
        }
      
      void
      jumpToAttribScope()
        {
          pos = attribs.begin();
        }
      
      void
      jumpToChildScope()
        {
          pos = children.begin();
        }
      
      void
      preAllocateStorage(size_t attribCnt, size_t childrenCnt)
        {
          // heuristics for storage pre-allocation (for tree diff application)
          attribs.reserve (attribCnt * 120 / 100);
          children.reserve (childrenCnt * 120 / 100);
          ASSERT (this->empty());
        }
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_RECORD_CONTENT_MUTATOR_H*/