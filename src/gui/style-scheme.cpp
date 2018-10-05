/*
  StyleScheme  -  magic keys to access uniform styling scheme

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

* *****************************************************/


/** @file style-scheme.hpp
 ** Definition of access keys for uniform UI styling.
 ** 
 ** @see gui::workspace::UiStyle
 ** 
 */

#include "gui/gtk-base.hpp"
#include "gui/style-scheme.hpp"

namespace gui {
  
  const Literal TAG_ERROR{"ERROR"};              ////////////////////////////////////////////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
  const Literal TAG_WARN{"WARN"};
  
  cuString CSS_CLASS_FLASH{"indication_flash"};                      ///< CSS class added temporarily to make a widget "flash" in response to the corresponding UI-Bus message 
  const uint STYLE_FLASH_DURATION_in_ms = 1100;
  
  
  
}// namespace gui
