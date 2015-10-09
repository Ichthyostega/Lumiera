/*
  TREE-DIFF-APPLICATION.hpp  -  language to describe differences in linearised form

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


/** @file tree-diff-application.hpp
 ** Concrete implementation(s) to apply structural changes to hierarchical
 ** data structures. Together with the generic #DiffApplicator, this allows
 ** to receive linearised structural diff descriptions and apply them to
 ** a given target data structure, to effect the corresponding changes.
 ** 
 ** @see diff-list-application-test.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_APPLICATION_H
#define LIB_DIFF_TREE_DIFF_APPLICATION_H


#include "lib/diff/tree-diff.hpp"
#include "lib/diff/gen-node.hpp"

namespace lib {
namespace diff{
  
  /**
   * concrete strategy to apply a structural diff to a target data structure
   * made from #Record<GenNode> elements.
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @see #TreeDiffInterpreter explanation of the verbs
   */
  template<>
  class DiffApplicationStrategy<Rec>
    : public TreeDiffInterpreter
    {
      
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
