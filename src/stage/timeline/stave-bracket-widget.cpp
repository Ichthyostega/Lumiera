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
 ** Implementation of drawing code to indicate the structure of nested tracks.
 ** The design of the drawing is inspired by classical score notation, where braces
 ** are used to group the staves (or stems) for one instrument (e.g. grand piano, organ),
 ** while brackets are used to group the staves of an ensemble (e.g. string quartet, symphony orchestra)
 ** The usual typesetting of musical notation relies much on classical design principles, based on the
 ** golden ratio Φ, which is known to be perceived as _balanced, neutral and unobtrusive._
 ** 
 ** The design implemented here is built around a vertical double bar line, and the width of this line
 ** is used as reference for size calculations; the curved top and bottom cap is enclosed within a
 ** bounding box of size Φ², when defining the _base width_ to be 1. The inner tangent of the curved
 ** cap will point towards the end of the smaller (inner) vertical line. While this design is quite
 ** simple in structure, and rather easy to construct geometrically, deriving all necessary coordinates
 ** numerically can be a challenge. To build this implementation, the constraint system of *FreeCAD*
 ** was used to define the relations, and the resulting numbers were picked directly from the resulting
 ** XML document, and used both to build a SVG for documentation, and for the constants in this source file.
 ** Using the Gtk::StyleContext and the given Cairo::Context, the drawing code derives the size of the
 ** defined standard font in device units, and uses this _em_ size as reference to derive a _scale_ factor,
 ** which is then applied to the drawing as a whole — taking into account any given vertical size limitations
 ** as imposed by the general nested trade head structure.
 ** @see the FreeCAD document can be found at `doc/devel/draw/StaveBracket.FCStd`
 ** @see SVG image `doc/devel/draw/StaveBracket.svg` for explanation of geometry
 ** @see explanation on page #TrackStaveBracket in the TiddlyWiki
 ** 
 */


#include "stage/timeline/stave-bracket-widget.hpp"
#include "stage/style-scheme.hpp"
#include "lib/util.hpp"

#include <cmath>



using util::min;
using util::max;
using std::ceil;


namespace stage {
namespace timeline {
  
  namespace {//---------Implementation-details--Stave-Bracket-design-----------------------
    
    const uint FALLBACK_FONT_SIZE_px = 12.5; // (assuming 96dpi and 10 Point font)
    const uint POINT_PER_INCH = 72;          // typographic point ≔ 1/72 inch
    
    const double BASE_WIDTH_PER_EM = 0.5;    // scale factor: width of double line relative to font size
    
    const double ORG       = 0.0;
    const double PHI       = (1.0 + sqrt(5)) / 2.0; // Golden Ratio Φ ≔ ½·(1+√5) ≈ 1.6180339887498948482
    const double PHI_MAJOR = PHI - 1.0;             // 1/Φ   = Φ-1
    const double PHI_MINOR = 2.0 - PHI;             // 1-1/Φ = 2-Φ
    const double PHISQUARE = 1.0 + PHI;             // Φ²    = Φ+1
    const double PHI_MINSQ = 5.0 - 3*PHI;           // Φ-minor of Φ-minor : (2-Φ)²= 2²-4Φ + Φ²
    
    const double BAR_WIDTH = PHI_MINOR;             // the main (bold) vertical bar line is right aligned to axis
    const double BAR_LEFT  = -BAR_WIDTH;
    const double LIN_WIDTH = PHI_MINSQ;             // thin line is Φ-minor of bold line (which itself is Φ-minor)
    const double LIN_LEFT  = PHI_MAJOR - LIN_WIDTH; // main line and thin line create a Φ-division
    
    const double SQUARE_TIP_X = PHISQUARE - PHI_MINOR;
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
     * Setup the base metric for this bracket drawing based on CSS styling.
     * @remark the width of the double line is used as foundation to derive
     *     further layout properties, based on the golden ratio.
     * @return scale factor to apply to the base layout
     */
    double
    baseWidth (StyleC style)
    {
      return BASE_WIDTH_PER_EM * getAbsoluteFontSize (style);
    }
    
    /**
     * determine the base metric, taking into account the available canvas size.
     * @param style CSS style context where this calculation shall apply
     * @param givenHeight the allocated vertical space for the drawing
     * @return scale factor to apply to the design of the bracket
     * @remark the design is anchored at the line width, and other parts
     *         are related by golden ratio Φ. Notably the bounding box of the
     *         top and bottom cap is defined as Φ² times the base width.
     *         Consequently the drawing requires a minimum height of two times
     *         this bounding box (for top and bottom cap); in case the given
     *         height allocation is not sufficient, the whole design will be
     *         scaled down to fit.
     * @see #baseWidth(StyleC) the desired base width, as derived from font size
     */
    double
    determineScale (StyleC style, int givenHeight)
    {
      auto required = 2*PHISQUARE + style->get_padding().get_top()
                                  + style->get_padding().get_bottom();
      auto maxScale = givenHeight / (required);
      return min (maxScale, baseWidth (style));
    }
    
    /** @return width in pixels required to realise the bracket construction,
     *          taking into account the possible vertical limitation
     * @param givenHeight vertical limitation in (device) pixels
     * @remark  actually #determineScale is responsible to observe limitations
     */
    int
    calcRequiredWidth (StyleC style, int givenHeight)
    {
      return ceil (PHISQUARE * determineScale (style,givenHeight)
                  +style->get_padding().get_right()
                  +style->get_padding().get_left()
                  );
    }
    
