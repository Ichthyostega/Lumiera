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
#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



using util::max;
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
    const uint FALLBACK_FONT_SIZE_px = 12.5; // (assuming 96dpi and 10 Point font)
    const uint POINT_PER_INCH = 72;          // typographic point ≔ 1/72 inch
    
    const double BASE_WIDTH_PER_EM = 0.5;    // scale factor: width of double line relative to font size
    
    const double ORG       = 0.0;
    const double PHI       = (1.0 + sqrt(5)) / 2.0; // Golden Ratio Φ = 1.6180339887498948482
    const double PHI_MAJOR = PHI - 1.0;             // 1/Φ   = Φ-1
    const double PHI_MINOR = 2.0 - PHI;             // 1-1/Φ = 2-Φ
    const double PHISQUARE = 1.0 + PHI;             // Φ²    = 1+Φ
    const double PHI_MINSQ = 5.0 - 3*PHI;           // Φ-minor of Φ-minor : (2-Φ)²= 2 ²-4Φ + Φ²
    
    const double BAR_WIDTH = PHI_MINOR;             // the main (bold) bar is right aligned to axis
    const double BAR_LEFT  = -BAR_WIDTH;
    const double LIN_WIDTH = PHI_MINSQ;             // thin line is Φ-minor or bold line (which itself is Φ-minor)
    const double LIN_LEFT  = PHI_MAJOR - LIN_WIDTH; // main line and thin line create a Φ-division
    
    const double SQUARE_TIP_X = 2.2360679774997880;
    const double SQUARE_TIP_Y = -PHISQUARE;
    const double SQUARE_MINOR = 1.0;
    
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
     * Use contextual CSS style information to find out about the _standard font size_
     * @return absolute nominal size of the font in standard state, given in device units (px)
     */
    double
    getAbsoluteFontSize(StyleC style)
    {
      Pango::FontDescription font = style->get_font (Gtk::STATE_FLAG_NORMAL);
      auto sizeSpec = double(font.get_size()) / PANGO_SCALE;
      // Note: size specs are given as integers with multiplier PANGO_SCALE (typically 1024)
      if (sizeSpec <=0) return FALLBACK_FONT_SIZE_px;
      if (not font.get_size_is_absolute())
        {// size is given relative (in points)
          auto screen = style->get_screen();
          if (not screen) return FALLBACK_FONT_SIZE_px;
          double dpi = screen->get_resolution();
          sizeSpec *= dpi / POINT_PER_INCH;
        } // spec{points}/point_per_inch*pixel_per_inch ⟼ pixel
      return sizeSpec;
    }
    
    /**
     * Probe the CairoContext to determine scale factor already set.
     * @return number of device units representing a single horizontal user unit
     */
    double
    deviceUnitsPerUserUnit (CairoC cox)
    {
      auto dx = 1.0, dy = 0.0;
      cox->user_to_device_distance (dx,dy);
      return dx;
    }
    
    /**
     * Setup the base metric for this bracket drawing based on CSS styling.
     * @remark the width of the double line is used as foundation to derive
     *     further layout properties, based on the golden ratio.
     * @return scale factor to apply to the base layout
     */
    double
    determineScale (StyleC style)
    {
      return BASE_WIDTH_PER_EM * getAbsoluteFontSize (style);
    }
    
    /** place left anchor reference line to right side of bold bar.
     * @remark taking into account the scale and padding
     */
    double
    anchorLeft (StyleC style, double scale)
    {
      return style->get_padding().get_left()
           + scale * BAR_WIDTH;
    }
    
    /** place top cap vertical anchor, down from canvas upside.
     * @remark anchored at lower side of enclosing Φ²-sized square.
     */
    double
    anchorUpper (StyleC style, double scale)
    {
      return style->get_padding().get_top()
           - scale * SQUARE_TIP_Y;
    }
    
    /** place bottom cap vertical anchor, mirroring top cap
     * @remark also taking into account the overall canvas height
     */
    double
    anchorLower (StyleC style, double scale, int canvasHeight)
    {
      auto lowerAnchor
         = canvasHeight
           - (style->get_padding().get_bottom()
              - scale * SQUARE_TIP_Y);
      auto minHeight = PHISQUARE*scale + style->get_padding().get_top();
      return max (lowerAnchor, minHeight);  // Fallback: both caps back to back
    }
    
    
    /**
     * Draw the curved end cap of the bracket, inspired by musical notation.
     * @param ox horizontal offset of the anchor point; pixels in target device space
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
      cox->scale (scale, upside? scale:-scale);
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
    
    /** draw the double bar to fit between upper and lower cap */
    void
    drawBar (CairoC cox, double leftX, double upperY, double lowerY, double scale)
    {
      cox->save();
      cox->translate (leftX, upperY);
      cox->scale (scale, scale);
      cox->set_source_rgb(0.0, 0.0, 0.8); ///////TICKET #1168 : retrieve colour from stylesheet
      //
      double height = max (0.0, (lowerY - upperY)/scale);
      cox->rectangle(BAR_LEFT, -SQUARE_MINOR, BAR_WIDTH, height + 2*SQUARE_MINOR);
      cox->rectangle(LIN_LEFT,           ORG, LIN_WIDTH, height);
      //
      cox->fill();
      //
      cox->restore();
    }
  }
  
  
  
  
  StaveBracketWidget::~StaveBracketWidget() { }
  
  StaveBracketWidget::StaveBracketWidget ()
    : _Base{}
    {
      get_style_context()->add_class (CLASS_fork_bracket);
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
    
    REQUIRE (1.0 == deviceUnitsPerUserUnit (cox)
            ,"Cairo surface in device coordinates assumed");
    
    StyleC style = this->get_style_context();
    double scale = determineScale (style);
    double left  = anchorLeft (style, scale);
    double upper = anchorUpper (style,scale);
    double lower = anchorLower (style, scale, h);
    
    drawCap (cox, left, upper, scale, true);
    drawCap (cox, left, lower, scale, false);
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
