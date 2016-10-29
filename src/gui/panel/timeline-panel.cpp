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
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"

//#include "lib/util.hpp"
#include <algorithm>
#include <cstdlib>



using util::_Fmt;
//using std::shared_ptr;
//using std::weak_ptr;
//using util::contains;
using Gtk::Widget;
using sigc::mem_fun;
using sigc::ptr_fun;
using std::cout;
using std::endl;
using std::rand;
using std::max;


namespace gui {
namespace panel {
    
  
  
  TimelinePanel::TimelinePanel (workspace::PanelManager& panelManager,
                                Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , twoParts_(Gtk::ORIENTATION_VERTICAL)
    , buttons_()
    , button_1_()
    , button_2_()
    , button_3_()
    , frame_("Gtk::Layout Experiments")
    , scroller_()
    , canvas_()
    {
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      twoParts_.pack_start(frame_);
      
      buttons_.set_layout(Gtk::BUTTONBOX_START);
      
      // buttons to trigger experiments
      button_1_.set_label("Experiment _1");
      button_1_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_1));
      buttons_.add(button_1_);
      
      button_2_.set_label("Experiment _2");
      button_2_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_2));
      buttons_.add(button_2_);
      
      button_3_.set_label("Experiment _3");
      button_3_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_3));
      buttons_.add(button_3_);
      
      button_4_.set_label("Experiment _4");
      button_4_.signal_clicked().connect(
                  mem_fun(*this, &TimelinePanel::experiment_4));
      buttons_.add(button_4_);
      //(End)buttons...
      
      frame_.add(scroller_);
      frame_.set_border_width(5);
      
      scroller_.set_shadow_type(Gtk::SHADOW_NONE);
      scroller_.set_border_width(10);
      scroller_.add(canvas_);
      
      canvas_.set_size(1000,500);
      
      // show everything....
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
    
    ChildEx* chld = makeChld();
    childz_.push_back(chld);
    uint x = rand() % 1000;
    uint y = rand() % 500;
    canvas_.put(*chld, x, y);
    chld->show();
  }
  
  
  void
  TimelinePanel::experiment_2()
  {
    frame_.set_label("Experiment 2...");
    for (Widget* chld : childz_)
      {
        uint x = canvas_.child_property_x(*chld);
        uint y = canvas_.child_property_y(*chld);
        int deltaX = -20 + rand() % 41;
        int deltaY = -15 + rand() % 31;
        x = uint(max (0, int(x) + deltaX));
        y = uint(max (0, int(y) + deltaY));
        
        canvas_.move (*chld, x,y);
      }
  }
  
  
  void
  TimelinePanel::experiment_3()
  {
    frame_.set_label("Experiment 3...");
    uint pos=0;
    for (Widget* chld : childz_)
      {
        uint y = rand() % 30;
        canvas_.move (*chld, pos, y);
        
        int width = chld->get_allocated_width();
        pos += 0.6 * width;
      }
  }
  
  
  void
  TimelinePanel::experiment_4()
  {
    frame_.set_label("Experiment 4...");
    uint killPos = rand() % childz_.size();
    ChildV::iterator killThat(&childz_[killPos]);
    ChildEx* victim = *killThat;
    childz_.erase (killThat);
    canvas_.remove (*victim);
    delete victim;
  }
  
  
  /* === Support for Investigation === */
  
  namespace {
    _Fmt childID("Chld-%02d");
    
    int instanceCnt = 0;
  }
  
  
  uint ChildEx::childNo = 0;
  
  
  ChildEx::ChildEx()
    : Gtk::Button(string (childID % childNo++))
    {
      ++instanceCnt;
    }
  
  
  void
  ChildEx::on_clicked()
  {
    cout << "|=="<<get_label()<<endl;
  }
  
  
  ChildEx*
  TimelinePanel::makeChld()
  {
    return Gtk::manage(new ChildEx);
  }

  
  ////////////////////////////////////////////////////////////////////TICKET #1020 : verification code for instance management
  ChildEx::~ChildEx()
    {
      --instanceCnt;
      if (instanceCnt > 0)
        cout << "  ↯↯  still "<<instanceCnt<<" children to kill..."<<endl;
      else
      if (instanceCnt == 0)
        cout << "+++ Success: all children are dead..."<<endl;
      else
        cout << "### ALARM ###"<<endl
             << "instanceCnt == "<<instanceCnt <<endl;
    }
  
  void
  __verifyDeadChildren()
    {
      if (instanceCnt == 0)
        cout << "+++ Success: all children are dead..."<<endl;
      else
        cout << "### ALARM ###"<<endl
             << "instanceCnt == "<<instanceCnt <<endl;
    }
  ////////////////////////////////////////////////////////////////////TICKET #1020 : verification code for instance management
  
  
  
}}   // namespace gui::panel
