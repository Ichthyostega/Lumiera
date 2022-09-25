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
  
  Literal
  ElementBoxWidget::Config::getIconID()  const
  {
    ///////////////////////////////////////////////////////////////////////////TICKET #1185 : implement logic to pick suitable icon...
    return "track_enabled";      //////////////////////////////////////////////TICKET #1219 : maybe at least a better generic placeholder icon...?
  }
  
  Gtk::IconSize
  ElementBoxWidget::Config::getIconSize()  const
  {
    ///////////////////////////////////////////////////////////////////////////TICKET #1185 : is Icon-Size flexible under some circumstances?
    return Gtk::ICON_SIZE_MENU;
  }

  
  ElementBoxWidget::Strategy
  ElementBoxWidget::Config::buildLayoutStrategy(ElementBoxWidget& widget)
  {
    Strategy strategy;          //NOTE: relying on return-value-optimisation
    ///////////////////////////////////////////////////////////////////////////TICKET #1235 : evaluate size constraint here
    return strategy;
  }
  
  
  
  ElementBoxWidget::ElementBoxWidget (Config config)
    : Frame{}
    , strategy_{config.buildLayoutStrategy(*this)}
    , label_{Gtk::ORIENTATION_HORIZONTAL}
    , icon_{Gtk::StockID{config.getIconID()}  ///////////////////////////////TICKET #1030 : use of stockIDs is deprecated; care for a more modern icon naming scheme
           , config.getIconSize()}
    {
      set_name(ID_element);
      get_style_context()->add_class(CLASS_background);     // Style to ensure an opaque backdrop
      set_label_align(0.0, 0.0);
      
      set_label_widget(label_);
      label_.add(icon_);
      label_.add(name_);
      label_.set_name(ID_caption);
      label_.get_style_context()->add_class(CLASS_background);
      name_.set_text(config.getName());
      name_.set_hexpand(true);
      
      this->show_all();
    }
  
  
  /**
   * Layout trend for ElementBoxWidget is nailed down (`final`) to "height-for-width".
   * The reason is, some of the use cases entail placing the element box onto a canvas
   * with horizontal extension calibrated to time units; doing so requires us to control
   * the extension, which is delegated through the strategy
   */
  Gtk::SizeRequestMode
  ElementBoxWidget::get_request_mode_vfunc()  const
  {
    return Gtk::SizeRequestMode::SIZE_REQUEST_HEIGHT_FOR_WIDTH;
  }
  
  /**
   * Layout preferences are delegated through the Strategy
   * - by default, the strategy will just invoke the inherited vfuncs
   * - however, when a size constraint for the ElementBoxWidget must be observed,
   *   the strategy will control the extension of our child widgets (side-effect)
   *   and then just return the extension as dictated by the constraints
   * @remark Based on the GTK-3.22 implementation code, we know that the minimum_width
   *   is only used for consistency checks (and otherwise ignored), while the natural_width
   *   will always be respected. The GKT layout management might _increase_ that value...
   *   - do adjust for additional border and margin settings from CSS
   *   - and to expand the widget when used within a container with fill-alignment
   *   Moreover the #get_preferred_height_for_width_vfunc will be invoked with the
   *   results from this function. The possible adjustment is done by invoking the
   *   vfunc `adjust_size_allocation` on the GTK-class, which typically delegates to
   *   gtk_widget_real_adjust_size_allocation, and the latter invokes
   *   - adjust_for_margin
   *   - adjust_for_align
   *   After these adjustments and some sanity checks, the resulting size allocation
   *   is passed to the `size_allocate` vfunc, which could be tapped on the Gtk::Widget
   *   C++ wrapper, but usually just stores this allocation into the widget private data. 
   */
  void
  ElementBoxWidget::get_preferred_width_vfunc (int& minimum_width, int& natural_width)  const
  {
    if (strategy_.is_size_constrained())
      minimum_width = natural_width = strategy_.getWidth();
    else
      _Base::get_preferred_width_vfunc (minimum_width,natural_width);
  }
  
  void
  ElementBoxWidget::get_preferred_height_for_width_vfunc (int width, int& minimum_height, int& natural_height)  const
  {
    if (strategy_.is_size_constrained() and strategy_.shall_control_height())
      minimum_height = natural_height = strategy_.getHeight();
    else
      _Base::get_preferred_height_for_width_vfunc (width, minimum_height,natural_height);
  }
  
  
  
}}// namespace stage::widget
