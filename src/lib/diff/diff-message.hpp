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
#include "lib/iter-adapter-stl.hpp"
#include "lib/diff/tree-diff.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-util.hpp"
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
      // default copy operations acceptable
      
      /**
       * DiffMessage builder:
       * take ownership of an opaque heap allocated context
       * from which the concrete diff can be pulled on demand 
       */
      explicit
      DiffMessage(DiffSource* diffGenerationContext)
        : _FrontEnd{DiffSource::build (diffGenerationContext)}
        { }
      
      /**
       * Convenience builder for consuming an brace enclosed initializer_list
       * @note initialiser elements will be _copied_ into a _heap alloacated_
       *       snapshot (vector), which is then managed by `shared_ptr`
       */
      DiffMessage(std::initializer_list<DiffStep> const&& ili)
        : DiffMessage{iter_stl::snapshot (move(ili))}
        { }
      
      /**
       * Convenience builder to take an arbitrary number of DiffStep arguments
       * @note like for the initializer_list, arguments will be copied into
       *       a _heap allocated snapshot_
       */
      template<typename...ARGS>
      DiffMessage(ARGS&& ...args)
        : DiffMessage{ {std::forward<ARGS>(args)...} }
        { }
      
      /**
       * Convenience builder to piggyback any Lumiera Forward Iterator
       * @note source iterator is copied into a heap allocated IterSource
       */
      template<class IT>
      DiffMessage(IT&& ii,                            enable_if< can_IterForEach<IT>, void*> =nullptr)
        : _FrontEnd{iter_source::wrapIter (std::forward<IT>(ii))}
        { }
      
      /**
       * Convenience builder to use elements form any STL-like container
       * @note creating heap allocated IterSource, which _refers_ to the original container
       * @warning like with any classical iterators, the container must stay alive and accessible 
       */
      template<class CON>
      DiffMessage(CON& container,                     enable_if< __and_< can_STL_ForEach<CON>
                                                                       ,__not_<can_IterForEach<CON>>>, void*> =nullptr)
        : _FrontEnd{iter_source::eachEntry(container)}
        { }
      
      
      /**
       * enable support to show content of the message.
       * After calling this function, operator string() renders all DiffSteps
       * @warning since by design a DiffMessage can only be ``pulled'' once,
       *          this operation needs to impose a _side effect:_ it materialises
       *          the complete diff sequence at once into a heap allocated buffer.
       */
      DiffMessage& updateDiagnostics();
    };
  
  
  
  namespace { // Implementation: take snapshot to enable diagnostics
    
    /** "materialised view" of the diff sequence */
    struct DiffSnapshot
      : std::vector<DiffStep>
      {
        DiffSnapshot(DiffMessage& srcMsg)
          {
            for ( ; srcMsg; ++srcMsg )
              push_back (*srcMsg);
          }
      };
    
    using _VecIter = DiffSnapshot::iterator;
    using _RangeIT = RangeIter<_VecIter>;
    using _Wrapped = WrappedLumieraIter<_RangeIT>;
    
    /**
     * Decorator to be layered transparently on top of DiffMessage.
     * Actually, what we do is to discharge the diff generator into
     * the DiffSnapshot buffer and then replace the link to the original
     * generator to this decorator, which, when iterated, yields the
     * contents of the DiffSnapshot one by one. But since all DiffSteps
     * are now stored into that DiffSnapshot _buffer we control,_ we're
     * able to produce a diagnostic listing of the complete sequence.
     */
    class MaterialisedDiffMessageBuffer
      : private DiffSnapshot
      , public _Wrapped
      {
      public:
        MaterialisedDiffMessageBuffer(DiffMessage& srcMsg)
          : DiffSnapshot{srcMsg}
          , _Wrapped{_RangeIT{DiffSnapshot::begin(), DiffSnapshot::end()}}
          { }
        
        virtual
        operator string()  const override
          {
            return "Diff--{"+util::join (static_cast<DiffSnapshot> (*this))+"}";
          }
      };
  }
  
  /** @par operational semantics
   * Since the underlying generator of the DiffStep sequence is an iterator,
   * the "materialised view" can only capture what's left at the point when
   * updateDiagnostics() is invoked. The captured rest sequence seamlessly
   * becomes the new generator and the old generator object is released,
   * since the assignment of the new backend typically removes the last
   * reference in the smart-ptr managing the generation backend. This
   * process can be repeated and then the [diagnostics](operator string())
   * will show the remainder of the sequence _left at that point._
   */
  inline DiffMessage&
  DiffMessage::updateDiagnostics()
  {
    return *this = DiffMessage{new MaterialisedDiffMessageBuffer(*this)};
  }

  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_DIFF_MESSAGE_H*/
