/*
  TREE-DIFF.hpp  -  language to describe differences in linearised form

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


/** @file list-diff.hpp
 ** A token language to represent structural changes in a tree like
 ** hierarchical data structure. In combination with the #DiffLanguage framework,
 ** this building block defines the set of operations to express both content
 ** and structural changes in a given data structure.
 ** 
 ** @todo UNIMPLEMENTED as of 12/14
 ** 
 ** @see diff-language.cpp
 ** @see diff-list-application-test.cpp
 ** @see list-diff.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_H
#define LIB_DIFF_TREE_DIFF_H


#include "lib/diff/diff-language.hpp"


namespace lib {
namespace diff{
  
  
  /**
   * Interpreter interface to define the operations ("verbs"),
   * which describe differences or changes in hierarchical data structure.
   * The meaning of the verbs is as follows:
   * - \c TODO
   * 
   * @todo to be defined
   */
  template<typename E>
  class TreeDiffInterpreter
    {
    public:
      
      ///////TODO actual operations go here
    };
  
  template<typename E>
  using TreeDiffLanguage = DiffLanguage<TreeDiffInterpreter<E>, E>;
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_H*/