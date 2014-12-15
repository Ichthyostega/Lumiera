/*
  LIST-DIFF.hpp  -  language to describe differences in linearised form

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
 ** A token language to represent changes in a list of elements.
 ** In combination with the #DiffLanguage framework, this building block
 ** defines the set of operations to express changes in a given list of elements.
 ** By implementing the #ListDiffInterpreter interface (visitor), a concrete usage
 ** can receive such a diff description and e.g. apply it to a target data structure.
 ** 
 ** @see diff-language.cpp
 ** @see diff-list-application-test.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_LIST_DIFF_H
#define LIB_DIFF_LIST_DIFF_H


#include "lib/diff/diff-language.hpp"


namespace lib {
namespace diff{
  
  
  /**
   * Interpreter interface to define the operations ("verbs"),
   * which describe differences or changes in a given list of data elements.
   * The meaning of the verbs is as follows:
   * - \c ins prompts to insert the given argument element at the \em current
   *          processing position into the target sequence. This operation
   *          allows to inject new data
   * - \c del requires to delete the \em next element at \em current position.
   *          For sake of verification, the element to be deleted is also
   *          included as argument (redundancy).
   * - \c pick just accepts the \em next element at \em current position into
   *          the resulting altered sequence. The element is given redundantly.
   * - \c push effects a re-ordering of the target list contents: it requires
   *          to \em push the \em next element at \em current processing position
   *          back further into the list, to be placed at a position \em behind
   *          the reference element given as argument.
   */
  template<typename E>
  class ListDiffInterpreter
    {
    public:
      virtual ~ListDiffInterpreter() { } ///< this is an interface
      
      virtual void ins(E e)    =0;
      virtual void del(E e)    =0;
      virtual void pick(E e)   =0;
      virtual void push(E e)   =0;
    };
  
  template<typename E>
  using ListDiffLanguage = DiffLanguage<ListDiffInterpreter<E>, E>;
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_LIST_DIFF_H*/
