/*
  TimelineClipTrack  -  Implementation of the timeline clip track object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file timeline-clip-track.cpp
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/timeline-clip.hpp"
#include "stage/widget/timeline/timeline-clip-track.hpp"
#include "stage/widget/timeline/timeline-view-window.hpp"

#include <boost/foreach.hpp>

using namespace Gtk;         ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using std::shared_ptr;
using util::contains;

namespace stage {
namespace widget {
namespace timeline {
  
  ClipTrack::ClipTrack (TimelineWidget &timelineWidget,
                        shared_ptr<model::ClipTrack> track)
    : Track(timelineWidget, track)
    {
      REQUIRE (track);
      
      // Connect signals
      track->getClipList().signal_changed().connect(
        sigc::mem_fun(this, &ClipTrack::onClipListChanged));
      
      updateClips();
    }
  
  void
  ClipTrack::draw_track (Cairo::RefPtr<Cairo::Context> cairo,
                         TimelineViewWindow* const window) const
  {
    REQUIRE (cairo);
    REQUIRE (window);
    
    // Draw a rectangle to let us know it works? :-)
    cairo->rectangle(window->time_to_x(Time::ZERO), 1,
      window->time_to_x(Time(500,0)) - window->time_to_x(Time::ZERO),
      get_height() - 2);
    
    cairo->set_source_rgb(0.5, 0.5, 0.5);
    cairo->fill_preserve();
    
    cairo->set_source_rgb(0.25, 0.25, 0.25);
    cairo->stroke();
    
    // Draw all clips
    std::pair<shared_ptr<model::Clip>, shared_ptr<timeline::Clip>> pair; 
    BOOST_FOREACH (pair, clipMap)
      {
        pair.second->draw(cairo, window);
      }
  }
  
  shared_ptr<timeline::Clip>
  ClipTrack::getClipAt(Time position) const
  {
    std::pair<shared_ptr<model::Clip>, shared_ptr<timeline::Clip>>
      pair; 
    BOOST_FOREACH (pair, clipMap)
      {
        if (pair.first->isPlayingAt (position))
          return pair.second;
      }
    
    // Nothing found
    return shared_ptr<timeline::Clip>();
  }
  
  //// private methods
  
  void
  ClipTrack::createTimelineClips()
  {
    // Share the draw strategy between all objects
    TODO("Use factory/builder to create Timline Clips");
    static shared_ptr<timeline::DrawStrategy> drawStrategy(new BasicDrawStrategy());
    BOOST_FOREACH (shared_ptr<model::Clip> modelClip, getModelTrack()->getClipList())
      {
        // Is a timeline UI clip present in the map already?
        if (!contains (clipMap, modelClip))
          {
            // The timeline UI clip is not present
            // We will need to create one
            clipMap[modelClip] = shared_ptr<timeline::Clip>(
              new timeline::Clip (modelClip, drawStrategy));
          }
      }
  }
  
  shared_ptr<model::ClipTrack>
  ClipTrack::getModelTrack ()
  {
    return std::dynamic_pointer_cast<model::ClipTrack>(modelTrack);
  }
  
  void
  ClipTrack::onClipListChanged ()
  {
    updateClips ();
  }
  
  void
  ClipTrack::removeOrphanedClips ()
  {
    std::map< shared_ptr<model::Clip>,
              shared_ptr<timeline::Clip>
            >
      orphanClipMap (clipMap);
    
    // Remove all clips which are still present in the sequence
    BOOST_FOREACH (shared_ptr<model::Clip> modelClip, getModelTrack()->getClipList())
      if (contains (orphanClipMap, modelClip))
        orphanClipMap.erase(modelClip);
    
    // orphanClipMap now contains all the orphaned clips
    // Remove them
    std::pair<shared_ptr<model::Clip>, shared_ptr<timeline::Clip>>
      pair; 
    BOOST_FOREACH (pair, orphanClipMap)
      {
        ENSURE (pair.first);
        clipMap.erase (pair.first);
      }
  }
  
  void
  ClipTrack::updateClips()
  {
    // Remove any clips which are no longer present in the model
    removeOrphanedClips ();
    
    // Create timeline clips from all the model clips
    createTimelineClips ();
  }
  
  
}}}// namespace stage::widget::timeline
