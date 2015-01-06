/*
  BUS-CONTROLLER.hpp  -  the UI-Bus service

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


/** @file bus-controller.hpp
 ** Service for bus-controller.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2015 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see bus-controller.cpp implementation
 ** 
 */


#ifndef GUI_CTRL_BUS_CONTROLLER_H
#define GUI_CTRL_BUS_CONTROLLER_H


#include "lib/error.hpp"
//#include "lib/hash-value.h"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>

namespace gui {
namespace ctrl{

  
  using util::isnil;
  using std::string;
  
  
  /**
   * Manage operation of the UI-Bus
   * 
   * @todo WIP 1/2015
   */
  class BusController
    : boost::noncopyable
    {
      string nothing_;
      
    public:
      explicit
      BusController (string const& b)
        : nothing_(b)
        { }
      
      
      
      /* == Adapter interface for == */
      
      void
      setSolution (string const& solution ="")
        {
          UNIMPLEMENTED ("tbw");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
          if (isDeaf())
            this->transmogrify (solution);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
        }
      
      
    protected:
      void maybe ()  const;
      
    };
  
  
  
  
  
  
  /** @internal in case
   */
  inline void
  BusController::maybe ()  const
  {
    UNIMPLEMENTED ("tbw");
  }
  
  
  
  
}} // namespace gui::ctrl  
#endif /*GUI_CTRL_BUS_CONTROLLER_H*/
