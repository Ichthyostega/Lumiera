/*
  assets-panel.cpp  -  Implementation of the assets panel
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include "assets-panel.hpp"

namespace lumiera {
namespace gui {
namespace panels {

AssetsPanel::AssetsPanel() :
  Panel("assets", _("Assets"), "assets_panel"),
  placeholder("Placeholder label. Is this supposed to be titled assets\nas in the proc layer? or resources\nas in cinelerra?")
  {
    pack_start(placeholder);
  }

}   // namespace panels
}   // namespace gui
}   // namespace lumiera
