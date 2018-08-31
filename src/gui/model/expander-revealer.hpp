/*
  EXPANDER-REVEALER.hpp  -  functor components for standard UI element actions

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file expander-revealer.hpp
 ** Helper components to implement some standard UI-element actions by installing a functor.
 ** 
 ** ## Interactions
 ** 
 ** @see [UI-Element protocol](\ref tangible.hpp)
 ** @see \ref BusTerm_test
 ** 
 */


#ifndef GUI_MODEL_EXPANDER_REVEALER_H
#define GUI_MODEL_EXPANDER_REVEALER_H


#include "lib/error.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"

#include <functional>
//#include <utility>
//#include <string>


namespace gui {
namespace model {
  
//  using std::string;
//  using lib::Symbol;
  
  
  /**
   * Functor component to support the default implementation of expanding/collapsing.
   */
  class Expander
    {
    public:
    protected:
      
      
      Expander()
        { }
      
    public:
      
    protected:
    };
  
  
  /**
   * Functor component to support the default implementation of revealing an UI-Element.
   */
  class Revealer
    {
    public:
    protected:
      
      
      Revealer()
        { }
      
    public:
      
    protected:
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_EXPANDER_REVEALER_H*/
