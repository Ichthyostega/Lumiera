/*
  timeline-state.cpp  -  Implementation of the timeline state object
 
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
 
* *****************************************************/

#include "timeline-state.hpp"

using namespace boost;
using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {
namespace timeline {

TimelineState::TimelineState(
  boost::shared_ptr<model::Sequence> source_sequence) :
  sequence(source_sequence),
  viewWindow(0, 1),
  selectionStart(0),
  selectionEnd(0),
  playbackPeriodStart(0),
  playbackPeriodEnd(0),
  playbackPoint(GAVL_TIME_UNDEFINED)
{
  REQUIRE(sequence);
  
  viewWindow.set_time_scale(GAVL_TIME_SCALE / 200);
  set_selection(2000000, 4000000);
}

boost::shared_ptr<model::Sequence>
TimelineState::get_sequence() const
{
  return sequence;
}

TimelineViewWindow&
TimelineState::get_view_window()
{
  return viewWindow;
}

gavl_time_t
TimelineState::get_selection_start() const
{
  return selectionStart;
}

gavl_time_t
TimelineState::get_selection_end() const
{
  return selectionEnd;
}

void
TimelineState::set_selection(gavl_time_t start, gavl_time_t end,
  bool reset_playback_period)
{    
  if(start < end)
    {
      selectionStart = start;
      selectionEnd = end;
    }
  else
    {
      // The selection is back-to-front, flip it round
      selectionStart = end;
      selectionEnd = start;
    }
    
  if(reset_playback_period)
    {
      playbackPeriodStart = selectionStart;
      playbackPeriodEnd = selectionEnd;
    }
    
  selectionChangedSignal.emit();
}

gavl_time_t
TimelineState::get_playback_period_start() const
{
  return playbackPeriodStart;
}
  
gavl_time_t
TimelineState::get_playback_period_end() const
{
  return playbackPeriodEnd;
}
  
void
TimelineState::set_playback_period(gavl_time_t start, gavl_time_t end)
{
  
  if(start < end)
    {
      playbackPeriodStart = start;
      playbackPeriodEnd = end;
    }
  else
    {
      // The period is back-to-front, flip it round
      playbackPeriodStart = end;
      playbackPeriodEnd = start;
    }
  
  playbackChangedSignal.emit();
}

void
TimelineState::set_playback_point(gavl_time_t point)
{
  playbackPoint = point;
  playbackChangedSignal.emit();
}

gavl_time_t
TimelineState::get_playback_point() const
{
  return playbackPoint;
}

sigc::signal<void>
TimelineState::selection_changed_signal() const
{
  return selectionChangedSignal;
}
  
sigc::signal<void>
TimelineState::playback_changed_signal() const
{
  return playbackChangedSignal;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

