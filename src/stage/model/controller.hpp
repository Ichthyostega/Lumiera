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
 ** @see stage::UIBus
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_MODEL_CONTROLLER_H
#define STAGE_MODEL_CONTROLLER_H


#include "lib/error.hpp"
#include "stage/model/tangible.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace stage {
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
      
    protected:
      /** default handler for all generic mark messages. Forwards to Tangible::doMark
       * @todo is there any default implementation for special messages,
       *       which might be eligible as a base class implementation??
       */
      virtual void
      doMark (GenNode const& mark)  override
        {
          // forward to default handler
          Tangible::doMark (mark);
        }
      
      
    public:
      using Tangible::Tangible;
      
    };
  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_CONTROLLER_H*/
