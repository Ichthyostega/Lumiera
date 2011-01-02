/*
  timeline-clip-track.cpp  -  Implementation of the timeline clip track object

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

#include <boost/foreach.hpp>

#include "timeline-clip.hpp"
#include "timeline-clip-track.hpp"
#include "timeline-view-window.hpp"

using namespace Gtk;
using boost::dynamic_pointer_cast;
using boost::shared_ptr;
using util::contains;

namespace gui {
namespace widgets {
namespace timeline {

  ClipTrack::ClipTrack (TimelineWidget &timelineWidget,
    shared_ptr<model::ClipTrack> track) :
    Track(timelineWidget, track)
  {
    REQUIRE (track);

    // Connect signals
    track->getClipList().signal_changed().connect(
      sigc::mem_fun(this, &ClipTrack::onClipListChanged));

    updateClips();
  }

  void
  ClipTrack::draw_track (
    Cairo::RefPtr<Cairo::Context> cairo,
    TimelineViewWindow* const window) const
  {
    REQUIRE (cairo);
    REQUIRE (window);

    // Draw a rectangle to let us know it works? :-)
    cairo->rectangle(window->time_to_x(0), 1,
      window->time_to_x(500000) - window->time_to_x(0),
      get_height() - 2);

    cairo->set_source_rgb(0.5, 0.5, 0.5);
    cairo->fill_preserve();
  
    cairo->set_source_rgb(0.25, 0.25, 0.25);
    cairo->stroke();

    // Draw all clips
    std::pair<shared_ptr<model::Clip>, shared_ptr<timeline::Clip> >
      pair; 
    BOOST_FOREACH (pair, clipMap)
      {
        pair.second->draw_clip(cairo, window);
      }
  }

  //// private methods

  void
  ClipTrack::createTimelineClips()
  {
    BOOST_FOREACH (shared_ptr<model::Clip> modelClip, getModelTrack()->getClipList())
      {
        // Is a timeline UI clip present in the map already?
        if (!contains (clipMap, modelClip))
          {
            // The timeline UI clip is not present
            // We will need to create one
            clipMap[modelClip] = shared_ptr<timeline::Clip>(
              new timeline::Clip (modelClip));
          }
      }
  }

  shared_ptr<model::ClipTrack>
  ClipTrack::getModelTrack ()
  {
    return dynamic_pointer_cast<model::ClipTrack>(model_track);
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
              shared_ptr<timeline::Clip> >
      orphanClipMap (clipMap);

    // Remove all clips which are still present in the sequence
    BOOST_FOREACH (shared_ptr<model::Clip> modelClip, getModelTrack()->getClipList())
      if (contains (orphanClipMap, modelClip))
        orphanClipMap.erase(modelClip);

    // orphanClipMap now contains all the orphaned clips
    // Remove them
    std::pair< shared_ptr<model::Clip>, shared_ptr<timeline::Clip> >
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

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
