/*
  TANGIBLE.hpp  -  a tangible element of the user interface

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


/** @file tangible.hpp
 ** Abstraction: a tangible element of the User Interface.
 ** Any such element is connected to the UIBus...
 ** 
 ** @todo as of 1/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_MODEL_TANGIBLE_H_
#define GUI_MODEL_TANGIBLE_H_


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
namespace model {
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * @todo write type comment...
   */
  class Tangible
    : boost::noncopyable
    {
      string nothing_;
      
    public:
      Tangible();
     ~Tangible();
      
    protected:
      string maybe ()  const;
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_TANGIBLE_H_*/
