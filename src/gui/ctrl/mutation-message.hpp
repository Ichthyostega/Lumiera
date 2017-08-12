/*
  MUTATION-MESSAGE.hpp  -  message on UI-Bus to cause changes to tangible UI elements

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


/** @file mutation-message.hpp
 ** Message on the UI-Bus to cause changes on the targeted [UI-Element](\ref model::Tangible).
 ** The UI-Bus offers a dedicated API to direct MutationMessages towards Tangible elements,
 ** as designated by the given ID. Actually, such messages serve as capsule to transport a
 ** diff-sequence -- since a diff sequence as such is always concrete and tied to a specific context,
 ** we can not represent it easily as an abstract type on interface level. The receiver of a diff
 ** sequence must offer the ability to be reshaped through diff messages, which is expressed through
 ** the interface DiffMutable. In the case at question here, gui::model::Tangible offers this interface
 ** and thus the ability to construct a concrete lib::diff::TreeMutator, which in turn is bound to the
 ** internals of the actual UI-Element. Together this allows for a generic implementation of MutationMessage
 ** handling, where the designated UI-Element is reshaped by applying an embedded concrete diff message
 ** with the help of a `DiffApplicator<DiffMutable>`, based on the TreeMutator exposed.
 ** 
 ** ## Creating mutation messages
 ** The UI-Bus invocation actually takes a reference to MutationMessage, and thus on usage a
 ** concrete instance needs to be created. This concrete Message embeds an actual diff sequence,
 ** which is some iterable sequence of lib::diff::DiffStep records.
 ** @warning be sure to understand that the diff sequence is really moved away and then consumed.
 ** 
 ** @todo as of 1/2017 there is an unsolved problem how such messages can be passed from lower layers.
 **       A direct symptom is the dependency of this header on model::Tangible, which in turn requires
 **       `sigc::Trackable`. This is a challenging topic, since we need to hand over to the UI-Event Thread  /////////////////////////////////TICKET #1066 : Concept for passing Diff Messages
 ** 
 ** @see AbstractTangible_test
 ** 
 */


#ifndef GUI_CTRL_MUTATION_MESSAGE_H
#define GUI_CTRL_MUTATION_MESSAGE_H


#include "lib/error.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "gui/model/tangible.hpp"
#include "lib/format-util.hpp"

#include <utility>
#include <string>


namespace gui {
namespace ctrl{
  
  using std::string;
  
  namespace diff_msg { // implementation details for embedding concrete diff messages
    
    using lib::diff::DiffApplicator;
    using model::Tangible;
    using std::move;
    
    class Holder
      {
      public:
        virtual ~Holder() {}; ///< this is an interface
        virtual void applyTo (Tangible&)   =0;
        virtual string describe()  const   =0;
      };
    
    template<typename DIFF>
    class Wrapped
      : public Holder
      {
        DIFF diff_;
        
        virtual void
        applyTo (Tangible& target)  override
          {
            DiffApplicator<Tangible> applicator(target);
            applicator.consume (move(diff_));
          }
        
        virtual string
        describe()  const override
          {
            DIFF copy(diff_); // NOTE: we copy, since each iteration consumes.
            return ::util::join (move(copy));
          }
        
      public:
        Wrapped (DIFF&& diffSeq)
          : diff_(move(diffSeq))
          { }
      };
    
    
    /** standard size to reserve for the concrete diff representation
     * @note this is a pragmatic guess, based on the actual usage pattern within Lumiera.
     *       This determines the size of the inline buffer within MutationMessage.
     *       You'll get an static assertion failure when creating a MutationMessage
     *       from a concrete diff representation requires more storage space...
     */
    enum { SIZE_OF_DIFF_REPRESENTATION = sizeof(std::vector<string>)
                                       + sizeof(size_t)
                                       + sizeof(void*)
         };
    
    using Buffer = lib::InPlaceBuffer<Holder, SIZE_OF_DIFF_REPRESENTATION>;
  }//(End) implementation details...
  
  
  
  
  
  /**
   * Message on the UI-Bus holding an embedded diff sequence.
   * The Nexus (hub of the UI-Bus) will prompt the designated Tangible
   * to expose a TreeMutator, and then apply the embedded diff.
   */
  class MutationMessage
    : public diff_msg::Buffer
    {
    public:
      /** build a MutationMessage by _consuming_ the given diff sequence
       * @param diffSeq some iterable DiffStep sequence.
       * @warning parameter will be moved into the embedded buffer and consumed
       */
      template<typename DIFF>
      MutationMessage(DIFF&& diffSeq)
        : diff_msg::Buffer{ embedType<diff_msg::Wrapped<DIFF>>()
                          , std::move(diffSeq)}
        { }
      
      void
      applyTo (model::Tangible& target)
        {
          access<diff_msg::Holder>()->applyTo(target);
        }
      
      operator string()  const
        {
          return unConst(this)->access<diff_msg::Holder>()->describe();
        }
      
    protected:
    };
  
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_MUTATION_MESSAGE_H*/
