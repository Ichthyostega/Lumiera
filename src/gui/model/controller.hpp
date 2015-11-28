/*
  CONTROLLER.hpp  -  generic interface of a sub-controller

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


/** @file controller.hpp
 ** Common Abstraction of all sub-controller, coordinated by the UI-Bus.
 ** 
 ** @todo as of 1/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see model::Tangible
 ** @see gui::UIBus
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_MODEL_CONTROLLER_H
#define GUI_MODEL_CONTROLLER_H


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace gui {
namespace model {
  
//  using lib::HashVal;
//  using util::isnil;
//  using std::string;
  
  
  /**
   * @todo write type comment...
   */
  class Controller
    : public Tangible
    {
      
    public:
      Controller();
     ~Controller();
      
    protected:
      string maybe ()  const;
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_CONTROLLER_H*/
