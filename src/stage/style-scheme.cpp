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
 ** @see stage::workspace::UiStyle
 ** 
 */

#include "stage/gtk-base.hpp"
#include "stage/style-scheme.hpp"

namespace stage {
  
  const Literal TAG_ERROR{"ERROR"};              ////////////////////////////////////////////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
  const Literal TAG_WARN{"WARN"};
  
  /** CSS class added temporarily to make a widget "flash"
   *  in response to the corresponding UI-Bus message
   * @see flash-deco.hpp */
  cuString CLASS_indication_flash{"indication-flash"};
  const uint STYLE_FLASH_DURATION_in_ms = 1100;
  
  
  /* ======== CSS selector names ======== */
  
  const Symbol NODE_fork{"fork"};
  const Symbol NODE_frame{"frame"};
  
  cuString CLASS_timeline      {"timeline"};
  cuString CLASS_timeline_page {"timeline__page"};
  cuString CLASS_timeline_body {"timeline__body"};
  cuString CLASS_timeline_fork {"timeline__fork"};
  cuString CLASS_timeline_ruler{"timeline__ruler"};
  cuString CLASS_slope_deep1   {"track-slope--deep1"};
  cuString CLASS_slope_deep2   {"track-slope--deep2"};
  cuString CLASS_slope_deep3   {"track-slope--deep3"};
  cuString CLASS_slope_deep4   {"track-slope--deep4"};
  cuString CLASS_slope_verydeep{"track-slope--verydeep"};
  
  
  
}// namespace stage
