/*
  StaveBracketWidget  -  track body area to show overview and timecode and markers

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file stave-bracket-widget.cpp
 ** Implementation of drawing code to indicate the structure of nested tracks
 ** in the header area of the Timleline UI.
 ** 
 ** @todo WIP-WIP-WIP as of 2/2023
 ** 
 */


#include "stage/timeline/stave-bracket-widget.hpp"
//#include "stage/style-scheme.hpp"  /////////////////////TODO needed?
//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::isnil;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  namespace {
    const uint REQUIRED_WIDTH_px = 30;
    
    const double ORG       = 0.0;
    const double PHI       = (1.0 + sqrt(5)) / 2.0; // Golden Ratio Φ = 1.6180339887498948482
    const double PHI_MAJOR = PHI - 1.0;             // 1/Φ   = Φ-1
    const double PHI_MINOR = 2.0 - PHI;             // 1-1/Φ = 2-Φ
    const double PHISQUARE = 1.0 + PHI;             // Φ²    = 1+Φ
    
    const double BAR_LEFT = -PHI_MINOR;
    
    const double SQUARE_TIP_X = 2.2360679774997880;
    const double SQUARE_TIP_Y = -PHISQUARE;
    
    const double ARC_O_XC  = -(3.0 + PHI);
    const double ARC_O_YC  = -6.8541019662496847;   // +Y points downwards
    const double ARC_O_R   =  8.0574801069408135;   // Radius of the arc segment
    const double ARC_O_TIP =  0.5535743588970450;   // Radians ↻ clockwise from +X
    const double ARC_O_END =  1.0172219678978512;
    
    const double ARC_I_XC  = -2.5;
    const double ARC_I_YC  = -7.3541019662496883;
    const double ARC_I_R   =  6.6978115661011230;
    const double ARC_I_TIP =  0.7853981633974485;
    const double ARC_I_END =  1.2490457723982538;
    
    /**
     * Setup the base metric for this bracket drawing based on CSS styling.
     * @remark the width of the double line is used as foundation to derive
     *     further layout properties, based on the golden ratio.
     * @return scale factor to apply to the base layout
     */
    double
    determineScale (StyleC style)
    {
      UNIMPLEMENTED ("set base line width based on suitable CSS property");
    }
    
    double
    anchorLeft (double scale)
    {
      UNIMPLEMENTED ("place left anchor reference line");
    }
    
    double
    anchorUpper (double scale)
    {
      UNIMPLEMENTED ("place top cap vertical anchor");
    }
    
    double
    anchorLower (double scale, int canvasHeight)
    {
      UNIMPLEMENTED ("place bottom cap vertical anchor");
    }
    
    
    /**
     * Draw the curved end cap of the bracket, inspired by musical notation.
     * @param ox horizontal offset of the anchor point in pixels
     * @param oy vertical anchor point offset, downwards is positive
     * @param scale stretch the design; default is bracket line width = 1.0
     * @param upside whether to draw the upper cup (`true`) or the lower
     * @remark See `doc/devel/draw/StaveBracket.svg` for explanation
     */
    void
    drawCap (CairoC cox, double ox, double oy, double scale, bool upside=true)
    {
      cox->save();
      cox->translate (ox,oy);
      cox->scale (scale,scale); /////////////////TODO: flip unless upside
      cox->set_source_rgb(0.0, 0.0, 0.8); ///////TICKET #1168 : retrieve colour from stylesheet
      // draw the inner contour of the bracket cap,
      // which is the outer arc from left top of the bar to the tip point
      cox->move_to(BAR_LEFT, ORG);
      cox->arc_negative(ARC_O_XC,ARC_O_YC,ARC_O_R, ARC_O_END, ARC_O_TIP);
      // draw the outer contour of the bracket cap,
      // which is the inner arc from tip point to Φ-minor of the enclosing square
      cox->arc         (ARC_I_XC,ARC_I_YC,ARC_I_R, ARC_I_TIP, ARC_I_END);
      cox->close_path();
      //
      cox->fill();
      //
      cox->restore();
    }
    
    void
    drawBar (CairoC cox, double leftX, double upperY, double lowerY, double scale)
    {
      UNIMPLEMENTED ("draw the double bar");
    }
  }
  
  
  
  
  StaveBracketWidget::~StaveBracketWidget() { }
  
  StaveBracketWidget::StaveBracketWidget ()
    : _Base{}
    {
      this->property_expand() = false;
    }
  
  
  
  /** */
  bool
  StaveBracketWidget::on_draw (CairoC cox)
  {
    // invoke (presumably empty) base implementation....
    bool event_is_handled = _Base::on_draw (cox);
    
    /////////////////////////////////////////////TICKET #1018 : placeholder drawing
    //
    int w = get_width();
    int h = get_allocated_height();
    cox->set_source_rgb(0.8, 0.0, 0.0);
    cox->set_line_width (5.0);
    cox->move_to(0, 0);
    cox->line_to(w, h);
    cox->move_to(w, 0);
    cox->line_to(0, h);
    cox->stroke();
    /////////////////////////////////////////////TICKET #1018 : placeholder drawing
    
    StyleC style = this->get_style_context();
    double scale = determineScale (style);
    double left  = anchorLeft (scale);
    double upper = anchorUpper (scale);
    double lower = anchorLower (scale, h);
    
    drawCap (cox, left, upper, scale, true);  //////////TODO proper scale and placement
    drawCap (cox, left, lower, scale, false);  //////////TODO proper scale and placement
    drawBar (cox, left, upper, lower, scale);
    
    return event_is_handled;
  }
  
  
  /** indicate layout oriented towards vertical extension */
  Gtk::SizeRequestMode
  StaveBracketWidget::get_request_mode_vfunc()  const
  {
    return Gtk::SizeRequestMode::SIZE_REQUEST_WIDTH_FOR_HEIGHT;
  }
  
  /**
   * The structural outline adapts flexible in vertical direction,
   * but requires a fixed horizontal size for proper drawing.
   */
  void
  StaveBracketWidget::get_preferred_width_vfunc (int& minimum_width, int& natural_width)  const
  {
    minimum_width = natural_width = REQUIRED_WIDTH_px;
  }
  
  void
  StaveBracketWidget::get_preferred_width_for_height_vfunc (int, int& minimum_width, int& natural_width)  const
  {
    get_preferred_width (minimum_width, natural_width);
  }
  
  
  
  
}}// namespace stage::timeline
