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
 ** \par Design considerations
 ** While -- conceptually -- our tree diff handling can be seen as an extension
 ** and generalisation of list diffing, the decision was \em not to embody this
 ** extension into the implementation technically, for sake of clarity. More so,
 ** since the Record, which serves as foundation for our »External Tree Description«,
 ** was made to look and behave like a list-like entity, but built with two distinct
 ** scopes at implementation level: the attribute scope and the contents scope. This
 ** carries over to the fine points of the list diff language semantics, especially
 ** when it comes to fault tolerance and strictness vs fuzziness in diff application.
 ** The implementation is thus faced with having to deal with an internal focus and
 ** a switch from scope to scope, which adds a lot of complexity. So the list diff
 ** application strategy can be seen as blueprint and demonstration of principles.
 ** 
 ** Another point in question is weather see the diff application as manipulating
 ** a target data structure, or rather building a reshaped copy. The fact that
 ** GenNode and Record are designed as immutable values seems to favour the latter,
 ** yet the very reason to engage into building this diff framework was how to
 ** handle partial updates within a expectedly very large UI model, reflecting
 ** the actual session model in Proc-Layer. So we end up working on a Mutator,
 ** which clearly signals we're going to reshape and re-rig the target data.
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
      using Content = Rec::ContentMutator;
      
      Rec::Mutator& target_;
      Content content_;
      
      
      /* == Implementation of the list diff application primitives == */
      
      void
      ins (GenNode const& n)  override
        {
          UNIMPLEMENTED("insert node");
        }
      
      void
      del (GenNode const& n)  override
        {
          UNIMPLEMENTED("delete next node");
        }
      
      void
      pick (GenNode const& n)  override
        {
          UNIMPLEMENTED("accept next node as-is");
        }
      
      void
      skip (GenNode const& n)  override
        {
          UNIMPLEMENTED("skip void position left by find");
        }      // assume the actual content has been moved away by a previous find()
      
      void
      find (GenNode const& n)  override
        {
          UNIMPLEMENTED("search the named node and insert it here");
        }      // consume and leave waste, expected to be cleaned-up by skip() later
      
      
      
      /* == Implementation of the tree diff application primitives == */
      
      void
      after (GenNode const& n)  override
        {
          UNIMPLEMENTED("cue to a position behind the named node");
        }
      
      void
      mut (GenNode const& n)  override
        {
          UNIMPLEMENTED("open nested context for diff");
        }
      
      void
      emu (GenNode const& n)  override
        {
          UNIMPLEMENTED("finish and leave nested diff context");
        }
      
      
    public:
      explicit
      DiffApplicationStrategy(Rec::Mutator& mutableTargetRecord)
        : target_(mutableTargetRecord)
        , content_()
        {
          target_.swapContent (content_);
        }
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
