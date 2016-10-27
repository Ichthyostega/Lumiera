/*
  TimelinePanel  -  Dockable panel to hold the main timeline view

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file timeline-panel.cpp
 ** Implementation of dockable timeline container.
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo build a new timeline widget, connected to the UI-Bus
 ** @see timeline-widget.hpp
 ** 
 */


#include "gui/gtk-lumiera.hpp"
#include "gui/panel/timeline-panel.hpp"

//#include "gui/workspace/workspace-window.hpp"
#include "gui/ui-bus.hpp"

//#include "lib/util.hpp"



//using std::shared_ptr;
//using std::weak_ptr;
//using util::contains;
using sigc::mem_fun;


namespace gui {
namespace panel {
    
  
  
  TimelinePanel::TimelinePanel (workspace::PanelManager& panelManager,
                                Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , twoParts_(Gtk::ORIENTATION_VERTICAL)
    , buttons_()
    , button_1_()
    , button_2_()
    , frame_("Gtk::Layout Experiments")
    , scroller_()
    {
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      twoParts_.pack_start(frame_);
      
      buttons_.add(button_1_);
      buttons_.add(button_2_);
      buttons_.set_layout(Gtk::BUTTONBOX_START);
      
      button_1_.set_label("Experiment _1");
      button_1_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_1));
      
      button_2_.set_label("Experiment _2");
      button_2_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_2));
      
      frame_.add(scroller_);
      frame_.set_border_width(5);
      
      scroller_.set_shadow_type(Gtk::SHADOW_NONE);
      
      this->add(twoParts_);
      this->show_all();
    }
  
  const char*
  TimelinePanel::getTitle()
  {
    return _("Timeline");
  }
  
  const gchar*
  TimelinePanel::getStockID()
  {
    return "panel_timeline";
  }
  
  
  void
  TimelinePanel::experiment_1()
  {
    frame_.set_label("Experiment 1...");
  }
  
  
  void
  TimelinePanel::experiment_2()
  {
    frame_.set_label("Experiment 2...");
  }
  
  
  
}}   // namespace gui::panel
