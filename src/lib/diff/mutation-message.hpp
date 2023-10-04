/*
  MUTATION-MESSAGE.hpp  -  message to cause changes to generic model elements

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


/** @file mutation-message.hpp
 ** Generic Message with an embedded diff, to describe changes to model elements.
 ** The ability to create and apply such messages relies on the [diff framework](\ref diff-language.hpp).
 ** Using diff messages allows to describe and effect changes, without actually knowing much about the target.
 ** Sender and receiver just need to share some common assumptions about the abstract structure of the data.
 ** 
 ** The challenging part with this task is the fact that we need to pass such messages over abstraction barriers
 ** and even schedule them into another thread (the UI event thread). Yet diff application actually is a _pull operation,_
 ** which means there must be a callback actually to retrieve the diff content, and this callback will happen from
 ** the context of the receiver.
 ** 
 ** # Mutation messages on the UI-Bus
 ** 
 ** The UI-Bus offers a dedicated API to direct MutationMessages towards [UI-Elements](\ref model::Tangible).
 ** Each _tangible element in the UI,_ be it Widget or Controller, is designated by an unique ID. Sending a
 ** Mutation message causes the target to alter and reshape itself, to comply to the _diff sequence_ indicated
 ** and transported through the message -- since a diff sequence as such is always concrete and relates to a
 ** a specific context, we can not represent it directly as a type on interface level. Rather, the receiver
 ** of a diff sequence must offer the ability to be reshaped through diff messages, which is expressed through
 ** the interface DiffMutable. In the case at question here, stage::model::Tangible offers this interface and thus
 ** the ability to construct a concrete lib::diff::TreeMutator, which in turn is bound to the internals of the
 ** actual UI-Element. In this framework, a diff is actually represented as a sequence of _diff verbs,_ which
 ** can be ``pulled'' one by one from the MutationMessage, and then applied to the target data structure
 ** with the help of a `DiffApplicator<DiffMutable>`, based on the TreeMutator exposed.
 ** 
 ** # Mutation messages sent from the Session into the UI
 ** 
 ** While components in the UI generate commands to work on the session, the effect of performing those commands
 ** is reflected back asynchronously into the GUI through MutationMessages. All _visible content in the UI_ is
 ** controlled by such messages. Initially the UI is a blank slate, and will be populated with content to reflect
 ** the content and structure of the session. Whenever the session changes, an incremental update is pushed into
 ** the UI as a diff.
 ** 
 ** Hand-over and application of mutations is actually a process in two steps. The necessity to change something
 ** is indicated (or ``triggered'') by passing a MutationMessage through the GuiNotification::facade. We should
 ** note at this point that Session and UI perform each within a dedicated single thread (contrast this to the
 ** player and render engine, which are inherently multithreaded). The UI is loaded as plug-in and opens the
 ** GuiNotification::facade when the event loop is started. Thus _initiating_ the mutation process is a simple
 ** invocation from the session thread, which enqueues the MutationMessage and schedules the trigger over into
 ** the UI event thread. This starts the second stage of diff application: when the UI is about to process
 ** this event, the MutationMessage (which was passed through a dispatcher queue) will be forwarded over the
 ** UI-Bus to reach the designated target object. On reception, the receiving UI-Element builds and exposes
 ** its TreeMutator and the starts to _pull_ the individual DiffStep entries from the MutationMessage.
 ** But in fact those entries aren't stored within the message, rather a callback is invoked. When initially
 ** creating the message, an _opaque generation context_ was established, which now receives those callbacks
 ** and generates the actual sequence of diff verbs, which are immediately passed on to through the DiffApplicator
 ** and the TreeMutator to effect the corresponding changes in the target data structure within the UI. Care has
 ** to be taken when referring to session data at that point, since the pull happens from within the UI thread;
 ** yet in the end this remains an opaque implementation detail within the session.
 ** 
 ** ## Creation of mutation messages
 ** 
 ** The standard case is to build a MutationMessage by passing a heap allocated generator object. This DiffSource
 ** object needs to implement the interface lib::IterSource<DiffStep>, with callbacks to generate the initial
 ** step and further steps. Incidentally, the MutationMessage takes ownership and manages the DiffSource generator.
 ** Beyond this standard case, MutationMessage offers several convenience constructors to produce simple diff
 ** messages with a predetermined fixes sequence of DiffStep entries.
 ** 
 ** @see MutationMessage_test
 ** @see AbstractTangible_test
 ** @see BustTerm_test::pushDiff()
 ** @see BusTerm::change()
 ** 
 */


