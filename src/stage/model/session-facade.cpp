/*
  SessionFacade  -  service for

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session-facade.cpp
 ** Service for session-facade.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2014 this is complete bs
 ** @todo THE NAME IS UNFORTUNATE!!!! "SessionFacade" should rather be a facade in Steam!
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see session-facade.cpp implementation
 ** 
 */


//#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "stage/model/session-facade.hpp"
#include "lib/depend.hpp"

//#include <string>
//#include <map>

//using std::map;
//using std::string;

//using util::contains;
//using util::isnil;


namespace stage {
namespace model {
  
  namespace { // internal details
    lib::Depend<SessionFacade> guiSessionFacade;
    
  } // internal details
  
  
  
  SessionFacade::SessionFacade()
    : nothing_("boo")
    { }
  
  
  SessionFacade::~SessionFacade() { }
  
  
  
  
  /**
   * 
   * @param id
   * @return
   */
  string
  SessionFacade::beCreative ()
  {
    return guiSessionFacade().maybe();
  }
  
  
  string
  SessionFacade::maybe()  const
  {
    return nothing_;
  }
  
  
  
}} // namespace stage::model
