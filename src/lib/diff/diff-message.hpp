/*
  DIFF-MESSAGE.hpp  -  message to cause changes to generic model elements

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file diff-message.hpp
 ** Generic Message with an embedded diff, to describe changes to model elements.
 ** The ability to create and apply such messages to describe and effect changes,
 ** without actually knowing much about the target to receive the diff, relies on the
 ** [diff framework](\ref diff-language.hpp).
 ** 
 ** The challenging part with this task is the fact that we need to pass such messages
 ** over abstraction barriers and even schedule them into another thread (the UI event thread),
 ** but diff application actually is a _pull operation_ and thus indicates that there must
 ** be a callback actually to retrieve the diff content.
 ** 
 ** @todo as of 1/2017 this is placeholder code and we need a concept //////////////////////////////////////////TICKET #1066 : how to pass diff messages
 ** @todo as of 8/2017 I am still rather clueless regarding the concrete situation to generate DiffMessage,
 **       and as a move into the dark I'll just define it to be based on IterSource...
 ** 
 ** @see DiffMessage_test
 ** @see AbstractTangible_test
 ** @see BusTerm::change()
 ** 
 */


#ifndef LIB_DIFF_DIFF_MESSAGE_H
#define LIB_DIFF_DIFF_MESSAGE_H


#include "lib/error.hpp"
#include "lib/iter-source.hpp"
#include "lib/diff/tree-diff.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/meta/util.hpp"

#include <string>


namespace lib {
namespace diff{
  
  using std::string;
  using std::__and_;
  using std::__not_;
  using lib::meta::enable_if;
  using lib::meta::can_IterForEach;
  using lib::meta::can_STL_ForEach;
  
  using DiffStep = TreeDiffLanguage::DiffStep;
  using DiffSource = IterSource<DiffStep>;
  
  
  
  /**
   * Opaque message to effect a structural change on a target, which is
   * likewise only known in an abstract way, as being specifically structured.
   * Sending such messages typically allows some _implemention defined_ part within
   * the Session to communicate structure and content to some other _implementation defined_
   * part within the UI-Layer, without the necessity of both partners to be tightly coupled on
   * implementation level or even know much about the other's implementation details. As motivation,
   * contrast this to a naive UI implementation, which directly accesses some backend data structure;
   * any change to the backend implementation typically affects the UI implementation on a detail level.
   *  
   * @todo WIP 8/2017 -- as a bold step towards the solution yet to be discovered,
   *       I now define DiffMessage to be based on IterSource<DiffStep>, which means
   *       to add yet another abstraction barrier, so the implementation of diff generation
   *       remains confined within the producer of DiffMessages.
   * @warning yet still the fundamental problem remains: the production context of such
   *       diff messages need to be conserved beyond the producer's thread context, because
   *       it will be pulled asynchronous from within the UI event thread!
   */
  struct DiffMessage
    : DiffSource::iterator
    {
      using _FrontEnd = DiffSource::iterator;
      
      
      DiffMessage() = default;
      
      /**
       * DiffMessage builder:
       * take ownership of an opaque heap allocated context
       * from which the concrete diff can be pulled on demand 
       */
      explicit
      DiffMessage(DiffSource* diffGenerationContext)
        : _FrontEnd{DiffSource::build (diffGenerationContext)}
        { }
      
      template<class IT>
      DiffMessage(IT ii,                      enable_if< can_IterForEach<IT>, void*> =nullptr)
        : _FrontEnd{iter_source::wrapIter(ii)}
        { }
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_DIFF_MESSAGE_H*/
