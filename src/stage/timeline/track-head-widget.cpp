/*
  TrackHeadWidget  -  display of track heads within the timeline

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


/** @file track-head-widget.cpp
 ** Implementation details regarding display of the track heads within the timeline UI.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/track-head-widget.hpp"

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
namespace timeline {
  
  
  
  
  TrackHeadWidget::~TrackHeadWidget() { }
  
  
  
  TrackHeadWidget::TrackHeadWidget()
    : Gtk::Grid{}
    , nameTODO_{"?"}
    , treeTODO_{"↳"}
    , childCnt_{0}
    {
      this->attach (nameTODO_, 0,0, 2,1);
      this->attach (treeTODO_, 0,1, 1,1);
      
      this->show_all();
    }
  
  
  void
  TrackHeadWidget::setTrackName (cuString& trackName)
    {
      nameTODO_.set_label (trackName);
    }
  
  /**
   * @remark The Lumiera Timeline model does not rely on a list of tracks, as most conventional
   * video editing software does -- rather, each sequence holds a _fork of nested scopes._
   * This recursively nested structure is reflected in the patchbay area corresponding to
   * each track in the _header pane_ of the timeline display, located to the left. The
   * patchbay for each track is a grid with initially four quadrants, and the 4th quadrant
   * holds the _content area,_ which is again a TrackHeadWidget. Additional sub-Tracks
   * are added as additional lines to the grid, while nested sub-Tracks will be handled
   * recursively by the corresponding nested TrackHeadWidget.
   * @note Child tracks are always appended. When tracks are reordered or deleted,
   *       the whole structure has to be re-built accordingly.
   */
  void
  TrackHeadWidget::injectSubFork (TrackHeadWidget& subForkHead)
  {
    ++childCnt_;
    this->attach (subForkHead, 1, childCnt_, 1,1);
  }
  
  
  void
  TrackHeadWidget::clearSubFork()
  {
    while (childCnt_ > 0)
      {
        this->remove_row (childCnt_);
        --childCnt_;
      }
  }
  
  
  
}}// namespace stage::timeline
