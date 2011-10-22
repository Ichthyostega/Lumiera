/*
  timeline-state.cpp  -  Implementation of the timeline state object

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


#include "gui/widgets/timeline/timeline-state.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/mutation.hpp"

using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {
namespace timeline {

using lib::time::FSecs;
using lib::time::Offset;
using lib::time::Duration;
using lib::time::Mutation;
using std::tr1::shared_ptr;



TimelineState::TimelineState (shared_ptr<model::Sequence> source_sequence)
  : sequence(source_sequence)
  , viewWindow(Offset(Time::ZERO), 1)
  , selection_(Time::ZERO, Duration::NIL)
  , playbackPeriod_(Time::ZERO, Duration::NIL)
  , playbackPoint_(Time::ZERO)
  , isPlayback_(false)
{
  REQUIRE(sequence);
  
  ////////////////////////////////////////////////////////////TICKET #798: how to handle GUI default state
  const int64_t DEFAULT_TIMELINE_SCALE =21000000;
  
  viewWindow.set_time_scale(DEFAULT_TIMELINE_SCALE);

  setSelection (Mutation::changeTime (Time(FSecs(2))));
  setSelection (Mutation::changeDuration(Duration(FSecs(2))));
  //////////////////////////////////////////////////////TICKET #797 : this is cheesy. Should provide a single Mutation to change all
}

shared_ptr<model::Sequence>
TimelineState::get_sequence() const
{
  return sequence;
}

TimelineViewWindow&
TimelineState::get_view_window()
{
  return viewWindow;
}

void
TimelineState::setSelection (Mutation const& change,
                             bool resetPlaybackPeriod)
{ 
  selection_.accept (change);
  if (resetPlaybackPeriod)
    setPlaybackPeriod(change);
  
  selectionChangedSignal.emit();
}
  
void
TimelineState::setPlaybackPeriod (Mutation const& change)
{
  playbackPeriod_.accept (change);
  playbackChangedSignal.emit();
}

void
TimelineState::setPlaybackPoint (Time newPosition)
{
  playbackPoint_ = newPosition;
  playbackChangedSignal.emit();
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