#ifndef LIB_DIFF_MUTATION_MESSAGE_H
#define LIB_DIFF_MUTATION_MESSAGE_H


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
   * Sending such messages typically allows some _implementation defined_ part within
   * the Session to communicate structure and content to some other _implementation defined_
   * part within the UI-Layer, without the necessity of both partners to be tightly coupled on
   * implementation level or even know much about the other's implementation details. As motivation,
   * contrast this to a naive UI implementation, which directly accesses some backend data structure;
   * any change to the backend implementation typically affects the UI implementation on a detail level.
   * @warning be sure to understand the fundamental problem of diff generation and application:
   *       the production context of diff messages need to be conserved beyond the producer's
   *       thread context, because it will be pulled asynchronous from within the UI event thread!
   */
  struct MutationMessage
    : DiffSource::iterator
    {
      using _FrontEnd = DiffSource::iterator;
      
      MutationMessage()                                   = default;
      MutationMessage(MutationMessage&&)                  = default;
      MutationMessage(MutationMessage const&)             = default;
      MutationMessage(MutationMessage& o)  : MutationMessage((MutationMessage const&)o) { }
      MutationMessage& operator= (MutationMessage const&) = default;
      MutationMessage& operator= (MutationMessage &&)     = default;
                                                            ////////////////////////TICKET #963  Forwarding shadows copy operations
      
      /**
       * MutationMessage builder:
       * take ownership of an opaque heap allocated context
       * from which the concrete diff can be pulled on demand 
       */
      explicit
      MutationMessage(DiffSource* diffGenerationContext)
        : _FrontEnd{DiffSource::build (diffGenerationContext)}
        { }
      
      /**
       * Convenience builder for consuming an brace enclosed initializer_list
       * @note initialiser elements will be _copied_ into a _heap allocated_
       *       snapshot (vector), which is then managed by `shared_ptr`
       */
      MutationMessage(std::initializer_list<DiffStep> const&& ili)
        : MutationMessage{iter_stl::snapshot (move(ili))}
        { }
      
      /**
       * Convenience builder to take an arbitrary number of DiffStep arguments
       * @note like for the initializer_list, arguments will be copied into
       *       a _heap allocated snapshot_
       */
      template<typename...ARGS>
      MutationMessage(ARGS&& ...args)
        : MutationMessage{ {std::forward<ARGS>(args)...} }
        { }
      
      /**
       * Convenience builder to piggyback any Lumiera Forward Iterator
       * @note source iterator is copied into a heap allocated IterSource
       */
      template<class IT>
      MutationMessage(IT&& ii,                        enable_if< can_IterForEach<IT>, void*> =nullptr)
        : _FrontEnd{iter_source::wrapIter (std::forward<IT>(ii))}
        { }
      
      /**
       * Convenience builder to use elements form any STL-like container
       * @note creating heap allocated IterSource, which _refers_ to the original container
       * @warning like with any classical iterators, the container must stay alive and accessible 
       */
      template<class CON>
      MutationMessage(CON& container,                 enable_if< __and_< can_STL_ForEach<CON>
                                                                       ,__not_<can_IterForEach<CON>>>, void*> =nullptr)
        : _FrontEnd{iter_source::eachEntry(container)}
        { }
      
      
      /**
       * enable support to show content of the message.
       * After calling this function, operator string() renders all DiffSteps
       * @warning since by design a MutationMessage can only be ``pulled'' once,
       *          this operation needs to impose a _side effect:_ it materialises
       *          the complete diff sequence at once into a heap allocated buffer.
       */
      MutationMessage& updateDiagnostics();
    };
  
  
  
  namespace { // Implementation: take snapshot to enable diagnostics
    
    /** "materialised view" of the diff sequence */
    struct DiffSnapshot
      : std::vector<DiffStep>
      {
        DiffSnapshot(MutationMessage& srcMsg)
          {
            for ( ; srcMsg; ++srcMsg )
              push_back (*srcMsg);
          }
      };
    
    using _VecIter = DiffSnapshot::iterator;
    using _RangeIT = RangeIter<_VecIter>;
    using _Wrapped = WrappedLumieraIter<_RangeIT>;
    
    /**
     * Decorator to be layered transparently on top of MutationMessage.
     * Actually, what we do is to discharge the diff generator into
     * the DiffSnapshot buffer and then replace the link to the original
     * generator to this decorator, which, when iterated, yields the
     * contents of the DiffSnapshot one by one. But since all DiffSteps
     * are now stored into that DiffSnapshot _buffer we control,_ we're
     * able to produce a diagnostic listing of the complete sequence.
     */
    class MaterialisedDiffBuffer
      : private DiffSnapshot
      , public _Wrapped
      {
      public:
        MaterialisedDiffBuffer(MutationMessage& srcMsg)
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
   * process can be repeated and then the [diagnostics](\ref operator string())
   * will show the remainder of the sequence _left at that point._
   */
  inline MutationMessage&
  MutationMessage::updateDiagnostics()
  {
    return *this = MutationMessage{new MaterialisedDiffBuffer(*this)};
  }

  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_MUTATION_MESSAGE_H*/
