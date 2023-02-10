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
 ** # Layout arrangement for the »Patchbay«
 ** 
 ** In Lumiera, »Tracks« are arranged as a system of nested scopes.
 ** The TrackHeadWidget thus has to reflect this nested structure, which can be achieved
 ** by recursively nesting further TrackHeadWidget instances. The header and »patchbay«
 ** for each scope is implemented as a `Gtk::Grid` widget, with initially three rows:
 ** - a row holding the Track Header label and menu (actually an \ref ElementBoxWidget)
 ** - a row corresponding to the _content area_ of the track itself, to hold the controls
 **   for this track's scope, i.e. the track _together with all nested sub-tracks._
 ** - a padding row to help synchronising track head and track body display.
 ** Additional sub-Tracks are added as additional lines to the grid, while deeper nested
 ** sub-Tracks will be handled by the corresponding nested TrackHeadWidget. The column
 ** to the left side will be increased accordingly to display the nested fork structure.
 ** 
 ** @todo as of 1/2023, the basic structure was (finally) settled and the painting
 **       of vertical space allocation looks correct up to ± 1px, when testing with
 **       various margin, border and padding settings in the CSS.
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/track-head-widget.hpp"

#include "lib/util.hpp"

using util::max;


namespace stage {
namespace timeline {
  
  
  
  
  TrackHeadWidget::~TrackHeadWidget() { }
  HeadControlArea::~HeadControlArea() { }
  
  
  
  TrackHeadWidget::TrackHeadWidget()
    : Gtk::Grid{}
    , trackName_{widget::Kind::ITEM, widget::Type::LABEL}
    , structure_{}
    , padding_{}
    , headCtrl_{}
    , childCnt_{0}
    {
      headCtrl_.set_valign(Gtk::Align::ALIGN_CENTER);
      headCtrl_.set_halign(Gtk::Align::ALIGN_FILL);
      this->attach (trackName_, 0,0, 2,1);
      this->attach (structure_, 0,1, 1,2);
      this->attach (headCtrl_, 1,1, 1,1); // corresponds to direct content
      this->attach (padding_,  1,2, 1,1);//  used to sync with sub-track display
      this->property_expand() = false;  //   do not expand to fill
      this->show_all();
    }
  
  
  HeadControlArea::HeadControlArea()
    : Gtk::Grid{}
    , ctrlTODO_{"💡"}
    {
      ctrlTODO_.set_xalign (0.3);
      ctrlTODO_.set_yalign (0.5);
      this->attach (ctrlTODO_, 0,0, 1,1);
      this->show_all();
    }
  
  
  void
  TrackHeadWidget::setTrackName (cuString& trackName)
  {
    trackName_.setName (trackName);
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

  uint
  TrackHeadWidget::getOverallHeight()  const
  {
    uint heightSum{0};
    for (uint line=1; line <= 2u + childCnt_; ++line)
      heightSum += getHeightAt (1,line);
    heightSum = max (heightSum, getExpansionHeight());
    return heightSum + getLabelHeight();
  }
  
  void
  TrackHeadWidget::enforceHeightAt(int left, int top, uint height)
  {
    auto* cell = this->get_child_at(left,top);
    REQUIRE (cell);
    cell->set_size_request (-1, height);
  }
  
  void
  TrackHeadWidget::accommodateContentHeight(uint contentHeight)
  {
    uint localHeight = getContentHeight();
    if (contentHeight > localHeight)
      enforceContentHeight (contentHeight);
  }
  
  void
  TrackHeadWidget::accommodateOverallHeight(uint overallHeight)
  {
    uint localHeight = getOverallHeight();
    if (overallHeight > localHeight)
      enforceExpansionHeight (overallHeight - getLabelHeight());
  }

  /**
   * The first part of each track's display relates to the direct content;
   * below that area, the content of sub-tracks may be added. This function
   * possibly adds further padding to shift the sub-track headers down
   * to correspond to the display of the sub track body content.
   * @param directHeight the vertical space to use for direct content
   */
  void
  TrackHeadWidget::syncSubtrackStartHeight (uint directHeight)
  {
    uint localHeight = getContentHeight() + getLabelHeight();
    if (directHeight > localHeight)
      enforceSyncPadHeight (directHeight - localHeight);
  }


  
  /**
   * @remark The Lumiera Timeline model does not rely on a list of tracks, as most conventional
   * video editing software does -- rather, each sequence holds a _fork of nested scopes._
   * In the _header pane,_ this recursively nested structure is reflected as nested Gtk::Grid
   * widgets, populated for each track with initially three rows:
   * - a row holding the Track Header label and menu (actually an \ref ElementBoxWidget)
   * - a row corresponding to the _content area_ of the track itself, to hold the controls
   *   to govern this track's scope, i.e. the track _together with all nested sub-tracks._
   * - a padding row to help synchronising track head and track body display.
   * - Additional sub-Tracks are added as additional lines to the grid, while deeper
   *   nested sub-Tracks will be handled by the corresponding nested TrackHeadWidget.
   * The column to the left side will be increased accordingly to display the nested fork structure.
   * @note Child tracks are always appended. When tracks are reordered or deleted,
   *       the whole structure has to be re-built accordingly.
   */
  void
  TrackHeadWidget::attachSubFork (TrackHeadWidget& subForkHead)
  {
    ++childCnt_;
    uint act = 2 + childCnt_;   //  left,top
    Gtk::Grid::attach (subForkHead, 1, act, 1,1);
    // expand the structure display column....
    Gtk::Grid::remove (structure_);   // width,height
    Gtk::Grid::attach (structure_,   0,1,    1, act);
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
    uint act = 2 + childCnt_;
    Gtk::Grid::remove (subForkHead);
    // reduce the structure display column....
    Gtk::Grid::remove (structure_);
    Gtk::Grid::attach (structure_,   0,1, 1,act);
  }
  
  
  void
  TrackHeadWidget::clearFork()
  {
    if (not childCnt_) return;
    Gtk::Grid::remove (structure_);
    while (childCnt_ > 0)
      {
        Gtk::Grid::remove_row (childCnt_);
        --childCnt_;
      }
    Gtk::Grid::attach (structure_, 0,1, 1,2);
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
