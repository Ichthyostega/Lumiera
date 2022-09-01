/*
  ElementBoxWidget  -  fundamental UI building block to represent a placed element

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


/** @file element-box-widget.cpp
 ** Implementation details of the UI building block to display an ID label.
 ** 
 ** @todo WIP-WIP-WIP as of 11/2018   ///////////////////////////////////////////////////////////////////////TICKET #1185
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/widget/element-box-widget.hpp"
#include "stage/style-scheme.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace widget {
  
  
  
  
  
  ElementBoxWidget::~ElementBoxWidget()  { }
  
  void
  ElementBoxWidget::Strategy::configure()
  {
    
  }
  
  ElementBoxWidget::ElementBoxWidget (Strategy strategy)
    : Frame{}
    , label_{Gtk::ORIENTATION_HORIZONTAL}
    , icon_{Gtk::StockID{"track_enabled"}, Gtk::ICON_SIZE_MENU}  ////////////////TODO: use of stockIDs is deprecated; care for a more modern icon naming scheme
    {
      strategy.configure();
      set_name(ID_element);
      get_style_context()->add_class(CLASS_background);
      set_label_align(0.0, 0.0);
      
      set_label_widget(label_);
      label_.add(icon_);
      label_.add(name_);
      label_.set_name(ID_caption);
      label_.get_style_context()->add_class(CLASS_background);
      name_.set_text(strategy.getName());
      name_.set_hexpand(true);
      
      this->show_all();
    }
  
  
  
  
  
}}// namespace stage::widget
