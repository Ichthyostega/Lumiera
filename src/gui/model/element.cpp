/*
  gui::model::Element  -  a generic element in the UI-Model

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


/** @file element.cpp
 ** Service for element.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2015 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see element.cpp implementation
 ** 
 */


#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "gui/model/element.hpp"

//#include <boost/noncopyable.hpp>
#include <string>
#include <map>

using std::map;
using std::string;

using util::contains;
using util::isnil;

namespace gui {
namespace model {
  
  namespace { // internal details
    
  } // internal details
  
  
  
  //NA::~NA() { }
  
  
  
  
  /**
   * 
   * @param id
   * @return
   */
  string
  fun (string& id)
  {
    return "x"+id;
  }

}} // namespace gui::model
