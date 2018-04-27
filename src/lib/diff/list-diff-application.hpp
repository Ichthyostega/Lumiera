/*
  LIST-DIFF-APPLICATION.hpp  -  consume and apply a list diff

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file list-diff-application.hpp
 ** Apply a "list diff" to a concrete sequence of elements in a container.
 ** This header provides specialisation(s) of the DiffApplicationStrategy to
 ** actual containers, choosing an implementation approach suitable for this
 ** specific kind of container. Together with a lib::diff::DiffApplicator,
 ** this allows to receive the description of changes (as a linearised sequence
 ** of DiffStep tokens) and apply them to a given concrete sequence of data
 ** elements, thereby transforming the contents of this target sequence.
 ** 
 ** @see diff-list-application-test.cpp
 ** @see ListDiffLanguage
 ** 
 */


#ifndef LIB_DIFF_LIST_DIFF_APPLICATION_H
#define LIB_DIFF_LIST_DIFF_APPLICATION_H


#include "lib/diff/list-diff.hpp"
#include "lib/format-string.hpp"

#include <algorithm>
#include <vector>
#include <tuple>


namespace lib {
namespace diff{
  
  using util::_Fmt;
  using std::vector;
  using std::move;
  
  /**
   * concrete strategy to apply a list diff to a target sequence given as vector.
   * The implementation swaps aside the existing content of the target sequence
   * and then consumes it step by step, while building up the altered content
   * within the previously emptied target vector. Whenever possible, elements
   * are moved directly to the target location.
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @warning behaves only EX_SANE in case of diff application errors,
   *          i.e. only partially modified / rebuilt sequence might be
   *          in the target when diff application is aborted
   * @see #ListDiffInterpreter explanation of the verbs
   */
  template<typename E, typename...ARGS>
  class DiffApplicationStrategy<vector<E,ARGS...>>
    : public ListDiffInterpreter<E>
    {
      using Vec = vector<E,ARGS...>;
      using Iter = typename Vec::iterator;
      
      Vec orig_;
      Vec& seq_;
      Iter pos_;
      
      bool
      end_of_target()
        {
          return pos_ == orig_.end();
        }
      
      void
      __expect_in_target (E const& elm, Literal oper)
        {
          if (end_of_target())
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "no (further) elements in target sequence") % oper % elm
                              , LERR_(DIFF_CONFLICT));
          if (*pos_ != elm)
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "found element %s on current target position instead")
                                    % oper % elm % *pos_
                              , LERR_(DIFF_CONFLICT));
        }
      
      void
      __expect_further_elements (E const& elm)
        {
          if (end_of_target())
            throw error::State(_Fmt("Premature end of target sequence, still expecting element %s; "
                                    "unable to apply diff further.") % elm
                              , LERR_(DIFF_CONFLICT));
        }
      
      void
      __expect_found (E const& elm, Iter const& targetPos)
        {
          if (targetPos == orig_.end())
            throw error::State(_Fmt("Premature end of sequence; unable to locate "
                                    "element %s in the remainder of the target.") % elm
                              , LERR_(DIFF_CONFLICT));
        }
      
      
      /* == Implementation of the diff application primitives == */
      
      void
      ins (E const& elm)  override
        {
          seq_.push_back(elm);
        }
      
      void
      del (E const& elm)  override
        {
          __expect_in_target(elm, "remove");
          ++pos_;
        }
      
      void
      pick (E const& elm)  override
        {
          __expect_in_target(elm, "pick");
          seq_.push_back (move(*pos_));
          ++pos_;
        }
      
      void
      skip (E const& elm)  override
        {
          __expect_further_elements (elm);
          ++pos_;
        }      // assume the actual content has been moved away by a previous find()
      
      void
      find (E const& elm)  override
        {
          __expect_further_elements (elm);
          Iter found = std::find(pos_, orig_.end(), elm);
          __expect_found (elm, found);
          seq_.push_back (move(*found));
        }      // consume and leave waste, expected to be cleaned-up by skip() later
      
      
    public:
      explicit
      DiffApplicationStrategy(vector<E>& targetVector)
        : seq_(targetVector)
        { }
      
      void
      initDiffApplication()
        {
          swap (seq_, orig_);
          seq_.reserve (orig_.size() * 120 / 100);    // heuristics for storage pre-allocation
          pos_ = orig_.begin();
        }
    };
  
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_LIST_DIFF_APPLICATION_H*/
