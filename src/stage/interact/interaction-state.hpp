/*
  INTERACTION-STATE.hpp  -  facility to watch and guide one aspect of UI interaction

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

*/


/** @file interaction-state.hpp
 ** Abstraction: a component to watch, maintain and guide UI state.
 ** Typically, each such component is focused on one specific aspect
 ** of complex interface interactions. An example would be a component
 ** to track gestures for trimming edits.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_INTERACTION_STATE_H
#define GUI_INTERACT_INTERACTION_STATE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <string>


namespace gui {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * Abstract foundation of UI state tracking components.
   * @todo write type comment...
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class InteractionState
    : util::NonCopyable
    {
    protected:
      
    public:
      
    private:
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_INTERACTION_STATE_H*/
