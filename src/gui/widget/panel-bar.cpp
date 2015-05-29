/*
  PanelBar  -  container to place widgets into the GDL dock attachment area

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


#include "gui/widget/panel-bar.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/workspace/panel-manager.hpp"
#include "gui/panel/panel.hpp"
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
namespace widget {
  
  
  PanelBar::PanelBar (panel::Panel& owner_panel, const gchar *stock_id)
    : Box()
    , panel_(owner_panel)
    , panelButton_(StockID(stock_id))
    , lockItem_(NULL)
    {
      set_border_width(1);
      
      panelButton_.set_relief(RELIEF_NONE);
      panelButton_.set_can_focus(false);
      panelButton_.show();
      pack_start(panelButton_, PACK_SHRINK);
      
      setupPanelButton();
    }
  
  
  void
  PanelBar::setupPanelButton()
  {
    REQUIRE(lockItem_ == NULL);
    
    // Add items for each type of panel
    for (int i = 0; i < PanelManager::getPanelDescriptionCount(); i++)
      {
        uString title = uString(PanelManager::getPanelTitle(i));
        uString slug (title);
        
        /* Slug should be a char only string, no spaces, numbers, or
         * symbols. Currently all panel titles are single words.
         * So the above works OK for now.
         */
        
        slot<void> func = bind(mem_fun(*this,
            &PanelBar::on_panel_type), i);
        
        panelButton_.append (slug, title, func);
        
        /** Keeping this snippet for easy reference
        * list.push_back( Menu_Helpers::StockMenuElem(
        * StockID(PanelManager::get_panel_stock_id(i)),
        * bind(mem_fun(*this, &PanelBar::on_panel_type), i) ));
        **/
      }
    
#if false  //////////////////////////////////////////////////////////////////TODO Port to GTK-3
    list.push_back( Menu_Helpers::SeparatorElem() );
#endif
    
    // Add extra commands
    slot<void> hide = mem_fun(*this, &PanelBar::on_hide);
    panelButton_.append("Hide","_Hide", hide);
    
    slot<void> lock = mem_fun(*this, &PanelBar::on_lock);
    panelButton_.append ("Lock", "_Lock", lock);
    
#if false  //////////////////////////////////////////////////////////////////TODO Port to GTK-3
    lockItem_ = dynamic_cast<CheckMenuItem*>(&list.back());
    ENSURE (lockItem_);
    lockItem_->set_active (panel_.is_locked());
#endif
    
    
    slot<void> hfunc = bind(mem_fun(*this, &PanelBar::on_split_panel),
                            ORIENTATION_HORIZONTAL);
    
    panelButton_.append("SplitHorizontal", "Split _Horizontal",hfunc);
    
    slot<void> vfunc = bind(mem_fun(*this, &PanelBar::on_split_panel),
                            ORIENTATION_VERTICAL);
    panelButton_.append("SplitVertical", "Split _Vertical", vfunc);
    
  }
  
  
  void
  PanelBar::on_panel_type (int type_index)
  {
    panel_.getPanelManager().switchPanel (panel_, type_index);
  }
  
  
  void
  PanelBar::on_hide()
  {
    panel_.show(false);
  }
  
  
  void
  PanelBar::on_lock()
  {
    REQUIRE(lockItem_);
    static bool is_locking = false;
    
    if (!is_locking)
      {
        is_locking = true;
        
        const bool lock = !panel_.is_locked();
        panel_.lock(lock);
        lockItem_->set_active(lock);
        
        is_locking = false;
      }
  }
  
  
  void
  PanelBar::on_split_panel (Gtk::Orientation split_direction)
  {
    panel_.getPanelManager().splitPanel (panel_, split_direction);
  }
  
  
}}// gui::widget