    /** @return width for the drawing, without considering height limitation */
    int
    calcDesiredWidth (StyleC style)
    {
      return ceil (PHISQUARE * baseWidth (style)
                  +style->get_padding().get_right()
                  +style->get_padding().get_left()
                  );
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
    drawCap (CairoC cox, Gdk::RGBA colour, double ox, double oy, double scale, bool upside=true)
    {
      cox->save();
      cox->translate (ox,oy);
      cox->scale (scale, upside? scale:-scale);
      cox->set_source_rgba(colour.get_red()
                          ,colour.get_green()
                          ,colour.get_blue()
                          ,colour.get_alpha());
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
    drawBar (CairoC cox, Gdk::RGBA colour, double leftX, double upperY, double lowerY, double scale)
    {
      cox->save();
      cox->translate (leftX, upperY);
      cox->scale (scale, scale);
      cox->set_source_rgba(colour.get_red()
                          ,colour.get_green()
                          ,colour.get_blue()
                          ,colour.get_alpha());
      //
      double height = max (0.0, (lowerY - upperY)/scale);
      cox->rectangle(BAR_LEFT, -SQUARE_MINOR, BAR_WIDTH, height + 2*SQUARE_MINOR);
      cox->rectangle(LIN_LEFT,           ORG, LIN_WIDTH, height);
      //
      cox->fill();
      //
      cox->restore();
    }
    
    /**
     * Indicate connection to nested sub-Track scopes.
     * Draw a connector dot at each joint, and a arrow
     * pointing towards the nested StaveBracket top.
     * @todo simplistic implementation as of 3/23;
     *       could be made expandable /collapsable
     */
    void
    connect (CairoC cox, Gdk::RGBA colour
            ,double leftX, double upperY, double lowerY, double width, double scale
            ,std::vector<uint> connectors)
    {
      double limit = lowerY - upperY;
      double line = leftX + scale*(LIN_LEFT + LIN_WIDTH/2);
      double rad = scale * PHI_MAJOR;
      cox->save();
      // shift connectors to join below top cap
      cox->translate (line, upperY);
      // fill circle with a lightened yellow hue
      cox->set_source_rgb(1 - 0.2*(colour.get_red())
                         ,1 - 0.2*(colour.get_green())
                         ,1 - 0.5*(1 - colour.get_blue()) );
      // draw a circle joint on top of the small vertical line
      for (uint off : connectors)
        if (off <= limit)
          {
            cox->move_to(rad,off);
            cox->arc    (  0,off, rad, 0, 2 * M_PI);
            cox->close_path();
          }
      //
      cox->fill_preserve();
      cox->set_source_rgba(colour.get_red()
                          ,colour.get_green()
                          ,colour.get_blue()
                          ,colour.get_alpha());
      cox->set_line_width(scale*LIN_WIDTH*PHI_MAJOR);
      cox->stroke();
      //
      // draw connecting arrows...
      cox->translate(rad,0);
      // Note: arrow tip uses complete width, reaches into the padding-right
      double len = width-line-rad-1; // -1 to create room for a sharp miter
      ASSERT (len > 0);
      double arr = len * PHI_MINOR;
      double bas = scale * PHI_MINOR;
      for (uint off : connectors)
        if (off <= limit)
          {
            cox->move_to(ORG,off);
            cox->line_to(arr,off);
            // draw arrow head...
            cox->move_to(arr,off-bas);
            cox->line_to(len,off);
            cox->line_to(arr,off+bas);
            cox->close_path();
          }
      cox->set_miter_limit(20); // to create sharp arrow tip
      cox->fill_preserve();
      cox->stroke();
      //
      cox->restore();
    }
    
  }//(End)Implementation details (drawing design)
  
  
  
  
  
  
  StaveBracketWidget::~StaveBracketWidget() { }
  
  StaveBracketWidget::StaveBracketWidget ()
    : _Base{}
    , connectors_{}
    {
      get_style_context()->add_class (CLASS_fork_bracket);
      this->property_expand() = false;
    }
  
  
  /**
   * Custom drawing: a »stave bracket« to indicate track scope.
   * The layout is controlled by settings in the CSS style context
   * - a _base width_ (which is the width of the vertical double bar)
   *   is based on current font settings, with scale #BASE_WIDTH_PER_EM
   * - this base width also defines the width requirement reported to
   *   GTK through #get_preferred_width_for_height_vfunc
   * - possible padding is picket up from CSS
   * - current text colour is used for drawing
   */
  bool
  StaveBracketWidget::on_draw (CairoC cox)
  {
    // invoke (presumably empty) base implementation....
    bool event_is_handled = _Base::on_draw (cox);
    
    StyleC style = this->get_style_context();
    auto  colour = style->get_color (Gtk::STATE_FLAG_NORMAL);
    int   height = this->get_allocated_height();
    int    width = this->get_width();
    double scale = determineScale (style, height);
    double  left = anchorLeft (style, scale);
    double upper = anchorUpper (style,scale);
    double lower = anchorLower (style, scale, height);
    
    drawCap (cox, colour, left, upper, scale, true);
    drawCap (cox, colour, left, lower, scale, false);
    drawBar (cox, colour, left, upper, lower, scale);
    connect (cox, colour, left, upper, lower, width, scale, connectors_);
    
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
   * but requires a proportional horizontal size for proper drawing.
   * The horizontal requisition is based on the font in CSS style context.
   */
  void
  StaveBracketWidget::get_preferred_width_for_height_vfunc (int givenHeight, int& minimum_width, int& natural_width)  const
  {
    StyleC style = this->get_style_context();
    minimum_width = natural_width = calcRequiredWidth (style, givenHeight);
  }
  
  void
  StaveBracketWidget::get_preferred_width_vfunc (int& minimum_width, int& natural_width)  const
  {
    StyleC style = this->get_style_context();
    minimum_width = natural_width = calcDesiredWidth (style);
  }
  
  
}}// namespace stage::timeline
