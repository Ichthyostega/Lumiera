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
 ** The relation to nested stave brackets for sub-Tracks can be indicated with a
 ** connection joint and arrow; prerequisite is to provide the vertical offset.
 ** 
 ** \par styling
 ** - styling is controlled via CSS, using the marker class \ref CLASS_fork_bracket
 ** - the »base width« of the vertical double line is based on the font's `em` setting
 ** - padding and colour attributes from CSS are observed
 ** 
 ** @see TrackHeadWidget::structure_
 ** 
 */


#ifndef STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H
#define STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H

#include "stage/gtk-base.hpp"

#include <vector>


namespace stage  {
namespace timeline {
  
  using CairoC = PCairoContext const&;
  using StyleC = PStyleContext const&;
  
  
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
      std::vector<uint> connectors_;
      
    public:
     ~StaveBracketWidget();
      StaveBracketWidget();
      
      void clearConnectors();
      void addConnector (uint offset);
      
    private:/* ===== Internals ===== */
      
      bool on_draw(CairoC cox)  override;
      
      Gtk::SizeRequestMode get_request_mode_vfunc()              const final;
      void get_preferred_width_vfunc (int&, int&)                const override;
      void get_preferred_width_for_height_vfunc (int, int&,int&) const override;
    };
  
  
  
  inline void
  StaveBracketWidget::clearConnectors()
  {
    connectors_.clear();
  }
  
  /**
   * Request to draw a connector to the nested sub-Track's stave bracket.
   * @param offset vertical location where the sub-Track starts, relative
   *        to the start of this stave bracket's start
   * @remark called from the 2nd DisplayEvaluation pass, when linking the layout
   * @see TrackHeadWidget::linkSubTrackPositions
   */
  inline void
  StaveBracketWidget::addConnector (uint offset)
  {
    connectors_.emplace_back (offset);
  }
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_STAVE_BRACKET_WIDGET_H*/
