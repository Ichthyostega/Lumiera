/*
  SPOT-LOCATOR.hpp  -  conduct and direct a global centre of activity

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file spot-locator.hpp
 ** Navigating a centre of user activity globally.
 ** When the user works with the application, we conceive this activity to be happening
 ** _"somewhere",_ at a _location_ within user interface space. We call this place where the
 ** user "is" and does its work *the Spot* -- to be seen both as a location and some part
 ** of the interface lit up by a spotlight beam. The SpotLocator implements the technical
 ** gear necessary to move this spot from one place in the interface to another.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_INTERACT_SPOT_LOCATOR_H
#define GUI_INTERACT_SPOT_LOCATOR_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * Control and navigate the global centre of activity
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class SpotLocator
    : util::NonCopyable
    {
      
    public:
      SpotLocator();
     ~SpotLocator();
      
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_SPOT_LOCATOR_H*/
