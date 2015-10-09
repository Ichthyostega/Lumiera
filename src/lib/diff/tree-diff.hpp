/*
  TREE-DIFF.hpp  -  language to describe differences in hierarchical data structures

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


/** @file tree-diff.hpp
 ** A token language to represent structural changes in a tree like
 ** hierarchical data structure. In combination with the #DiffLanguage framework,
 ** this building block defines the set of operations to express both content
 ** and structural changes in a given data structure.
 ** 
 ** @todo UNIMPLEMENTED as of 12/14
 ** 
 ** @see diff-language.cpp
 ** @see diff-tree-application-test.cpp
 ** @see tree-diff.cpp
 ** @see GenNode
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_H
#define LIB_DIFF_TREE_DIFF_H


#include "lib/diff/diff-language.hpp"
#include "lib/diff/list-diff.hpp"
#include "lib/diff/gen-node.hpp"


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
  class TreeDiffInterpreter
    {
    public:
      using Val = GenNode;
      
      virtual ~TreeDiffInterpreter() { } ///< this is an interface
      
      virtual void ins (GenNode n)   =0;
      virtual void del (GenNode n)   =0;
      virtual void pick(GenNode n)   =0;
      virtual void find(GenNode n)   =0;
      virtual void skip(GenNode n)   =0;
      
      virtual void after(GenNode n)  =0;
      virtual void mut (GenNode n)   =0;
      virtual void emu (GenNode n)   =0;
    };
  
  struct TreeDiffLanguage
    : DiffLanguage<TreeDiffInterpreter, GenNode>
    {
      using Interpreter = TreeDiffInterpreter;
      
      // List Diff sub language
      DiffStep_CTOR(ins);
      DiffStep_CTOR(del);
      DiffStep_CTOR(pick);
      DiffStep_CTOR(find);
      DiffStep_CTOR(skip);
      
      // Tree structure verbs
      DiffStep_CTOR(after);
      DiffStep_CTOR(mut);
      DiffStep_CTOR(emu);
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_H*/
