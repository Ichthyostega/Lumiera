/*
  SESSION-FACADE.hpp  -  service for

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

*/


/** @file session-facade.hpp
 ** Service for session-facade.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2014 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see session-facade.cpp implementation
 ** 
 */


#ifndef SESSION_FACADE_H
#define SESSION_FACADE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <string>


namespace stage {
namespace model {
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class SessionFacade
    : util::NonCopyable
    {
      string nothing_;
      
    public:
      SessionFacade();
     ~SessionFacade();
      
      
      static string beCreative ();
      
      /* == Adapter interface for == */
      
      
      
    protected:
      string maybe ()  const;
      
      void lock_to_TestContent();
      void releaseTestMode();
      
      friend class Diagnostics;
    };
  
  
  
}} // namespace stage::model
#endif /*SESSION_FACADE_H*/
