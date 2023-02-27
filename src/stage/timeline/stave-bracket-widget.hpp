/*
  STAVE-BRACKET-WIDGET.hpp  -  draw connector to indicate nested track structure

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

*/


/** @file stave-bracket-widget.hpp
 ** Widget to group tracks visually in the Timeline presentation.
 ** The TrackHeadWidget is structured according to the nested structure of tracks
 ** in the Fork, using nested widgets to represent sub-tracks; in the grid's first column
 ** a graphical structure is rendered to indicate the nesting and connectivity of tracks.
 ** While based on few rather simple visual primitives, this structure need to adapt to
 ** the actual space allocation and positioning of sub-tracks in the layout; technicalities
 ** of actual drawing this structure is abstracted into this custom widget — allowing the
 ** track head to indicate the necessary layout constraints generic and recursively.
 ** 
 ** @todo WIP-WIP-WIP as of 2/2023
 ** 
 */


#ifndef STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H
#define STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H

#include "stage/gtk-base.hpp"
//#include "stage/model/controller.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using CairoC = PCairoContext const&;
  using StyleC = PStyleContext const&;

//  class TrackPresenter;
  
  
  /**
   * Helper widget to handle display of the structure of track nesting in the timeline
   * header pane. For each TrackHeadWidget, the connected cells in the first grid column
   * are populated with this component, indicating connectivity and layout constraints.
   * The actual drawing is based on Cairo, and implemented in the virtual draw() function.
   */
  class StaveBracketWidget
    : public Gtk::DrawingArea
    {
      using _Base = Gtk::DrawingArea;
      
    public:
     ~StaveBracketWidget();
      
      StaveBracketWidget ();
      
    private:/* ===== Internals ===== */
      
      bool on_draw(CairoC cox)  override;
      
      Gtk::SizeRequestMode get_request_mode_vfunc()              const final;
      void get_preferred_width_vfunc (int&, int&)                const override;
      void get_preferred_width_for_height_vfunc (int, int&,int&) const override;
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H*/
