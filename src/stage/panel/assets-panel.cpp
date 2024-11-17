/*
  assets-panel.cpp  -  Implementation of the assets panel

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file assets-panel.cpp
 ** Implementation of a (dockable) panel to organise the assets of a project.
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/assets-panel.hpp"

namespace stage {
namespace panel {
  
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
  
  
}}// namespace stage::panel
