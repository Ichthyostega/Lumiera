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
 ** Message on the UI-Bus to cause changes on the targeted [UI-Element](\ref Tangible).
 ** The UI-Bus offers a dedicated API to direct MutationMessages towards Tangible elements,
 ** as designated by the given ID. Actually, such messages serve as capsule to transport a
 ** diff-sequence -- since a diff sequence as such is always concrete and tied to a specific context,
 ** we can not represent it directly as an abstract type on interface level. The receiver of a diff
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
 ** which is an iterable sequence of lib::diff::DiffStep records.
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP
 ** 
 ** @see [AbstractTangible_test]
 ** 
 */


#ifndef GUI_CTRL_MUTATION_MESSAGE_H
#define GUI_CTRL_MUTATION_MESSAGE_H


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/diff/diff-mutable.hpp"

//#include <boost/noncopyable.hpp>
//#include <utility>
#include <string>


namespace gui {
namespace model {
  class Tangible;
}
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
//  using lib::diff::GenNode;
  using std::string;
  
  
  /**
   * Message on the UI-Bus holding an embedded diff sequence.
   * The Nexus (hub of the UI-Bus) will prompt the designated Tangible
   * to expose a TreeMutator, and then apply the embedded diff.
   */
  class MutationMessage
    {
    protected:
      
    public:
      virtual ~MutationMessage();  ///< this is an interface
      
      virtual void
      applyTo (lib::diff::DiffMutable& target)
        {
          UNIMPLEMENTED("how to embed a diff sequence and apply this to the target");
        }
      
      operator string()  const
        {
          UNIMPLEMENTED("string representation of diff/mutation messages");
        }
      
    protected:
    };
  
  
    
  
  /**  */
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_MUTATION_MESSAGE_H*/
