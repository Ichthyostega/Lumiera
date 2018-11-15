/*
  TIMELINE-GROUP-TRACK.hpp  -  Declaration of the timeline group track object

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


/** @file timeline-group-track.hpp
 ** Preliminary implementation of a group track within the timeline display
 ** @deprecated to be replaced by a different implementation approach
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#ifndef STAGE_WIDGET_TIMELINE_GROUP_TRACK_H
#define STAGE_WIDGET_TIMELINE_GROUP_TRACK_H

#include "stage/widget/timeline/timeline-track.hpp"
#include "stage/model/group-track.hpp"

namespace stage {
namespace widget {
namespace timeline {
  
  class GroupTrack
    : public timeline::Track
    {
    public:
      GroupTrack (TimelineWidget &timeline_widget,
                  shared_ptr<model::GroupTrack> track);
      
      void
      draw_track (Cairo::RefPtr<Cairo::Context> cairo,
                  TimelineViewWindow* constwindow)  const;
        
    protected:
      void on_child_list_changed();
    };
    
    
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_GROUP_TRACK_H*/
