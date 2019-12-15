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
 ** this building block defines the set of operations to express both structural
 ** and content changes in a given data structure.
 ** 
 ** This »tree diff language« does not rely on any concrete data structure or layout,
 ** just on some general assumptions regarding the ordering and structure of the data.
 ** - top level is a root record
 ** - a record has a type, a collection of named attributes, and a collection of children
 ** - all elements within a record are conceived as elements in ordered sequence, with the
 **   attributes first, followed by the children. The end of the attribute scope is marked
 **   by the the first emerging unnamed entry, i.e the first child.
 ** - the individual elements in these sequences have a distinguishable identity and
 **   optionally a name (and a named element counts as attribute).
 ** - moreover, the elements carry a typed payload data element, which possibly is
 **   a \em nested record ("nested child object"). In case of value elements,
 **   however, the element itself is identified with this value payload.
 ** - the typing of the elements is outside the scope of the diff language; it is assumed
 **   that the receiver of the diff knows what types to expect and how to deal with them.
 ** - there is a notion of changing or mutating the data content, while retaining
 **   the identity of the element. Of course this requires the data content to be
 **   assignable, which makes content mutation an optional feature.
 ** - beyond that, like in list diff, elements might be changed through a sequence of
 **   deletion and insertion of a changed element with the same identity.
 ** - since the tree like data structure is _recursive_, mutation of nested records
 **   is represented by "opening" the nested record, followed by a recursive diff.
 ** By implementing the #TreeDiffInterpreter interface (visitor), a concrete usage
 ** can receive a diff description and possibly apply it to suitable target data.
 ** @remarks the standard usage is to create a DiffApplicator(target) and fed
 **     a diff sequence to it. We provide a standard implementation of the
 **     DiffApplicator + DiffApplicationStrategy, based on a _custimisable intermediary,_
 **     the TreeMutator. This allows to apply a given tree to any suitably compatible
 **     target data structure; notably there is a preconfigured setup for our
 **     _"generic tree representation"_, diff::Record<GenNode>.
 ** 
 ** @see diff-language.cpp
 ** @see tree-diff-application.cpp
 ** @see DiffTreeApplication_test
 ** @see list-diff.cpp
 ** @see diff::GenNode
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
   * - \c ins prompts to insert the given argument element at the \em current
   *          processing position into the target sequence. This operation
   *          allows to inject new data
   * - \c del requires to delete the \em next element at \em current position.
   *          For sake of verification, the ID of the argument payload is
   *          required to match the ID of the element about to be discarded.
   * - \c pick just accepts the \em next element at \em current position into
   *          the resulting altered sequence. Again, the ID of the argument
   *          has to match the ID of the element to be picked, for sake
   *          of verification.
   * - \c find effect a re-ordering of the target scope contents: it requires
   *          to \em search for the (next respective single occurrence of the)
   *          given element further down into the remainder of the current
   *          record scope (but not into nested child scopes). The designated
   *          element is to be retrieved and inserted as the next element
   *          at current position.
   * - \c skip processing hint, emitted at the position where an element
   *          previously extracted by a \c find verb happened to sit within
   *          the old order. This allows an optimising implementation to “fetch”
   *          a copy and just drop or skip the original, thereby avoiding to
   *          shift any other elements.
   * - \c after shortcut to \c pick existing elements up to the designated point.
   *          As a special notation, \c after(Ref::ATTRIBUTES) allows to fast forward
   *          to the first child element, while \c after(Ref::END) means to accept
   *          all of the existing data contents as-is (presumably to append further
   *          elements beyond that point).
   * - \c set assign a new value to the designated element.
   *          This is primarily intended for primitive data values and requires
   *          the payload type to be assignable, without changing the element's
   *          identity. The element is identified by the payload's ID and needs
   *          to be present already, i.e. it has to be mentioned by preceding
   *          order defining verbs (the list diff verbs, `pick`, or `find`).
   * - \c mut bracketing construct to open a nested sub scope, for mutation.
   *          The element designated by the ID of the argument needs to be a
   *          ["nested child object"](\ref Record). Moreover, this element must
   *          have been mentioned with the preceding diff verbs at that level,
   *          which means that the element as such must already be present in the
   *          altered target structure. The `mut(ID)` verb then opens this nested
   *          record for diff handling, and all subsequent diff verbs are to be
   *          interpreted relative to this scope, until the corresponding
   *          \c emu(ID) verb is encountered.
   * - \c emu bracketing construct and counterpart to \c mut(ID). This verb
   *          must be given precisely at the end of the nested scope (it is
   *          not allowed to "return" from the middle of a scope, for sake
   *          of sanity). At this point, this child scope is left and the
   *          parent scope with all existing diff state is popped from an
   *          internal stack
   */
  class TreeDiffInterpreter
    {
    public:
      using Val = GenNode;
      
      virtual ~TreeDiffInterpreter() { } ///< this is an interface
      
      virtual void ins (GenNode const& n)   =0;
      virtual void del (GenNode const& n)   =0;
      virtual void pick(GenNode const& n)   =0;
      virtual void find(GenNode const& n)   =0;
      virtual void skip(GenNode const& n)   =0;
      
      virtual void after(GenNode const&n)   =0;
      virtual void set (GenNode const& n)   =0;
      virtual void mut (GenNode const& n)   =0;
      virtual void emu (GenNode const& n)   =0;
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
      DiffStep_CTOR(set);
      DiffStep_CTOR(mut);
      DiffStep_CTOR(emu);
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_H*/
