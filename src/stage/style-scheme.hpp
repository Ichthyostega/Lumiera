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

#ifndef STAGE_STYLE_SCHEME_H
#define STAGE_STYLE_SCHEME_H


#include "lib/symbol.hpp"


namespace Glib {
  class ustring;
}
namespace stage {
  
  using lib::Literal;  ///////TODO clarify if we want to use Symbol (interned string constant) here.
  using lib::Symbol;
  using cuString = const Glib::ustring;
  
  
  extern const Literal TAG_ERROR;                    ////////////////////////////////////////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
  extern const Literal TAG_WARN;
  
  extern cuString CLASS_indication_flash;
  extern const uint STYLE_FLASH_DURATION_in_ms;
  
  
  /* ======== CSS selector names ======== */
  
  extern const Symbol NODE_fork;
  extern const Symbol NODE_frame;
  
  extern cuString ID_element;
  extern cuString ID_idlabel;
  
  extern cuString CLASS_idlabel;
  extern cuString CLASS_idlabel_icon;
  extern cuString CLASS_idlabel_menu;
  extern cuString CLASS_idlabel_name;
  extern cuString CLASS_elementbox;
  extern cuString CLASS_elementbox_idlabel;
  extern cuString CLASS_elementbox_video;
  extern cuString CLASS_elementbox_audio;
  extern cuString CLASS_elementbox_text;
  extern cuString CLASS_elementbox_auto;
  extern cuString CLASS_elementbox_event;
  extern cuString CLASS_elementbox_effect;
  extern cuString CLASS_elementbox_label;
  extern cuString CLASS_elementbox_ruler;
  extern cuString CLASS_elementbox_group;
  extern cuString CLASS_elementbox_meta;
  
  extern cuString CLASS_timeline;
  extern cuString CLASS_timeline_page;
  extern cuString CLASS_timeline_body;
  extern cuString CLASS_timeline_fork;
  extern cuString CLASS_timeline_ruler;
  extern cuString CLASS_slope_deep1;
  extern cuString CLASS_slope_deep2;
  extern cuString CLASS_slope_deep3;
  extern cuString CLASS_slope_deep4;
  extern cuString CLASS_slope_verydeep;
  
  extern cuString CLASS_background;
  
  
}// namespace stage
#endif /*STAGE_STYLE_SCHEME_H*/
