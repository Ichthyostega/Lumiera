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
      Gtk::Requisition minDummy;
      widget.get_preferred_size (minDummy, natSize);
    }
    
    inline int
    queryNaturalHeight (Gtk::Widget const& widget)
    {
      int minDummy{0}, natHeight{0};
      widget.get_preferred_height(minDummy, natHeight);
      return natHeight;
    }
    
    inline int
    queryNaturalWidth (Gtk::Widget const& widget)
    {
      int minDummy{0}, natWidth{0};
      widget.get_preferred_width(minDummy, natWidth);
      return natWidth;
    }

    /** point of reference for layout computations */
    Gtk::Requisition ICON_SIZ{};
    
    /** excess factor used to prevent "layout flickering"
     * @remark once hidden, an element will only be re-shown
     *         when some excess headroom is available */
    const double HYSTERESIS = 1.6;
    
    inline void
    initIconSizeHeuristic (Gtk::Widget const& icon)
    {
      if (ICON_SIZ.width > 0) return;
      queryNaturalSize (icon, ICON_SIZ);
    }
    
  }//(End)helpers
  
  
  
  
  IDLabel::~IDLabel()                    { }
  ElementBoxWidget::~ElementBoxWidget()  { }
  
  
  Literal
  ElementBoxWidget::Config::getIconID()  const
  {
    ///////////////////////////////////////////////////////////////////////////TICKET #1185 : implement logic to pick suitable icon...
    return ICON_placement;
  }
  
  Literal
  ElementBoxWidget::Config::getMenuSymb()  const
  {
    ///////////////////////////////////////////////////////////////////////////TICKET #1185 : implement logic to pick suitable icon...
    return ICON_arrow_hand_menu;
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
  
  
  IDLabel::IDLabel (Literal iconID, Literal menuSymb, Gtk::IconSize siz)
    : Gtk::Box{Gtk::ORIENTATION_HORIZONTAL}
    , imgIcon_{Gtk::StockID{iconID},   siz}    ///////////////////////////////TICKET #1030 : use of stockIDs is deprecated; care for a more modern icon naming scheme
    , imgMenu_{Gtk::StockID{menuSymb}, siz}   ////////////////////////
    {
      icon_.set_image(imgIcon_);
      menu_.set_image(imgMenu_);
      this->add(icon_);
      this->add(menu_);
      this->add(name_);
      this->set_name(ID_idlabel);
      this->get_style_context()->add_class(CLASS_background);
      this->get_style_context()->add_class(CLASS_idlabel);
      icon_.get_style_context()->add_class(CLASS_idlabel_icon);
      menu_.get_style_context()->add_class(CLASS_idlabel_menu);
      name_.get_style_context()->add_class(CLASS_idlabel_name);
      name_.set_hexpand(true);
      
      this->show_all();
      initIconSizeHeuristic (icon_);
    }
  
  
  cuString
  IDLabel::getCaption()  const
  {
    return name_.get_text();
  }
  
  void
  IDLabel::setCaption(cuString& idCaption)
  {
    name_.set_text(idCaption);
    show_all();
    // cache required full display size (for size constrained layout)
    queryNaturalSize (*this, labelFullSize_);
  }
  
  
  
  ElementBoxWidget::ElementBoxWidget (Config config)
    : Frame{}
    , strategy_{config.buildLayoutStrategy(*this)}
    , label_{config.getIconID()
            ,config.getMenuSymb()
            ,config.getIconSize()}
    {
      set_name(ID_element);
      get_style_context()->add_class(CLASS_background);     // Style to ensure an opaque backdrop
      get_style_context()->add_class(CLASS_elementbox);
      set_label_align(0.0, 0.0);
      
      set_label_widget(label_);
      label_.setCaption(config.getName());
      label_.get_style_context()->add_class(CLASS_elementbox_idlabel);
      
      this->show_all();
    }
  
  
  void
  ElementBoxWidget::setName (cuString& nameID)
  {
    label_.setCaption (nameID);
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
  ElementBoxWidget::imposeSizeConstraint (int widthC, int heightC)
  {
    ASSERT (label_.get_realized(), "ElementBoxWidget layout constraint imposed "
                                   "on widget not yet realized by GTK");
    label_.imposeSizeConstraint (widthC, heightC);
  }
  
  /**
   * Ensure the IDLabel stays within a given size constraint.
   * In case the standard rendering complete with icon and name caption
   * exceeds the given screen space, try to bring this widget into imposed
   * limits by reducing or hiding some parts.
   */
  void
  IDLabel::imposeSizeConstraint (int widthC, int heightC)
  {
    // short circuit: need to perform precise checks?
    if (  labelFullSize_.width >  widthC
       or labelFullSize_.height > heightC
       )
      this->adaptSize(widthC, heightC);
  }
  
  
  namespace {//IDLabel layout management internals....
    
    /** attempt to reduce space consumption
     * @return achieved width reduction in pixels
     */
    inline int
    reduce(Gtk::Button& icon)
    {
      int widthReduction{0};
      if (icon.get_visible())
        {
          widthReduction = queryNaturalWidth (icon);
          icon.hide();
        }
      return widthReduction;
    }

    /// @todo 10/22 also attempt to shorten the label...    ///////////////////TICKET #1242 : adjust size of the ID caption
    inline int
    reduce(Gtk::Label& label, int goal)
    {
      ASSERT (goal >=0);
      int reduction{0};
      if (label.get_visible())
        {
          int width = queryNaturalWidth (label);
///////////////TODO do something to shorten the label /////////////////////////TICKET #1242 : adjust size of the ID caption
///////   int after = queryNaturalWidth (label);
///////   reduction = width - after;
          if (reduction < goal)
            {//shortening alone does not suffice
              label.hide();
              reduction = width;
            }
        }
      return reduction;
    }
    
    /** attempt to use available space to show more content
     * @param icon widget to possibly expand
     * @param w additional width available
     * @param h vertical headroom available
     * @param reCheck function to update and verify success
     * @return if hidden content could successfully be expanded
     */
    template<class FUN>
    inline bool
    maybeShow(Gtk::Button& icon, int w, int h, FUN& reCheck)
    {
      if (icon.is_visible()) return true; // nothing can be done here
      bool success{false};
      if (w >= ICON_SIZ.width * HYSTERESIS and h >= ICON_SIZ.height)
        {
          icon.show();
          if (not (success=reCheck()))
            icon.hide();
        }
      return success;
    }
    
    template<class FUN>
    inline bool
    maybeShow(Gtk::Label& label, int w, int h, FUN& reCheck)
    {
      bool success{false};
      // use icon dimensions as as heuristics to determine
      // if attempting to show the label is worth trying...
      if (w >= ICON_SIZ.width * HYSTERESIS and h >= ICON_SIZ.height)
        {
          label.show();
          int width = queryNaturalWidth (label);
          int goal = width - w;
          if (goal > 0) // too large, yet might fit if shortened
            reduce (label, goal);
          if (not (success=reCheck()))
            label.hide();
        }
      return success;
    }
    

  }//(End)Layout helpers
  

  /**
   * Multi-step procedure to keep this IDLabel widget within the given
   * screen size constraints. In case the extension needs to be reduced,
   * the name label and both icons are probed and possibly reduced.
   * Otherwise, if there is sufficient headroom, an attempt is made
   * possibly to show parts again, albeit with some hysteresis.
   * @todo as of 10/22, a planned full implementation will eventually
   *       shorten the caption text and possibly also combine both
   *       Icons into a single button when necessary...     ///////////////////TICKET #1242 : adjust size of the ID caption
   */
  void
  IDLabel::adaptSize (int widthC, int heightC)
  {
    // first determine if vertical extension is problematic
    int currH = queryNaturalHeight (*this);
    if (currH > heightC)
      {//hide all child widgets,
      // not much options left...
        name_.hide();
        menu_.hide();
        icon_.hide();
        return;
      }
    
    // now test if we need to reduce or can expand
    int currW = queryNaturalWidth (*this);
    if  (currW > widthC)
      {//reduce to comply
        int goal = currW - widthC;
        ASSERT (goal > 0);
        if ((goal -= reduce(name_, goal)) <= 0) return;
        if ((goal -= reduce(menu_)      ) <= 0) return;
        if ((goal -= reduce(icon_)      ) <= 0) return;
        currW = queryNaturalWidth(*this);
        goal = currW - widthC;
        ENSURE (goal <= 0, "IDLabel layout management floundered. "
                           "Removed all content, yet remaining width %d > %d"
                         ,                                         currW, widthC);
      }
    else
      {//maybe some headroom left to show more?
        int headroom = widthC - currW;
        auto reCheck = [&]() -> bool
                          {// WARNING: side effect assignment
                            currW = queryNaturalWidth (*this);
                            currH = queryNaturalHeight(*this);
                            headroom = widthC - currW;
                            return currH <= heightC
                               and currW <= widthC;
                          };
        
        if (not maybeShow (icon_, headroom, heightC, reCheck)) return;
        if (not maybeShow (menu_, headroom, heightC, reCheck)) return;
        if (not maybeShow (name_, headroom, heightC, reCheck)) return;
      }
  }
  
  
  
}}// namespace stage::widget
