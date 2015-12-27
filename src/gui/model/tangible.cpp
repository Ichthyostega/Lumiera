/*
  Tangible  -  common implementation base of all relevant interface elements

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

* *****************************************************/


/** @file tangible.cpp
 ** Common base implementation of all tangible and connected interface elements.
 ** 
 ** @see abstract-tangible-test.cpp
 ** @see [explanation of the fundamental interactions](tangible.hpp)
 ** 
 */


#include "gui/model/tangible.hpp"
#include "gui/model/widget.hpp"
#include "gui/model/controller.hpp"


namespace gui {
namespace model {
  
  
  
  Tangible::~Tangible() { }  // Emit VTables here...
  
  
  
  /** invoke the generic reset hook
   * @note the actual subclass has to override the doReset() hook
   *       to perform the actual clean-up work.
   * @remarks the intention is that, after invoking reset(), the
   *       interface element or controller is in pristine (presentation) state
   */
  void
  Tangible::reset()
    {
      this->doReset();
    }
  
  
  /**
   * Prepare a command or action for actual invocation, once the execution context
   * has been established. The action is not executed right away, but it is now ready
   * and bound to the concrete arguments supplied with the [record](\ref lib::diff::Rec).
   * @param prototype handle to a command instantiation, to be readied for invocation
   * @param arguments suitable tuple of values, to be used to outfit the prototype
   */
  void
  Tangible::prepareCommand (Cmd const& prototype, Rec&& arguments)
  {
    uiBus_.act (prototype.bind (std::forward<Rec>(arguments)));
  }
  
  
  /**
   * Actually trigger execution of an action or command.
   * @param preparedAction handle pointing to a command definition,
   *        which needs to be outfitted with arguments and ready for invocation.
   */
  void
  Tangible::issueCommand (Cmd const& preparedAction)
  {
    uiBus_.act (preparedAction.bang());
  }
  
  
  /**
   * Expand this element and remember the expanded state.
   * This is a generic Slot to connect UI signals against.
   * @note The concrete Widget or Controller has to override the
   *       ::doExpand() extension point to provide the actual UI
   *       behaviour. If this virtual method returns `true`, the
   *       state change is deemed relevant and persistent, and
   *       thus a "state mark" is sent on the UI-Bus.
   */
  void
  Tangible::slotExpand()
  {
    if (this->doExpand(true))
      uiBus_.note (GenNode("expand", true));
  }
  
  
  /**
   * Collapse or minimise this element and remember the collapsed state.
   * This is a generic Slot to connect UI signals against.
   */
  void
  Tangible::slotCollapse()
  {
    if (this->doExpand(false))
      uiBus_.note (GenNode("expand", false));
  }
  
  
  /**
   * @todo 12/2015 not clear yet what needs to be done
   * @remarks the intention is to request the given child
   *          to be brought into sight. We need to set up some kind
   *          of children registration, but better not do this in
   *          a completely generic fashion, for danger of overengineering.
   *          Moreover, it is not clear yet, who will issue this request
   *          and at which element the initial request can/will be targeted.
   */
  void
  Tangible::slotReveal(ID child)
  {
    this->doReveal(child);
    this->doRevealYourself();
  }
  
  
  
}} // namespace gui::model
