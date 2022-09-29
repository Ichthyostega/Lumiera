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
 ** A special twist arises from the requirement to show the temporal extension of media,
 ** leading to a display on a time calibrated canvas, where a given time span corresponds
 ** to some fixed pixel count, according to the current zoom factor. Such a layout stands in
 ** contradiction to the fundamental design principles of GTK — the assumption being that
 ** all widget layout shall be determined dynamically, to adopt to screen resolution, system
 ** fonts and user provided styles and themes.
 ** 
 ** This conflict can be reconciled by representing this time-to-size calibration in the form
 ** of a desired minimal extension reported by the individual widget. Since GTK only ever _expands_
 ** the widget provided size request (e.g. to accommodate for additional borders and padding from CSS),
 ** our timeline canvas can be sure to get at least the necessary extension. Moreover, we can assure,
 ** with the help of our custom style sheet, that those size constrained widgets do not require any
 ** additional decoration, borders or margin. Accordingly, we choose `Gtk::Frame` as the base class,
 ** so the border is drawn as part of the widget's content, with the identification label placed on top.
 ** 
 ** However, after declaring to GTK that the widget can be rendered within the specific size constraint,
 ** it now becomes our responsibility to enforce this size constraint onto any child widgets used as
 ** part of the ElementBoxWidget — especially we have to query the size required to represent the
 ** name-ID label, possibly taking measures to reduce this size to fit
 ** @todo as of 9/2022 we just hide the label text completely to comply with the constraints;
 **       it is conceivable to use a more elaborate approach and to shorten the label text to fit.
 ** 
 ** @todo WIP-WIP as of 09/2022   ///////////////////////////////////////////////////////////////////////TICKET #1185
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/widget/element-box-widget.hpp"
#include "stage/style-scheme.hpp"

//#include "lib/format-string.hpp" //////////////////TODO debugging
#include "lib/format-cout.hpp" //////////////////////TODO debugging

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::contains;
//using Gtk::Widget;


namespace stage {
namespace widget {
  
  
  namespace {//Implementation helpers...
    /**
     * Helper to retrieve what GTK effectively uses as minimal extension of a widget.
     * @remark in fact this is the so called "preferred natural size", since GTK always
     *         allocates at least this amount of screen extension. Judging from the GTK 3.24
     *         code (as of 9/2022), the minimal size setting and the "size_request" are in most
     *         cases just used for consistency checks, while the natural size is determined in accordance
     *         to the layout preference (height-for-width or width-for-hight) and then only ever increased
     *         to fit further CSS settings (border, margin) and to handle fill-layout.
     * @warning however note that `Gtk::Layout` (which we use as foundation for our Timeline canvas) indeed
     *         starts its calculation from the **minimal width** of the attached child widget. Thus, as far
     *         as implementing the VFuncs, both cases should be treated symmetrically.
     */
    inline void
    queryNaturalSize (Gtk::Widget const& widget, Gtk::Requisition& natSize)
    {
      Gtk::Requisition minimumDummy;
      widget.get_preferred_size (minimumDummy, natSize);
    }
  }//(End)helpers
  
  
  
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
  ElementBoxWidget::Config::buildLayoutStrategy(ElementBoxWidget&)
  {
    Strategy strategy;
    if (widthConstraint_)
      strategy.getWidth = move(widthConstraint_);
    if (heightConstraint_)
      strategy.getHeight = move(heightConstraint_);
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
   * - by default, the strategy will just invoke the inherited VFuncs
   * - however, when a size constraint for the ElementBoxWidget must be observed,
   *   the strategy will control the extension of our child widgets (side-effect)
   *   and then just return the extension as dictated by the constraints
   * @remark Based on the GTK-3.24 implementation code, we know that most usages will draw
   *   upon the natural_width, but there are some use cases (esp. `Gtk::Layout`), where the
   *   calculation takes minimum_width as starting point. The returned values will always be
   *   respected, while the GTK layout engine might _increase_ the given extensions...
   *   - do adjust for additional border and margin settings from CSS
   *   - and to expand the widget when used within a container with fill-alignment
   *   Moreover the #get_preferred_height_for_width_vfunc will be invoked with the
   *   results from this function. The possible adjustment is done by invoking the
   *   VFunc `adjust_size_allocation` on the GTK-class, which typically delegates to
   *   gtk_widget_real_adjust_size_allocation, and the latter invokes
   *   - adjust_for_margin
   *   - adjust_for_align
   *   After these adjustments and some sanity checks, the resulting size allocation is passed
   *   to the `size_allocate` VFunc, which passes through the `Gtk::Widget::on_size_allocate()`
   *   and after that by default stores this allocation into the widget's private data.
   */
  void
  ElementBoxWidget::get_preferred_width_vfunc (int& minimum_width, int& natural_width)  const
  {
    if (strategy_.is_size_constrained())
      minimum_width = natural_width = strategy_.getWidth();
    else
      _Base::get_preferred_width_vfunc (minimum_width,natural_width);
  }
  
  /**
   * @remark only provided for sake of completeness, since GTK code is complex and some code path
   *         might ignore our _request mode preference_ and invoke both functions.
   */
  void
  ElementBoxWidget::get_preferred_height_vfunc (int& minimum_height, int& natural_height)  const
  {
    if (strategy_.shall_control_height())
      minimum_height = natural_height = strategy_.getHeight();
    else
      _Base::get_preferred_height_vfunc (minimum_height,natural_height);
  }
  
  void
  ElementBoxWidget::get_preferred_height_for_width_vfunc (int width, int& minimum_height, int& natural_height)  const
  {
    if (strategy_.is_size_constrained() and strategy_.shall_control_height())
      minimum_height = natural_height = strategy_.getHeight();
    else
      _Base::get_preferred_height_for_width_vfunc (width, minimum_height,natural_height);
  }
  
  /**
   * Tap into the notification of screen space allocation to possibly enforce size constraints.
   * Generally speaking, explicit size constrained widgets are not a concept supported by GTK,
   * due to its flexible layout model and the use of CSS for styling and theming. Unfortunately
   * we need this feature, in order to implement Clip widgets on a time calibrated canvas.
   * Thus we use an **implementation trick**: we report our size constraints as "natural size"
   * to GTK, knowing that GTK will respect those extensions (unless the widget is placed into
   * a container with fill-alignment). However, by doing so, we have effectively lied to GTK
   * with respect to our child widgets. Thus we now have to take on the responsibility to
   * somehow make those child widgets fit into the limited size allocation....
   */
  void
  ElementBoxWidget::on_size_allocate (Gtk::Allocation& availableSize)
  {
    if (strategy_.is_size_constrained())
      imposeSizeConstraint (availableSize.get_width(), availableSize.get_height());
    _Base::on_size_allocate(availableSize);
  }
  
  
  /**
   * Ensure the child widgets can be represented and possibly adjust or hide content,
   * in case the extension of ElementBoxWidget is explicitly constrained in size.
   */
  void
  ElementBoxWidget::imposeSizeConstraint(int widthC, int heightC)
  {
    ASSERT (name_.get_realized(), "ElementBoxWidget layout constraint imposed "
                                  "on widget not yet realized by GTK");

    bool hiddenCaption = not name_.get_visible();
    if (not hiddenCaption)
      { // detect if label box fits within given size constraint
        queryNaturalSize (label_, labelFullSize_);
        
        if (labelFullSize_.width > widthC or labelFullSize_.height > heightC)
          name_.hide();
      }
    else
      {
        if (labelFullSize_.width <= widthC and labelFullSize_.height <= heightC)
          name_.show();
      }
  }
  
  
  
}}// namespace stage::widget
