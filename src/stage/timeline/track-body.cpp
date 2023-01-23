/*
  TrackBody  -  track body area within the timeline display canvas

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


/** @file track-body.cpp
 ** Implementation details regarding display management of the
 ** track body area within the timeline display canvas. Especially,
 ** this code handles the structuring of vertical space, and the way
 ** this vertical extension maps to specific tracks.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/track-body.hpp"
#include "stage/timeline/track-profile.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"/////////////////////TODO debugging output

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
using util::isnil;
using util::min;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  namespace {
    const uint DEFAULT_CONTENT_HEIGHT_px = 40;
    const uint TIMELINE_BOTTOM_PADDING_px = 5;
  }
  
  
  
  
  TrackBody::TrackBody()
    : contentHeight_{DEFAULT_CONTENT_HEIGHT_px}
    , contentOffset_{0}
    , startLine_{0}
    , subTracks_{}
    , rulers_{}
    { }
  
  
  TrackBody::~TrackBody()
  { // indicate change of the global track structure
    signalStructureChange_();
  }
  
  
  /** storage for common style/padding settings */
  Decoration TrackBody::decoration{};
  
  
  void
  TrackBody::setTrackName (cuString& trackName)
  {
    TODO ("is the track name of any relevance for the TrackBody widget?");
  }
  
  
  /* ==== Interface: ViewHook ===== */
  
  void
  TrackBody::hook (TrackBody& subBody)
  {
    subTracks_.push_back (&subBody);
    
    // notify presentation code of the changed structure
    subBody.signalStructureChange_ = signalStructureChange_;
    signalStructureChange_(); // this _is_ such a change
  }

  void
  TrackBody::remove (TrackBody& subBody)
  {
    util::removeall (subTracks_, &subBody);
    signalStructureChange_();
  }
  
  void
  TrackBody::rehook (TrackBody& subBody)  noexcept
  {
    util::removeall (subTracks_, &subBody);
    subTracks_.push_back (&subBody);
    signalStructureChange_();
  }
  
  
  /* ==== Code for vertical track display layout ==== */

  /**
   * ensure content with the given extension can be accommodated
   * within this track's content area
   */
  void
  TrackBody::accommodateContentHeight(uint contentExtension)
  {
    if (contentExtension > contentHeight_)
{//////////////////////////////////////////////////////TODO
 cout<<"|+| contentHeight("<<contentHeight_<<" ⟶ "<<contentExtension<<" this(body)="<<this<<")"<<endl;      
      contentHeight_ = contentExtension;
}//////////////////////////////////////////////////////TODO      
  }
  
  /**
   * recursively calculate the height in pixels to display this track,
   * including all nested sub-tracks and possibly decoration/padding from CSS
   * @note height attempts to account for everything (to allow sync with header),
   *       including the rulers and possibly padding on root track, which are
   *       rendered onto a separate canvas, and additionally also nested slope.
   */
  uint
  TrackBody::calcHeight()  const
  {
    return calcContentHeight()
         + calcSubtrackHeight();
  }
  
  /**
   * @remark here _content_ means the direct content of this track,
   *         plus its rulers and padding, but excluding nested tracks.
   */
  uint
  TrackBody::calcContentHeight() const
  {
    return calcRulerHeight()
         + contentHeight_ + decoration.content
         + (isnil (subTracks_)? 0  //  slope down to nested scope
                              : decoration.borders[0]);
  }
  
  /**
   * sum up the vertical extension required by all overview rulers.
   * @return height in pixels, including all gap space
   */
  uint
  TrackBody::calcRulerHeight()  const
  {                        // „insider trick“ to include prelude padding on root track...
    uint overviewHeight{startLine_== 0?  // parent adds offset to startLine_ of any sub-track
                        decoration.topMar : 0};
    for (auto& ruler : rulers_)
      {
        overviewHeight += ruler->calcHeight()
                        + ruler->getGapHeight()
                        + decoration.ruler;
      }
    return overviewHeight;
  }
  
  uint
  TrackBody::calcSubtrackHeight()  const
  {
    uint heightSum{isnil (subTracks_)? 0   // approximate slope up (possibly exaggerated)
                                     : decoration.borders[0] };
    for (TrackBody* subTrack : subTracks_)
      heightSum += subTrack->calcHeight();
    return heightSum;
  }

  
  namespace {
    /** helper to get the width of combined slope borders.
     * Upwards slopes are combined up to a certain degree;
     * however, the actual width of such a combined border
     * is defined through a class in the CSS stylesheet.
     * The TrackBody::decoration.borders array holds the
     * actual values read from the CSS.
     */
    inline uint
    combinedSlopeHeight (uint depth)
    {
      if (depth==0) return 0;
      depth = min (depth, TrackBody::decoration.borders.size() - 1);
      return TrackBody::decoration.borders[depth];
    }
  }
  
  
  /**
   * recursively establish the screen space allocation for this structure of nested tracks.
   * For one, we'll have to find out about the total vertical space for each track, so to
   * establish the vertical starting position, which is required to place clips onto the canvas.
   * 
   * Moreover we have to build the TrackProfile, which is an abstracted description of the sequence
   * of track elements, akin to a vertical cross section through the track bodies. This profile is
   * repeatedly "played back" to paint the background and overlays corresponding to each track.
   * 
   * This function recursively processes the tree of track bodies...
   * - pre: the given profile is built and complete up to the (upper side) start of the current track.
   * - post: the profile is elaborated for this track and its children, down to the lower end.
   * @return total vertical extension required for this track with all its nested sub tracks, in pixels.
   */
  uint
  TrackBody::establishTrackSpace (TrackProfile& profile)
  {
    uint line=0;
    bool topLevel = isnil (profile);
    if (topLevel)
      {
        // global setup for the profile
        line += decoration.topMar;
        profile.append_prelude();
      }
    else
      { // adjust if preceded by a combined up-slope
        line += combinedSlopeHeight (profile.getPrecedingSlopeUp());
      }
    
    // reserve space for the overview rulers
    for (auto& ruler : rulers_)
      {
        uint rulerHeight = ruler->calcHeight();
        uint gapHeight = ruler->getGapHeight();
        line += rulerHeight+gapHeight + decoration.ruler;
        profile.append_ruler (rulerHeight);
        if (gapHeight > 0)
          profile.append_gap (gapHeight);
      }
    if (topLevel)
      {
        // The first Profile elements are always visible on top;
        // we render this prefix part on a separate drawing canvas,
        profile.markPrefixEnd();
        // ...and now we switch to the second, scrollable canvas,
        // which uses its own local coordinates, thus restart Y-pos.
        line = 0;
      }
    // mark offset of the actual content area relative to this track's top
    this->contentOffset_ = line + decoration.trackPad;
    
    // allocate space for the track content
    line += this->contentHeight_ + decoration.content;
    profile.append_content (this->contentHeight_);
    
    // account for the space required by nested sub-tracks
    if (not isnil (subTracks_))
      {
        // account for a single slope one step down
        line += decoration.borders[0]; // (downward slopes are never combined)
        profile.addSlopeDown();
        
        for (TrackBody* subTrack : subTracks_)
          {
            // (re)set the subTrack's start coordinates
            // to reflect the allocation calculation done thus far
            subTrack->startLine_ = this->startLine_ + line;
            line += subTrack->establishTrackSpace (profile);
          }
        
        profile.addSlopeUp(); // note: up-slopes might be combined
      }                      //        thus we'll add their contribution
                            //         at the calling function one level higher
    if (topLevel)
      {
        // adjust when reaching top-level after a combined up-slope
        line += combinedSlopeHeight (profile.getPrecedingSlopeUp());
        
        line += decoration.botMar + TIMELINE_BOTTOM_PADDING_px;
        profile.append_coda (TIMELINE_BOTTOM_PADDING_px);
      }
    
cout<<"|*| establishTrackSpace(content="<<contentHeight_<<" res="<<line<<" this(body)="<<this<<")"<<endl;
    return line;
  }
  
  
  
}}// namespace stage::timeline
