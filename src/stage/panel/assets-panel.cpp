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


/** @file assets-panel.cpp
 ** Implementation of a (dockable) panel to organise the assets of a project.
 */

#include "gui/gtk-base.hpp"
#include "gui/panel/assets-panel.hpp"

namespace gui  {
namespace panel{
  
  AssetsPanel::AssetsPanel(workspace::PanelManager& panelManager, Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    {
      notebook_.append_page(media_, _("Media"));
      notebook_.append_page(clips_, _("Clips"));
      notebook_.append_page(effects_, _("Effects"));
      notebook_.append_page(transitions_, _("Transitions"));
      
      pack_start(notebook_);
    }
  
  const char*
  AssetsPanel::getTitle()
  {
    return _("Assets");
  }
  
  const gchar*
  AssetsPanel::getStockID()
  {
    return "panel_assets";
  }
  
  
}}// namespace gui::panel
