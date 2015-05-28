/*
  timeline-clip-track.hpp  -  Declaration of the timeline clip track object

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

*/
/**
 * @file widgets/timeline/timeline-clip-track.hpp
 * This file contains the definition of timeline track objects which
 * contain clips.
 */

#ifndef TIMELINE_CLIP_TRACK_HPP
#define TIMELINE_CLIP_TRACK_HPP

#include "gui/widgets/timeline/basic-draw-strategy.hpp"
#include "gui/widgets/timeline/timeline-track.hpp"
#include "gui/model/clip-track.hpp"
#include "lib/time/timevalue.hpp"

#include <vector>

namespace gui {
namespace widgets {
namespace timeline {
  
  using lib::time::Time;
  
  class Clip;
  class TimelineViewWindow;
  
  
  class ClipTrack : public timeline::Track
  {
  public:
    /**
     * Constructor.
     */
    ClipTrack(TimelineWidget &timelineWidget,
              shared_ptr<model::ClipTrack> track);
  
    /**
     * Draw the track in the timeline.
     */
    void
    draw_track(Cairo::RefPtr<Cairo::Context> cairo,
      TimelineViewWindow* const window) const;

    /**
     * Gets the clip that is occupying the given time. If there is no track, return a NULL
     * pointer.
     * @param the given time
     */
    shared_ptr<timeline::Clip>
    getClipAt(Time position) const;

  private:

    /**
     * Ensures timeline UI clips have been created for every model clip in track.
     */
    void
    createTimelineClips();

    /**
     * Gets the modelTrack as a ClipTrack.
     */
    shared_ptr<model::ClipTrack>
    getModelTrack ();

    /**
     * An event handler that receives notifications for when the models clip list has been
     * changed.
     */
    void
    onClipListChanged();

    /**
     * Removes any UI clips which no longer have corresponding model clips present in the
     * sequence.
     */
    void
    removeOrphanedClips();

    /**
     * Update the attached timeline clips.
     */
    void
    updateClips();

    /**
     * The clipMap maps model clips to timeline widget clips which are responsible for the
     * UI representation of a clip.
     */
    std::map<shared_ptr<model::Clip>,
             shared_ptr<timeline::Clip> >
      clipMap;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_CLIP_TRACK_HPP
