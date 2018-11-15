/*
  STYLE_SCHEME.hpp  -  magic keys to access uniform styling scheme

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


/** @file style-scheme.hpp
 ** Hard wired key constants to access a global uniform styling scheme.
 ** 
 ** @todo as or 10/2018 this is mere placeholder code. The StyleManager needs rework
 ** 
 ** @see ui-style.hpp
 ** 
 */

#ifndef GUI_STYLE_SCHEME_H
#define GUI_STYLE_SCHEME_H


#include "lib/symbol.hpp"


namespace Glib {
  class ustring;
}
namespace gui {
  
  using lib::Literal;
  using cuString = const Glib::ustring;
  
  
  extern const Literal TAG_ERROR;                    ////////////////////////////////////////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
  extern const Literal TAG_WARN;
  
  extern cuString CSS_CLASS_FLASH;
  extern const uint STYLE_FLASH_DURATION_in_ms;
  
  
  
}// namespace gui
#endif /*GUI_STYLE_SCHEME_H*/
