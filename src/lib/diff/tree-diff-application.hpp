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

#include <utility>

namespace lib {
namespace diff{
  
  using std::move;
  using std::swap;
  
  
  /**
   * concrete strategy to apply a structural diff to a target data structure
   * made from #Record<GenNode> elements. This data structure is assumed to be
   * recursive, tree-like. But because Record elements are conceived as immutable
   * and value-like, the tree diff application actually works on a Rec::Mutator
   * wrapping the target record to be altered through consuming the diff.
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @see #TreeDiffInterpreter explanation of the verbs
   */
  template<>
  class DiffApplicationStrategy<Rec::Mutator>
    : public TreeDiffInterpreter
    {
      using Storage = RecordSetup<GenNode>::Storage;
      
      Rec::Mutator& target_;
      Storage attribs_;
      Storage children_;
      
      
      /* == Implementation of the list diff application primitives == */
      
      void
      ins (GenNode n)  override
        {
          UNIMPLEMENTED("insert node");
        }
      
      void
      del (GenNode n)  override
        {
          UNIMPLEMENTED("delete next node");
        }
      
      void
      pick (GenNode n)  override
        {
          UNIMPLEMENTED("accept next node as-is");
        }
      
      void
      skip (GenNode n)  override
        {
          UNIMPLEMENTED("skip void position left by find");
        }      // assume the actual content has been moved away by a previous find()
      
      void
      find (GenNode n)  override
        {
          UNIMPLEMENTED("search the named node and insert it here");
        }      // consume and leave waste, expected to be cleaned-up by skip() later
      
      
      
      /* == Implementation of the tree diff application primitives == */
      
      void
      after (GenNode n)  override
        {
          UNIMPLEMENTED("cue to a position behind the named node");
        }
      
      void
      mut (GenNode n)  override
        {
          UNIMPLEMENTED("open nested context for diff");
        }
      
      void
      emu (GenNode n)  override
        {
          UNIMPLEMENTED("finish and leave nested diff context");
        }
      
      
    public:
      explicit
      DiffApplicationStrategy(Rec::Mutator& mutableTargetRecord)
        : target_(mutableTargetRecord)
        , attribs_()
        , children_()
        {
          swap (attribs_, target_.attribs());
          swap (children_, target_.children());
          
          // heuristics for storage pre-allocation
          target_.attribs().reserve (attribs_.size() * 120 / 100);
          target_.children().reserve (children_.size() * 120 / 100);
        }
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
