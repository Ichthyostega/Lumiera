/*
  assets-panel.cpp  -  Implementation of the assets panel

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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

#include "gui/gtk-lumiera.hpp"
#include "gui/panels/assets-panel.hpp"

namespace gui {
namespace panels {

AssetsPanel::AssetsPanel(workspace::PanelManager &panel_manager,
    Gdl::DockItem &dock_item) :
  Panel(panel_manager, dock_item, get_title(), get_stock_id())
{
  notebook.append_page(media, _("Media"));
  notebook.append_page(clips, _("Clips"));
  notebook.append_page(effects, _("Effects"));
  notebook.append_page(transitions, _("Transitions"));

  pack_start(notebook);
}

const char*
AssetsPanel::get_title()
{
  return _("Assets");
}

const gchar*
AssetsPanel::get_stock_id()
{
  return "panel_assets";
}

}   // namespace panels
}   // namespace gui
