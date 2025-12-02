/*
  LIST-DIFF.hpp  -  language to describe differences between list like sequences

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file list-diff.hpp
 ** A token language to represent changes in a list of elements.
 ** In combination with the [DiffLanguage framework](\ref diff-langue.hpp), this building
 ** block defines the set of operations to express changes in a given list of elements.
 ** By implementing the lib::diff::ListDiffInterpreter interface (visitor), a concrete
 ** usage can receive such a diff description and e.g. apply it to a target data structure.
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
   * - \c find effect a re-ordering of the target list contents: it requires
   *          to \em search for the (next respective single occurrence of the)
   *          given element further down into the remainder of the list,
   *          to bring it forward and insert it as the next element.
   * - \c skip processing hint, emitted at the position where an element
   *          previously extracted by a \c find verb happened to sit within
   *          the old order. This allows an optimising implementation to “fetch”
   *          a copy and just drop or skip the original, thereby avoiding to
   *          shift any other elements.
   */
  template<typename E>
  class ListDiffInterpreter
    {
    public:
      virtual ~ListDiffInterpreter() { } ///< this is an interface
      
      virtual void ins (E const& e)  =0;
      virtual void del (E const& e)  =0;
      virtual void pick(E const& e)  =0;
      virtual void find(E const& e)  =0;
      virtual void skip(E const& e)  =0;
    };
  
  template<typename E>
  struct ListDiffLanguage
    : DiffLanguage<ListDiffInterpreter<E>, E>
    {
      using Interpreter = ListDiffInterpreter<E>;
      
      DiffStep_CTOR(ins);
      DiffStep_CTOR(del);
      DiffStep_CTOR(pick);
      DiffStep_CTOR(find);
      DiffStep_CTOR(skip);
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_LIST_DIFF_H*/
