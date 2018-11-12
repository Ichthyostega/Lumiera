/*
  GtkCanvasExperiment  -  Explore capabilities of the GTK canvas widget

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


/** @file gtk-canvas-experiment.cpp
 ** Implementation of gtk canvas experiments.
 ** 
 ** @remark as of 10/2018 we start to build a new timeline widget,
 **         based on this technology demo.
 ** @see timeline-widget.hpp
 ** 
 */


#include "gtk-canvas-experiment.hpp"

#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>



using util::_Fmt;
using util::isnil;
using Gtk::Widget;
using sigc::mem_fun;
using sigc::ptr_fun;
using std::string;
using std::rand;
using std::max;


namespace demo {
  
  
  CanvasDemoPanel::CanvasDemoPanel ()
    : Box{}
    , twoParts_(Gtk::ORIENTATION_VERTICAL)
    , buttons_()
    , frame_("Gtk::Layout Experiments")
    , scroller_()
    , canvas_()
    {
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      twoParts_.pack_start(frame_);
      
      buttons_.set_layout(Gtk::BUTTONBOX_START);
      
      // buttons to trigger experiments
      button_1_.set_label("_place");
      button_1_.set_use_underline();
      button_1_.set_tooltip_markup("<b>Experiment 1</b>:\nplace new child widget\nat random position on the canvas");
      button_1_.signal_clicked().connect(
                  mem_fun(*this, &CanvasDemoPanel::experiment_1));
      buttons_.add(button_1_);
      
      button_2_.set_label("_move");
      button_2_.set_use_underline();
      button_2_.set_tooltip_markup("<b>Experiment 2</b>:\nmove all child widgets randomly");
      button_2_.signal_clicked().connect(
                  mem_fun(*this, &CanvasDemoPanel::experiment_2));
      buttons_.add(button_2_);
      
      button_3_.set_label("a_lign");
      button_3_.set_use_underline();
      button_3_.set_tooltip_markup("<b>Experiment 3</b>:\nalign all child widgets in a row\nwith silight random vertical offset");
      button_3_.signal_clicked().connect(
                  mem_fun(*this, &CanvasDemoPanel::experiment_3));
      buttons_.add(button_3_);
      
      button_4_.set_label("_grow");
      button_4_.set_use_underline();
      button_4_.set_tooltip_markup("<b>Experiment 4</b>:\nextend arbitrary child widget's text");
      button_4_.signal_clicked().connect(
                  mem_fun(*this, &CanvasDemoPanel::experiment_4));
      buttons_.add(button_4_);
      
      button_5_.set_label("_kill");
      button_5_.set_use_underline();
      button_5_.set_tooltip_markup("<b>Experiment 5</b>:\nkill arbitrary child widget");
      button_5_.signal_clicked().connect(
                  mem_fun(*this, &CanvasDemoPanel::experiment_5));
      buttons_.add(button_5_);
      
      toggleDraw_.set_label("draw");
      toggleDraw_.signal_clicked().connect(
                  [this]() {
                             canvas_.enableDraw (this->toggleDraw_.get_active());
                           });
      buttons_.add(toggleDraw_);
      //(End)buttons...
      
      frame_.add(scroller_);
      frame_.set_border_width(5);
      
      scroller_.set_shadow_type(Gtk::SHADOW_IN);
      scroller_.property_expand() = true;    // dynamically grab any available additional space
      scroller_.set_border_width(10);
      scroller_.add(canvas_);
      
      canvas_.adjustSize();
      
      // show everything....
      this->add(twoParts_);
      this->show_all();
    }
  
  
  void
  CanvasDemoPanel::experiment_1()
  {
    frame_.set_label("Experiment 1... PLACE");
    
    ChildEx* chld = makeChld();
    childz_.push_back(chld);
    uint x = rand() % 1000;
    uint y = rand() % 500;
    canvas_.put(*chld, x, y);
    chld->show();
    canvas_.adjustSize();
  }
  
  
  void
  CanvasDemoPanel::experiment_2()
  {
    frame_.set_label("Experiment 2... MOVE");
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
    canvas_.adjustSize();
  }
  
  
  void
  CanvasDemoPanel::experiment_3()
  {
    frame_.set_label("Experiment 3... ALIGN");
    uint pos=0;
    for (Widget* chld : childz_)
      {
        uint y = rand() % 30;
        canvas_.move (*chld, pos, y);
        
        int width = chld->get_allocated_width();
        pos += 0.6 * width;
      }
    canvas_.adjustSize();
  }
  
  
  void
  CanvasDemoPanel::experiment_4()
  {
    frame_.set_label("Experiment 4... GROW");
    if (isnil (childz_))
      {
        ERROR (test, "need to fabricate more childz before you can grow 'em...");
        return;
      }
    uint selector = rand() % childz_.size();
    ChildEx& toGrow = *childz_[selector];
    toGrow.set_label ("***"+toGrow.get_label()+"***");
  }
  
  
  void
  CanvasDemoPanel::experiment_5()
  {
    frame_.set_label("Experiment 5... KILL");
    if (isnil (childz_))
      {
        WARN (test, "no children to kill. so sad.");
        return;
      }
    uint killPos = rand() % childz_.size();
    ChildV::iterator killThat(&childz_[killPos]);
    ChildEx* victim = *killThat;
    childz_.erase (killThat);
    canvas_.remove (*victim);
    delete victim;
  }
  
  
  void
  Canvas::enableDraw (bool yes)
  {
    shallDraw_ = yes;
    
    // force redrawing of the visible area...
    auto win = get_window();
    if (win)
      {
        int w = get_allocation().get_width();
        int h = get_allocation().get_height();
        Gdk::Rectangle rect{0, 0, w, h};
        win->invalidate_rect(rect, false);
      }
  }
  
  
  
