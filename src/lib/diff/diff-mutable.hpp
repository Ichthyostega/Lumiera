/*
  DIFF-MUTABLE.hpp  -  interface for data structures mutable through diff message

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file diff-mutable.hpp
 ** Abstraction or descriptor interface for a data structure
 ** exposing the ability for mutation by receiving diff messages.
 ** Differences and changes on such data structures can be specified in the form
 ** of a "linearised diff language". Such a diff can be represented as a sequence
 ** of tokens, describing the manipulations necessary to effect the desired change.
 ** While, conceptually, such a diff is drawn against a generic tree like data
 ** description (actually based on diff::Record<GenNode>), with the help of a
 ** customisable intermediary binding layer, it is possible to apply such diff
 ** messages onto data structures otherwise not further disclosed. For this to
 ** work, the target data structure itself must offer the ability to construct
 ** such a suitable intermediary binding layer. This interface declares and
 ** offers this ability.
 ** 
 ** A data structure exposing this interface in one way or the other (which
 ** might be through the help of an ADL bound free function) thus offers some kind
 ** of closure, which, when invoked, will build a concrete TreeMutator implementation
 ** into the given working buffer. Obviously, this TreeMutator is then somehow opaquely
 ** wired to the data structure so to be able to invoke transforming operations on
 ** this target data. Here, the TreeMutator interface defines the primitive operations
 ** necessary to apply a conforming diff message onto this otherwise private data
 ** structure. Based on such a setup, client code may create a DiffApplicator
 ** to embody this custom TreeMutator, which in the end allows to consume
 ** diff messages, thereby transforming and mutating the target data
 ** structure without the need to know any internal details.
 ** 
 ** @see diff-language.hpp
 ** @see tree-diff-application.hpp
 ** @see diff-virtualised-application-test.cpp
 ** @see TreeMutator
 ** 
 */


#ifndef LIB_DIFF_DIFF_MUTABLE_H
#define LIB_DIFF_DIFF_MUTABLE_H


#include "lib/diff/tree-mutator.hpp"



namespace lib {
namespace diff{
  
  
  /**
   * Marker or capability interface:
   * an otherwise not further disclosed data structure,
   * which can be transformed through "tree diff messages"
   */
  class DiffMutable
    {
    public:
      virtual ~DiffMutable() { }  ///< this is an interface
      
      /** build a custom implementation of the TreeMutator interface,
       *  suitably wired to cause appropriate changes to the opaque
       *  data structure, in accordance to the semantics of the
       *  tree diff language.
       * @param buffer a buffer handle, which can be used to placement-construct
       */
      virtual void buildMutator (TreeMutator::Handle buffer)    =0;
    };
  
  
  
}} // namespace lib::diff

// enable an extension to the TreeMutator builder DSL for DiffMutable
#include "lib/diff/tree-mutator-diffmutable-binding.hpp"

#endif /*LIB_DIFF_DIFF_MUTABLE_H*/
