/*
  PanelBar  -  container to place widgets into the GDL dock attachment area

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file panel-bar.cpp
 ** Implementation of a custom container widget to place toolbar buttons
 ** into the active docking header area of GDL docking panels.
 */


#include "stage/widget/panel-bar.hpp"
#include "stage/workspace/workspace-window.hpp"
#include "stage/workspace/panel-manager.hpp"
#include "stage/panel/panel.hpp"
#include "stage/draw/cairo-util.hpp"
#include "include/logging.h"

#include <boost/foreach.hpp>
#include <algorithm>


namespace stage {
namespace widget {
  
  using sigc::slot;
  using sigc::bind;
  using sigc::mem_fun;
  using stage::workspace::PanelManager;
  
  
  PanelBar::PanelBar (panel::Panel& owner_panel, gchar const*stock_id)
    : Box()
    , panel_{owner_panel}
    , panelButton_{Gtk::StockID(stock_id)}              /////////////////////////////////////////////////////TICKET #1030 : Stock-Items must be replaced for GTK-4
    , lockItem_(NULL)
    {
      set_border_width(1);
      
      panelButton_.set_relief (Gtk::RELIEF_NONE);
      panelButton_.set_can_focus(false);
      panelButton_.show();
      pack_start (panelButton_, Gtk::PACK_SHRINK);
      
      setupPanelButton();
    }
  
  
  /**
   * Sets up panelButton, populating it with menu items.
   * @todo still not completely ported to GTK-3 and not used as of 5/2025   /////////////////////////////////TICKET #937 : complete Port to GTK-3
   */
  void
  PanelBar::setupPanelButton()
  {
    REQUIRE (lockItem_ == NULL);
    
    // Add items for each type of panel
    for (int i = 0; i < PanelManager::getPanelDescriptionCount(); i++)
      {
        uString title{uString(PanelManager::getPanelTitle(i))};
        uString slug {title};
        
        /* Slug should be a char only string, no spaces, numbers, or
         * symbols. Currently all panel titles are single words.
         * So the above works OK for now.
         */
        
        slot<void> func = bind(mem_fun(*this,
            &PanelBar::on_panel_type), i);
        
        panelButton_.append (slug, title, func);
        
     /* Keeping this snippet for easy reference                    //////////////////////////////////////////TICKET #937 : complete Port to GTK-3
      * list.push_back( Menu_Helpers::StockMenuElem(
      * StockID(PanelManager::get_panel_stock_id(i)),
      * bind(mem_fun(*this, &PanelBar::on_panel_type), i) ));
      */
      }
    
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #937 : complete Port to GTK-3
    list.push_back( Menu_Helpers::SeparatorElem() );
#endif
    
    // Add extra commands
    slot<void> hide = mem_fun(*this, &PanelBar::on_hide);
    panelButton_.append("Hide","_Hide", hide);
    
    slot<void> lock = mem_fun(*this, &PanelBar::on_lock);
    panelButton_.append ("Lock", "_Lock", lock);
    
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #937 : complete Port to GTK-3
    lockItem_ = dynamic_cast<CheckMenuItem*>(&list.back());
    ENSURE (lockItem_);
    lockItem_->set_active (panel_.is_locked());
#endif
    
    
    slot<void> hfunc = bind (mem_fun(*this, &PanelBar::on_split_panel), Gtk::ORIENTATION_HORIZONTAL);
    
    panelButton_.append("SplitHorizontal", "Split _Horizontal",hfunc);
    
    slot<void> vfunc = bind (mem_fun(*this, &PanelBar::on_split_panel), Gtk::ORIENTATION_VERTICAL);
    panelButton_.append("SplitVertical", "Split _Vertical", vfunc);
    
  }
  
  
      
  /**
   * An event handler for when a panel type is chosen.
   * @param type_index index of the panel description that will be instantiated.
   */
  void
  PanelBar::on_panel_type (int type_index)
  {
    panel_.getPanelManager().switchPanel (panel_, type_index);
  }
  
  /**
   * Event handler for when the split panel menu item is clicked
   * @param split_direction The direction to split in.
   */
  void
  PanelBar::on_split_panel (Gtk::Orientation split_direction)
  {
    panel_.getPanelManager().splitPanel (panel_, split_direction);
  }
  
  
  /** An event handler for when the "Hide" menu item is clicked */
  void
  PanelBar::on_hide()
  {
    panel_.show(false);
  }
  
  /** Event handler for when the "Lock" menu item is clicked */
  void
  PanelBar::on_lock()
  {
    REQUIRE(lockItem_);
    
    bool lockState{not panel_.is_locked()};
    panel_.lock (lockState);
    lockItem_->set_active(lockState);
  }
  
  
  
}}// stage::widget
