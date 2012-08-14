/*
  panel-bar.hpp  -  Declaration of the panel bar widget

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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

#include "gui/widgets/panel-bar.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/workspace/panel-manager.hpp"
#include "gui/panels/panel.hpp"
#include "gui/util/rectangle.hpp"
#include "include/logging.h"

#include <boost/foreach.hpp>
#include <algorithm>

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace std;
using namespace gui::workspace;

namespace gui {
namespace widgets {

PanelBar::PanelBar(panels::Panel &owner_panel, const gchar *stock_id) :
  Box(),
  panel(owner_panel),
  panelButton(StockID(stock_id)),
  lockItem(NULL)
{
  set_border_width(1);
  
  panelButton.set_relief(RELIEF_NONE);
  panelButton.set_can_focus(false);
  panelButton.show();
  pack_start(panelButton, PACK_SHRINK);
  
  setup_panel_button();
}

void
PanelBar::setup_panel_button()
{
#if 0
  REQUIRE(lockItem == NULL);
  
  Menu& menu = panelButton.get_menu();
  MenuList& list = menu.items();
  
  // Add items for each type of panel
  for(int i = 0; i < PanelManager::get_panel_description_count(); i++)
    {
      list.push_back( Menu_Helpers::StockMenuElem(
        StockID(PanelManager::get_panel_stock_id(i)),
        bind(mem_fun(*this, &PanelBar::on_panel_type), i) ));
    }

  list.push_back( Menu_Helpers::SeparatorElem() );

  // Add extra commands
  list.push_back( Menu_Helpers::MenuElem(_("_Hide"),
    mem_fun(*this, &PanelBar::on_hide) ) );

  list.push_back( Menu_Helpers::CheckMenuElem(_("_Lock"),
    mem_fun(*this, &PanelBar::on_lock) ) );
  lockItem = dynamic_cast<CheckMenuItem*>(&list.back());
  ENSURE(lockItem);
  lockItem->set_active(panel.is_locked());
  
  list.push_back( Menu_Helpers::MenuElem(_("Split _Horizontal"),
    bind(mem_fun(*this, &PanelBar::on_split_panel),
      ORIENTATION_HORIZONTAL) ) );
  list.push_back( Menu_Helpers::MenuElem(_("Split _Vertical"),
    bind(mem_fun(*this, &PanelBar::on_split_panel),
      ORIENTATION_VERTICAL) ) );
#endif
}

void
PanelBar::on_panel_type(int type_index)
{  
  panel.get_panel_manager().switch_panel(panel, type_index);
}

void
PanelBar::on_hide()
{
  panel.show(false);
}

void
PanelBar::on_lock()
{
  static bool is_locking = false;
 
  REQUIRE(lockItem);
  
  if(!is_locking)
    {
      is_locking = true;
      
      const bool lock = !panel.is_locked();
      panel.lock(lock);
      lockItem->set_active(lock);
      
      is_locking = false;
    }
}

void
PanelBar::on_split_panel(Gtk::Orientation split_direction)
{
  panel.get_panel_manager().split_panel(panel, split_direction);
}

} // widgets
} // gui
