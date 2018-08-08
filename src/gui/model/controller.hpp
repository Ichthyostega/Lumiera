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
 ** @todo as of 8/2018 we should consider to distill a common default implementation usable for most controllers.
 **       especially, there might be a default (NOP) implementation of the diff mutator builder
 ** 
 ** @see model::Tangible
 ** @see gui::UIBus
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_MODEL_CONTROLLER_H
#define GUI_MODEL_CONTROLLER_H


#include "lib/error.hpp"
#include "gui/model/tangible.hpp"
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
      
      /* ==== Tangible interface ==== */
      
      virtual bool
      doReset()  override
        {
          UNIMPLEMENTED ("Controller reset");
        }
      
      virtual bool
      doExpand (bool yes)  override
        {
          UNIMPLEMENTED ("Controller doExpand");
        }
      
      virtual void
      doReveal (ID child)  override
        {
          UNIMPLEMENTED ("Controller doReveal");
        }
      
      virtual void
      doRevealYourself()  override
        {
          UNIMPLEMENTED ("Controller doRevealYourself");
        }
      
      virtual bool
      doMsg (string text)  override
        {
          UNIMPLEMENTED ("Controller doMsg");
        }
      
      virtual bool
      doClearMsg ()  override
        {
          UNIMPLEMENTED ("Controller doClearMsg");
        }
      
      virtual bool
      doErr (string text)  override
        {
          UNIMPLEMENTED ("Controller doErr");
        }
      
      virtual bool
      doClearErr ()  override
        {
          UNIMPLEMENTED ("Controller doClearErr");
        }
      
      virtual void
      doFlash()  override
        {
          UNIMPLEMENTED ("Controller doFlash");
        }
      
      virtual void
      doMark (GenNode const& mark)  override
        {
          UNIMPLEMENTED ("Controller doMark");
        }
      
      
    public:
      using Tangible::Tangible;
      
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_CONTROLLER_H*/
