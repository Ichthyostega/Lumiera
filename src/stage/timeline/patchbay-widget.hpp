/*
  PATCHBAY-WIDGET.hpp  -  display of the patchbay to control track placement parameters

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

*/


/** @file patchbay-widget.hpp
 ** Widget to hold the track head controls within the timeline header pane UI.
 ** [The fork](\ref session::Fork), a recursively nested system of scopes, is rendered within
 ** the timeline display as a system of nested tracks. For each of these tracks we get a header
 ** section, allowing to control its placement parameters, including start time, output routing
 ** level and panning. The _Patch Bay_ is the container holding all those track header controls,
 ** arranged into a recursively nested structure. Besides that, the Patch Bay serves a secondary
 ** concern, namely to present this nested structure with proper vertical scrolling, so to keep
 ** each Track Head aligned with the display of the corresponding track's content.
 ** 
 ** @todo WIP-WIP-WIP as of 10/2018
 ** 
 */


#ifndef STAGE_TIMELINE_PATCHBAY_WIDGET_H
#define STAGE_TIMELINE_PATCHBAY_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using PAdjustment = Glib::RefPtr<Gtk::Adjustment>;
  
  class TrackHeadWidget;

  
  /**
   * Header pane control area corresponding to fork of nested tracks.
   * The structure of child tracks is built up recursively, starting with a single
   * top level TrackHeadWidget corresponding to the »fork root«. The actual controls
   * for the individual tracks are managed by those TrackHeadWidget elements, while
   * the PatchbayWidget corresponds to the whole structure and is responsible for
   * presenting the proper vertical scrolling, as dictated by the actual track
   * content within the BodyCanvasWidget sitting at the right side of the
   * HeaderPaneWidget (which is the enclosing container of this PatchbayWidget).
   * @todo WIP-WIP as of 4/2019
   */
  class PatchbayWidget
    : public Gtk::Viewport
    {
    public:
      PatchbayWidget (PAdjustment const& vScroll);
     ~PatchbayWidget();
      
      /** @internal Initially install the contents corresponding to the track fork root */
      void installFork (TrackHeadWidget& rootTrackHead);
      void disable();
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_PATCHBAY_WIDGET_H*/
