/*
  TimelineState  -  timeline presentation state holder

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline-state.cpp
 ** Implementation of presentation state for the first draft of the timeline display.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 ** @deprecated this is defunct code, left over from the old GTK-2 GUI of Lumiera.
 **    Deactivated since 2016 and no longer included since 3/23 but left in tree
 **    for later reference
 */


#include "stage/widget/timeline/timeline-state.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/mutation.hpp"
#include "lib/time/control.hpp"

using namespace Gtk;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace sigc;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace widget {
namespace timeline {
  
  using lib::time::FSecs;
  using lib::time::Offset;
  using lib::time::Duration;
  using lib::time::Mutation;
  using lib::time::Control;
  using std::shared_ptr;
  
  
  TimelineState::TimelineState (shared_ptr<model::Sequence> sourceSequence)
    : sequence_(sourceSequence)
    , viewWindow_(Offset(Time::ZERO), 1)
    , selection_(Time::ZERO, Duration::NIL)
    , selectionListener_()
    , playbackPeriod_(Time::ZERO, Duration::NIL)
    , playbackPoint_(Time::ZERO)
    , isPlayback_(false)
    {
      REQUIRE(sequence_);
      
      // Initialise the selection listener
      selectionListener_ (TimeSpan (Time::ZERO, Duration::NIL));
      selectionListener_.connect(
          mem_fun(*this, &TimelineState::on_selection_changed));
      
      ////////////////////////////////////////////////////////////TICKET #798: how to handle GUI default state
      const int64_t DEFAULT_TIMELINE_SCALE =6400;
  
      NOTREACHED ("3/23: this code is dead and left in-tree for later reference only");
//    viewWindow_.set_time_scale(DEFAULT_TIMELINE_SCALE);
      
      setSelection (Mutation::changeTime (Time(FSecs(2))));
      setSelection (Mutation::changeDuration (Duration(FSecs(2))));
      //////////////////////////////////////////////////////TICKET #797 : this looks cheesy. Should provide a single Mutation to change all
    }
  
  
  shared_ptr<model::Sequence>
  TimelineState::getSequence() const
  {
    return sequence_;
  }
  
  
  TimelineViewWindow&
  TimelineState::getViewWindow()
  {
    return viewWindow_;
  }
  
  
  void
  TimelineState::setSelection (Mutation const& change,
                               bool resetPlaybackPeriod)
  {
    selection_.accept (change);
    if (resetPlaybackPeriod)
      setPlaybackPeriod(change);
    
    selectionChangedSignal_.emit();
  }
  
  
  void
  TimelineState::setPlaybackPeriod (Mutation const& change)
  {
    playbackPeriod_.accept (change);
    playbackChangedSignal_.emit();
  }
  
  
  void
  TimelineState::setPlaybackPoint (Time newPosition)
  {
    playbackPoint_ = newPosition;
    playbackChangedSignal_.emit();
  }
  
  
  void
  TimelineState::setSelectionControl (SelectionControl &control)
  {
    control.disconnect();
    selection_.accept (control);
    control.connectChangeNotification (selectionListener_);
  }
  
  
  sigc::signal<void>
  TimelineState::selectionChangedSignal()  const
  {
    return selectionChangedSignal_;
  }
  
  
  sigc::signal<void>
  TimelineState::playbackChangedSignal()  const
  {
    return playbackChangedSignal_;
  }
  
  
  void
  TimelineState::on_selection_changed (TimeSpan)
  {
    selectionChangedSignal_.emit();
  }
  
  
}}}// namespace stage::widget::timeline
