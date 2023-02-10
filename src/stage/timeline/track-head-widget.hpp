/*
  TRACK-HEAD-WIDGET.hpp  -  display of track heads within the timeline

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


/** @file track-head-widget.hpp
 ** Widget to represent a track head with placement parameters, within the timeline header pane.
 ** [The fork](\ref session::Fork), a recursively nested system of scopes, is rendered within
 ** the timeline display as a system of nested tracks. Each of these tracks possibly holds some
 ** child tracks plus some actual media clips, which all inherit parameters of placement from
 ** this fork ("track"). These parameters address various aspects of how content is attached
 ** ("placed") into the model at large, examples being
 ** - how to route the output
 ** - how to "place" this content into an _output space_, like e.g.
 **   + sound panning
 **   + video overlay parameters (additive, opaque, transparent)
 **   + video or audio _level_ (=fader)
 ** - how to locate this content in time (e.g. relative to some marker)
 ** For each track, we display a "patchbay"-like content control in the timeline header pane,
 ** which serves to control such aspects relevant for all content contained within the scope
 ** of this track, including the sub-tracks nested therein.
 ** 
 ** @todo WIP-WIP-WIP as of 10/2018
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_HEAD_WIDGET_H
#define STAGE_TIMELINE_TRACK_HEAD_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/ctrl/bus-term.hpp"
#include "stage/model/view-hook.hpp"
#include "stage/widget/element-box-widget.hpp"
#include "stage/timeline/stave-bracket-widget.hpp"




namespace stage  {
namespace timeline {
  
  using ID = ctrl::BusTerm::ID;
  
  class HeadControlArea
    : public Gtk::Grid
    {
      Gtk::Label ctrlTODO_;
      
    public:
      HeadControlArea();
     ~HeadControlArea();
    };
  
  /**
   * Header pane control area corresponding to a Track with nested child Tracks.
   * This structure is used recursively to build up the Fork of nested Tracks.
   * - first row: Placement + Property pane
   * - second row: content or nested tracks.
   * @todo WIP-WIP as of 12/2016
   */
  class TrackHeadWidget
    : public Gtk::Grid
    , public model::ViewHook<TrackHeadWidget>
    {
      widget::ElementBoxWidget trackName_;
      StaveBracketWidget structure_;
      Gtk::Box   padding_;
      HeadControlArea headCtrl_;
      
      uint childCnt_;
      
      /* ==== Interface: ViewHook ===== */
      
      void hook   (TrackHeadWidget&)          override;
      void remove (TrackHeadWidget&)          override;
      void rehook (TrackHeadWidget&) noexcept override;
      
    public:
      TrackHeadWidget();
     ~TrackHeadWidget();
      
      void setTrackName (cuString&);
      
      uint getContentHeight() const;
      uint getOverallHeight() const;
      
      void accommodateContentHeight(uint);
      void accommodateOverallHeight(uint);
      void syncSubtrackStartHeight (uint);

    private:/* ===== Internals ===== */
      
      /** Integrate the control area for a nested sub track fork. */
      void attachSubFork (TrackHeadWidget& subForkHead);
      void detachSubFork (TrackHeadWidget& subForkHead);

      /** Discard all nested sub track display widgets. */
      void clearFork();
      
      /** get the height allocated at cell(x,y) */
      uint getHeightAt (int left, int top)  const;
      void enforceHeightAt(int left, int top, uint height);

      uint getExpansionHeight() const { return getHeightAt (0,1); };
      uint getSyncPadHeight()   const { return getHeightAt (1,2); };
      uint getLabelHeight()     const { return getHeightAt (0,0); };
      
      void enforceContentHeight  (uint h){ enforceHeightAt (1,1, h); }
      void enforceSyncPadHeight  (uint h){ enforceHeightAt (1,2, h); }
      void enforceExpansionHeight(uint h){ enforceHeightAt (0,1, h); }
    };

  
  /** @remark the cell(1,1) is guaranteed to exist; it may be empty,
   *          or hold the placement controls for this track's scope. */
  inline uint
  TrackHeadWidget::getContentHeight() const
  {
    return getHeightAt (1,1);
  }
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_HEAD_WIDGET_H*/