  void
  Canvas::adjustSize()
  {
    recalcExtension_ = true;
  }
  
  void
  Canvas::determineExtension()
  {
      if (not recalcExtension_) return;
      
      uint extH=20, extV=20;
      Gtk::Container::ForeachSlot callback
        = [&](Gtk::Widget& chld)
                {
                  auto alloc = chld.get_allocation();
                  uint x = alloc.get_x();
                  uint y = alloc.get_y();
                  x += alloc.get_width();
                  y += alloc.get_height();
                  extH = max (extH, x);
                  extV = max (extV, y);
                };
      foreach(callback);
      recalcExtension_ = false;
      set_size (extH, extV);
  }
  
  
  bool
  Canvas::on_draw(Cairo::RefPtr<Cairo::Context> const& cox)
  {
    if (shallDraw_)
      {
        uint extH, extV;
        determineExtension();
        get_size (extH, extV);
        
        auto adjH = get_hadjustment();
        auto adjV = get_vadjustment();
        double offH = adjH->get_value();
        double offV = adjV->get_value();
        
        cox->save();
        cox->translate(-offH, -offV);
        
        // draw red diagonal line
        cox->set_source_rgb(0.8, 0.0, 0.0);
        cox->set_line_width (10.0);
        cox->move_to(0, 0);
        cox->line_to(extH, extV);
        cox->stroke();
        cox->restore();
        
        // cause child widgets to be redrawn
        bool event_is_handled = Gtk::Layout::on_draw(cox);
        
        // any drawing which follows happens on top of child widgets...
        cox->save();
        cox->translate(-offH, -offV);
        
        cox->set_source_rgb(0.2, 0.4, 0.9);
        cox->set_line_width (2.0);
        cox->rectangle(0,0, extH, extV);
        cox->stroke();
        cox->restore();
        
        return event_is_handled;
      }
    else
      return Gtk::Layout::on_draw(cox);
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
  CanvasDemoPanel::makeChld()
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
  
  
  
}   // namespace demo
