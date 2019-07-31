/*
  gtk-style-experiment.cpp  -  simple application to investigate GTK styling

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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
*/


/** @file gtk-style-experiment.cpp
 ** Simple GTK Application frame to investigate some aspects of GTK's CSS implementation.
 ** Especially the goal is to _grab some existing CSS styling_ with a Gtk::StyleContext
 ** and use this for custom drawing. Within Lumiera, we use custom drawing for some parts
 ** of the Timeline UI presentation.
 ** @see stage::timeline::BodyCanvasWidget
 */


#include "stage/gtk-base.hpp"
#include "lib/searchpath.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <string>

using util::cStr;
using std::string;

namespace research {
  
  namespace {
    const string STYLESHEET_NAME{"gtk-style-experiment.css"};
    const string RESOURCE_PATH{"$ORIGIN/gui"};
    
    const string CLASS_experiment{"experiment"};
  }
  
  using CairoC = stage::PCairoContext const&;
  using StyleC = stage::PStyleContext const&;
  
  using stage::PStyleContext;
  
  
  class Canvas
    : public Gtk::Layout
    {
      bool shallDraw_       = false;
      bool recalcExtension_ = false;
      
      StyleC style_;
      
    public:
      Canvas(StyleC refStyle)
        : style_{refStyle}
      { }
      
      void adjustSize();
      void enableDraw (bool);
      
    private:
      virtual bool on_draw (Cairo::RefPtr<Cairo::Context> const&)  override;
      
      void determineExtension();
    };


  class StyleTestPanel
    : public Gtk::Box
    {
    public:
      StyleTestPanel();
      
    private:
      Gtk::Box twoParts_;
      Gtk::ButtonBox buttons_;
      Gtk::Button button_1_;
      Gtk::Button button_2_;
      Gtk::CheckButton toggleDraw_;
      Gtk::Frame frame_;
      Gtk::ScrolledWindow scroller_;
      
      PStyleContext pStyle_;
      Canvas canvas_;
      
      PStyleContext setupStyle();
      
      void experiment_1();
      void experiment_2();
    };
  
  
  /* ===== Implementation ===== */
  
  StyleTestPanel::StyleTestPanel()
    : Box{}
    , twoParts_{Gtk::ORIENTATION_VERTICAL}
    , buttons_{}
    , frame_{"Gtk::StyleContext Experiments"}
    , scroller_{}
    , pStyle_{}
    , canvas_{pStyle_}
    {
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      twoParts_.pack_start(frame_);
      
      buttons_.set_layout(Gtk::BUTTONBOX_START);
      
      // buttons to trigger experiments
      button_1_.set_label("_grow");
      button_1_.set_use_underline();
      button_1_.set_tooltip_markup("<b>Experiment 1</b>:\ngrow the border size");
      button_1_.signal_clicked().connect(
                  mem_fun(*this, &StyleTestPanel::experiment_1));
      buttons_.add(button_1_);
      
      button_2_.set_label("_dump");
      button_2_.set_use_underline();
      button_2_.set_tooltip_markup("<b>Experiment 2</b>:\ndump style information");
      button_2_.signal_clicked().connect(
                  mem_fun(*this, &StyleTestPanel::experiment_2));
      buttons_.add(button_2_);
      
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
      
      frame_.get_style_context()->add_class(CLASS_experiment);
      pStyle_ = setupStyle();
      
      canvas_.adjustSize();
      
      // show everything....
      this->add(twoParts_);
      this->show_all();
    }
  
  
  
  PStyleContext
  StyleTestPanel::setupStyle()
  {
    auto screen = Gdk::Screen::get_default();
    auto css_provider = Gtk::CssProvider::create();
    try
      {
        css_provider->load_from_path (lib::resolveModulePath (STYLESHEET_NAME, RESOURCE_PATH));
      }
    catch(Glib::Error const& failure)
      {
        WARN (stage, "Failure while loading stylesheet '%s': %s", cStr(STYLESHEET_NAME), cStr(failure.what()));
      }
    
    Gtk::StyleContext::add_provider_for_screen (screen, css_provider,
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    return PStyleContext{};
  }
  
  
  
  void
  StyleTestPanel::experiment_1()
  {
    frame_.set_label("Experiment 1... GROW");
    
    canvas_.adjustSize();
  }
  
  
  void
  StyleTestPanel::experiment_2()
  {
    frame_.set_label("Experiment 2... DUMP");
//    canvas_.adjustSize();
  }

  
  /* === Canvas drawing code === */
  
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
      
      uint extH=200, extV=200;
      recalcExtension_ = false;
      set_size (extH, extV);
  }
  
  
  bool
  Canvas::on_draw(CairoC cox)
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

  
  
  
  class Window
    : public Gtk::Window
    {
      StyleTestPanel demoPanel_;
      
    public:
      Window()
        : demoPanel_{}
        {
          this->add (demoPanel_);
        }
    };
  
}//namespace research



int
main (int argc, char* argv[])
{
  NOTICE (main, "*** GTK Style Experiment ***");
  
  auto guiApp = Gtk::Application::create (argc, argv);
  
  research::Window window;
  return guiApp->run(window);
}
