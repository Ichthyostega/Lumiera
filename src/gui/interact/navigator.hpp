/*
  NAVIGATOR.hpp  -  controller for global navigation through interface space

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


/** @file navigator.hpp
 ** Global interface navigation control.
 ** Beyond just clicking on some buttons or issuing menu / key commands,
 ** the Lumiera interface allows the user to _move_ through interface space,
 ** to leave one _"place"_ (WorkSite) and enter another. The interact::Navigator
 ** is a controller entity to implement the necessary mechanics for this navication.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_INTERACT_NAVIGATOR_H
#define GUI_INTERACT_NAVIGATOR_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  class SpotLocator;
  
  
  
  /**
   * Global cross-cutting navigation in interface space
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class Navigator
    : boost::noncopyable
    {
      SpotLocator& spotLocator_;
      
    public:
      Navigator (SpotLocator&);
     ~Navigator ();
      
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_NAVIGATOR_H*/
