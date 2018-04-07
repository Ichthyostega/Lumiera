/*
  TEST-ELEMENT-ACCESS.hpp  -  fake access directory to handle generic UI entities for test

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


/** @file test-element-access.hpp
 ** Generic building block in the Lumiera GUI model.
 ** A model::Element has a unique identifier, which is tied to the
 ** identification scheme used in the "real" model in Proc-Layer.
 ** Model elements can be addressed receive mutations caused by changes
 ** and rebuilding of elements within the Session; moreover, a generic
 ** representation of attributes is provided.
 ** 
 ** @note as of 1/2015 this is a first draft and WIP-WIP-WIP
 ** @todo WIP  ///////////////////////TICKET #1134
 ** 
 ** @see ////TODO_test usage example
 ** @see element.cpp implementation
 ** 
 */


#ifndef GUI_MODEL_TEST_ELEMENT_ACCESS_H
#define GUI_MODEL_TEST_ELEMENT_ACCESS_H


#include "lib/error.hpp"
#include "gui/model/element-access.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


  
namespace gui {
namespace model {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class TestElementAccess
    : public ElementAccess
    {
      
    public:
      explicit
      TestElementAccess ()
        { }
      
      
      
      
      /* == Adapter interface for == */
      
      
      
    protected:
    };
  
  
  
  
  
  
  /** @internal in case
   */
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1134
  template<class X>
  inline void
  ElementAccess<X>::maybe ()  const
  {
    UNIMPLEMENTED ("tbw");
  }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1134
  
  
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_TEST_ELEMENT_ACCESS_H*/
