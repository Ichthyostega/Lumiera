/*
  group-track.hpp  -  Declaration of the timeline group track object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/
/** @file widgets/timeline/group-track.hpp
 ** This file contains the definition of group track timeline objects
 */

#ifndef GROUP_TRACK_HPP
#define GROUP_TRACK_HPP

#include "track.hpp"

namespace gui {
namespace widgets {
namespace timeline {
  
class GroupTrack : public Track
{
public:
  GroupTrack();
  
  void add_child_track(timeline::Track* child);
  
  const std::vector<timeline::Track*>& get_child_tracks() const;
  
  virtual void draw_track(Cairo::RefPtr<Cairo::Context> cairo,
    TimelineViewWindow* constwindow)
    const;
    
private:
  //----- Data -----//
  std::vector<timeline::Track*> children;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // GROUP_TRACK_HPP
