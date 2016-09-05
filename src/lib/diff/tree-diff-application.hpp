/*
  TREE-DIFF-APPLICATION.hpp  -  consume and apply a tree diff

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
 ** Concrete implementation to apply structural changes to hierarchical
 ** data structures. Together with the generic #DiffApplicator, this allows
 ** to receive linearised structural diff descriptions and apply them to
 ** a given target data structure, to effect the corresponding changes.
 ** 
 ** ## Design considerations
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
 ** Another point in question is whether to treat the diff application as
 ** manipulating a target data structure, or rather building a reshaped copy.
 ** The fact that GenNode and Record are designed as immutable values seems to favour
 ** the latter, yet the very reason to engage into building this diff framework was
 ** how to handle partial updates within a expectedly very large UI model, reflecting
 ** the actual session model in Proc-Layer. So we end up working on a Mutator,
 ** which clearly signals we're going to reshape and re-rig the target data.
 ** 
 ** \par related
 ** Closely related to this generic application of tree changes is the situation,
 ** where we want to apply structural changes to some non-generic and private data
 ** structure. In fact, it is possible to _use the same tree diff language_ for
 ** this specific case, with the help of an _adapter_. Thus, within our diff
 ** framework, we provide a _similar binding_ for the DiffApplicator, but
 ** then targeted towards such an [structure adapter](\ref TreeMutator)
 ** 
 ** ## State and nested scopes
 ** Within the level of a single #Record, our tree diff language works similar to
 ** the list diff (with the addition of the \c after(ID) verb, which is just a
 ** shortcut to accept parts of the contents unaltered). But after possibly rearranging
 ** the contents of an "object" (Record), the diff might open some of its child "objects"
 ** by entering a nested scope. This is done with the \c mut(ID)....emu(ID) bracketing
 ** construct. On the implementation side, this means we need to use a stack somehow.
 ** The decision was to manage this stack explicitly, as a std::stack (heap memory).
 ** Each entry on this stack is a "context frame" for list diff. Which makes the
 ** tree diff applicator a highly statefull component.
 ** 
 ** Even more so, since -- for \em performance reasons -- we try to alter the
 ** tree shaped data structure \em in-place. We want to avoid the copy of possibly
 ** deep sub-trees, when in the end we might be just rearranging their sequence order.
 ** This design decision comes at a price tag though
 ** - it subverts the immutable nature of \c Record<GenNode> and leads to
 **   high dependency on data layout and implementation details of the latter.
 **   This is at least prominently marked by working on a diff::Record::Mutator,
 **   so the client has first to "open up" the otherwise immutable tree
 ** - the actual list diff on each level works by first \em moving the entire
 **   Record contents away into a temporary buffer and then \em moving them
 **   back into new shape one by one. In case of a diff conflict  (i.e. a
 **   mismatch between the actual data structure and the assumptions made
 **   for the diff message on the sender / generator side), an exception
 **   is thrown, leaving the client with a possibly corrupted tree, where
 **   parts might even still be stashed away in the temporary buffer,
 **   and thus be lost.
 ** We consider this unfortunate, yet justified  by the very nature of applying a diff.
 ** When the user needs safety or transactional behaviour, a deep copy should be made
 ** before attaching the #DiffApplicator
 ** 
 ** @note as of 2/2016, there is the possibility this solution will become part
 **       of a more generic solution, currently being worked out in tree-diff-mutator-binding.hpp
 ** 
 ** @see DiffTreeApplication_test
 ** @see DiffListApplication_test
 ** @see GenNodeBasic_test
 ** @see tree-diff.hpp
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_APPLICATION_H
#define LIB_DIFF_TREE_DIFF_APPLICATION_H


#include "lib/diff/tree-diff.hpp"
#include "lib/diff/tree-diff-mutator-binding.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>

namespace lib {
namespace diff{
  
  using util::unConst;
  using util::cStr;
  using util::_Fmt;
  using std::move;
  using std::swap;
  
  
  
  
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
