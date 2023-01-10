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
#include "lib/format-cout.hpp"/////////////////////////////TODO 4dump

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;
using util::max;


namespace stage {
namespace timeline {
  
  
  
  
  TrackHeadWidget::~TrackHeadWidget() { }
  HeadControlArea::~HeadControlArea() { }
  
  
  
  TrackHeadWidget::TrackHeadWidget()
    : Gtk::Grid{}
    , nameTODO_{"?"}
    , treeTODO_{"↳"}
    , headCtrl_{}
    , childCnt_{0}
    {
      nameTODO_.set_xalign(0);
      nameTODO_.set_yalign(0);
      treeTODO_.set_xalign(0);
      treeTODO_.set_yalign(0.5);
      this->attach (nameTODO_, 0,0, 2,1);
      this->attach (treeTODO_, 0,1, 1,1);
      attachDirectContent();
      
      this->show_all();
    }
  
  
  HeadControlArea::HeadControlArea()
    : Gtk::Grid{}
    , ctrlTODO_{"💡"}
    {
      ctrlTODO_.set_xalign (0.0);
      ctrlTODO_.set_yalign (0.5);
      this->attach (ctrlTODO_, 0,0, 1,1);
      // dynamically grab any available additional space
      ctrlTODO_.property_expand() = true;
      this->show_all();
    }
  
  
  void
  TrackHeadWidget::setTrackName (cuString& trackName)
  {
    nameTODO_.set_label (trackName);
  }
  
  uint
  TrackHeadWidget::getHeightAt (int left, int top)  const
  {
    auto* cell = this->get_child_at(left,top);
    if (cell == nullptr) return 0;
    int actual = cell->get_height();
    int minimal=0, natural=0;
    cell->get_preferred_height(minimal, natural);
    return max (0, max (actual, natural));
  }

  /**
   * @remark the cell(1,1) is guaranteed to exist;
   *  if childCnt_ == 0, it holds the direct content control area
   */
  uint
  TrackHeadWidget::calcContentHeight() const
  {
    uint heightSum = 0;
    for (uint line=1; line <= max(1u, childCnt_); ++line)
      {
        int h = getHeightAt (1,line);
        heightSum += max (0,h);
      }
    return heightSum;
  }
  
  uint
  TrackHeadWidget::calcOverallHeight()  const
  {
    uint heightSum = 0;
    for (uint line=0; line <= max(1u, childCnt_); ++line)
      {
        int h1 = getHeightAt (0,line);
        int h2 = getHeightAt (1,line);
        
        heightSum += max (0, max (h1,h2));
      }
    return heightSum;
  }
  
  void
  TrackHeadWidget::accommodateContentHeight(uint contentHeight)
  {
    uint localHeight = calcContentHeight();
    if (contentHeight > localHeight)
      increaseContentHeight (contentHeight-localHeight);
  }
  
  /** apply the Δ to the »content area« (4th quadrant) */
  void
  TrackHeadWidget::increaseContentHeight(uint delta)
  {
    increaseHeightAt (1,1, delta);
  }
  
  /** increase the general vertical spread by the given Δ  */
  void
  TrackHeadWidget::increaseExpansionHeight(uint delta)
  {
    increaseHeightAt (0,1, delta);
  }
  
  void
  TrackHeadWidget::increaseHeightAt(int left, int top, uint delta)
  {
uint hvor = calcOverallHeight();      
    auto* cell = this->get_child_at(left,top);
    REQUIRE (cell);
    uint h = getHeightAt (left,top);
    cell->set_size_request (-1, h+delta);
uint hnach = calcOverallHeight();      
cout<<"|+| Head:inc ("<<left<<","<<top<<") h="<<h<<" Δ="<<delta<<" vor:"<<hvor<<" nach:"<<hnach<<endl;      
  }


  
  /**
   * @remark The Lumiera Timeline model does not rely on a list of tracks, as most conventional
   * video editing software does -- rather, each sequence holds a _fork of nested scopes._
   * This recursively nested structure is reflected in the patchbay area corresponding to
   * each track in the _header pane_ of the timeline display, located to the left. The
   * patchbay for each track is a grid with initially four quadrants, and the 4th quadrant
   * corresponds to the _content area,_ in case this is a leaf track. Otherwise there would
   * be nested sub-Tracks, and this lower right grid cell would then hold a TrackHeadWidget
   * recursively. Additional sub-Tracks are added as additional lines to the grid, while
   * deeper nested sub-Tracks will be handled by the corresponding nested TrackHeadWidget.
   * @note Child tracks are always appended. When tracks are reordered or deleted,
   *       the whole structure has to be re-built accordingly.
   */
  void
  TrackHeadWidget::attachSubFork (TrackHeadWidget& subForkHead)
  {
    if (not childCnt_) detachDirectContent();
    ++childCnt_;           //  left,top
    this->attach (subForkHead, 1, childCnt_, 1,1);
  }
  
  /**
   * @internal remove a complete sub-fork from display.
   * @remarks due to the automatic ref-counting system of GTK+, it is sufficient
   *          just to remove the entry from the `Gtk::Container` baseclass, which
   *          automatically decrements the refcount; alternatively we could as well
   *          destroy the Gtkmm wrapper-Object (i.e. the `Gtk::Widget` subclass),
   *          since this also destroys the underlying `gobj` and automatically
   *          detaches it from any container. (however, here this isn't necessary,
   *          since the TrackHeadWidget is managed by the DisplayFrame)
   */
  void
  TrackHeadWidget::detachSubFork (TrackHeadWidget& subForkHead)
  {
    --childCnt_;
    Gtk::Grid::remove (subForkHead);
    if (not childCnt_) attachDirectContent();
  }
  
  
  void
  TrackHeadWidget::clearFork()
  {
    if (not childCnt_) return;
    while (childCnt_ > 0)
      {
        this->remove_row (childCnt_);
        --childCnt_;
      }
    attachDirectContent();
  }
  
  
  /**
   * @remark a _leaf track_ holds content immediately, and thus also
   *         provides a head area with placement controls directly attached.
   *         In all other cases, a nested TrackHeadWidget for the sub-fork
   *         is installed
   */
  void
  TrackHeadWidget::attachDirectContent()
  {
    headCtrl_.show();
    Gtk::Grid::attach(headCtrl_, 1,1, 1,1);
  }
  
  void
  TrackHeadWidget::detachDirectContent()
  {
    Gtk::Grid::remove (headCtrl_);
    headCtrl_.hide();
  }

  
  
  /* ==== Interface: ViewHook ===== */
  
  void
  TrackHeadWidget::hook (TrackHeadWidget& subHead)
  {
    attachSubFork (subHead);
  }

  void
  TrackHeadWidget::remove (TrackHeadWidget& subHead)
  {
    detachSubFork (subHead);
  }
  
  /** @remark This implementation will not interfere with the widget's lifecycle.
   *   The widget with all its children is just detached from presentation (leaving an
   *   empty grid cell), and immediately re-attached into the "bottom most" cell, as
   *   given by the current childCnt_
   *  @note in theory it is possible to end up with several widgets in a single cell,
   *   and GTK can handle that. Given our actual usage of these functions, such should
   *   never happen, since we manage all widgets as slave of the model::Tangible in charge.
   */
  void
  TrackHeadWidget::rehook (TrackHeadWidget& hookedSubHead)  noexcept
  {
    detachSubFork (hookedSubHead);
    attachSubFork (hookedSubHead);
  }
  
  
  
}}// namespace stage::timeline
