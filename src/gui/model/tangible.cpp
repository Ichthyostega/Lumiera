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
 ** 
 */


//#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "gui/model/tangible.hpp"
#include "gui/model/widget.hpp"
#include "gui/model/controller.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <map>

//using std::map;
//using std::string;

//using util::contains;
//using util::isnil;

namespace gui {
namespace model {
  
  namespace { // internal details
    
  } // internal details
  
  
  
  Tangible::~Tangible() { }  // Emit VTables here...
  
  
  
  /** */
  void
  Tangible::reset()
    {
      this->doReset();
    }
  
  
  /**
   * Prepare a command or action for actual invocation, once the execution context
   * has been established. The action is not executed right away, but it is now ready
   * and bound to the concrete arguments supplied with the [record][lib::diff::Rec].
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
   */
  void
  Tangible::slotExpand()
  {
    this->doExpand(true);
    uiBus_.note (GenNode("expand", true));
  }
  
  
  /**
   * Collapse or minimise this element and remember the collapsed state.
   * This is a generic Slot to connect UI signals against.
   */
  void
  Tangible::slotCollapse()
  {
    this->doExpand(false);
    uiBus_.note (GenNode("expand", false));
  }
  
  
  /**
   * @todo not clear yet what needs to be done
   */
  void
  Tangible::slotReveal(ID child)
  {
    this->doReveal(child);
    this->doRevealYourself();
  }
  
  
  
}} // namespace gui::model
