/*
  TIMELINE-STATE.hpp  -  timeline presentation state holder

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


/** @file timeline-state.hpp
 ** Presentation state for the first draft of the timeline display. Very likely
 ** to be replaced by a complete rewrite.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 ** @deprecated this is defunct code, left over from the old GTK-2 GUI of Lumiera.
 **    Deactivated since 2016 and no longer included since 3/23 but left in tree
 **    for later reference
 */



#ifndef STAGE_WIDGET_TIMELINE_STATE_H
#define STAGE_WIDGET_TIMELINE_STATE_H

#include "stage/gtk-base.hpp"
//#include "stage/widget/timeline/timeline-view-window.hpp"////////////////////////////TODO : obsolete and removed 3/23

#include "lib/time/timevalue.hpp"
#include "lib/time/mutation.hpp"
#include "lib/time/control.hpp"

#include <memory>


namespace stage {
  namespace model {
    class Sequence;
  }  
  namespace widget { 
  namespace timeline {
  
  using lib::time::Time;
  using lib::time::Offset;
  using lib::time::TimeVar;
  using lib::time::TimeSpan;
  using lib::time::TimeValue;
  using lib::time::Mutation;
  using lib::time::Control;
  
  using std::shared_ptr;
  
  typedef Control<TimeSpan> SelectionControl;

///////////////////////////////////////////////////////////////////////////////////////TODO : dummy placeholder to keep the code in-tree (for later reference)
  class TimelineViewWindow
    {
    public:
      TimelineViewWindow (Offset offset, int64_t scale)
        {
          NOTREACHED ("3/23: this code is dead and left in-tree for later reference only");
        }
    };
///////////////////////////////////////////////////////////////////////////////////////TODO : dummy placeholder to keep the code in-tree (for later reference)
  
  
  /**
   * SelectionListener is a template class which emits a signal when
   * the value is changed by it's associated time::Control object.
   * SelectionListener wraps a sigc::signal that emits every time
   * the selection is changed by the time::Control object.
   * SelectionListener does NOT emit the signal if a change to the
   * selection is made outside of the Control/Listener partnership.
   */
  template<class TI>
  class SelectionListener
    : util::NonCopyable
    {
      sigc::signal<void, TI> valueChangedSignal_;
      
    public:
      SelectionListener() { }
      
      void
      operator() (TI const& changeValue)  const
        {
          valueChangedSignal_.emit (changeValue);
        }
      
      
      void connect (const sigc::slot<void, TI> &connection)
        {
          valueChangedSignal_.connect (connection);
        }
    };
  
  
  
  /**
   * TimelineState is a container for the state data for TimelineWidget.
   * @remarks TimelineStates can be swapped out so that TimelineWidget
   *          can flip between different views.
   */
  class TimelineState
    {
    public:
      
      /**
       * @param source_sequence The sequence on which the TimelineWidget
       *        will operate when this TimelineState is attached.
       */
      TimelineState (shared_ptr<model::Sequence> sourceSequence);
      
      
    public:
      /**
       * Gets the sequence that is attached to this timeline state object.
       * @return Returns a shared_ptr to the sequence object.
       */
      shared_ptr<model::Sequence> getSequence()  const;
      
      /**
       * Gets a reference to the timeline view window object.
       * @return Returns the timeline view window object.
       */
      timeline::TimelineViewWindow& getViewWindow();
      
      
      SelectionListener<TimeSpan>&
      getSelectionListener()              { return selectionListener_; }
      
      Time getSelectionStart()      const { return selection_.start();}
      Time getSelectionEnd()        const { return selection_.end();  }
      Time getPlaybackPeriodStart() const { return selection_.start();}
      Time getPlaybackPeriodEnd()   const { return selection_.end();  }
      
      Time getPlaybackPoint()       const { return playbackPoint_; }
      
      
      /** is there currently any ongoing playback process?
       *  Otherwise the #getPlaybackPoint() is meaningless */
      bool isPlaying() const { return isPlayback_; }
      
      void setSelectionControl (SelectionControl &control);
      
      /**
       * Sets the period of the selection.
       * @param resetPlaybackPeriod Specifies whether to set the
       *        playback period to the same as this new selection.
       */
      void setSelection(Mutation const& change,
                        bool resetPlaybackPeriod = true);
      
      void setPlaybackPeriod(Mutation const& change);
      
      /**
       * Sets the time which is currently being played back.
       * @param point The time index being played.
       * @todo do we ever get the situation that we don't have such a position?
       * @todo very likely to be handled differently, once
       *       the GUI is really connected to the Player
       */
      void setPlaybackPoint(Time newPos);
      
      /**
       * A signal to notify when the selected period has changed.
       */
      sigc::signal<void> selectionChangedSignal()  const;
      
      /**
       * A signal to notify when the playback point or playback periods have
       * changed.
       */
      sigc::signal<void> playbackChangedSignal()  const;
      
      
      
    private: /* ========= Event Handlers ========== */
      /**
       * Event handler for when the selection is changed
       */
      void on_selection_changed (TimeSpan selection);
      
      
    private:
      /**
       * A pointer to the sequence object which this timeline_widget will represent.
       * @remarks This pointer is set by the constructor and is constant, so
       * will not change in value during the lifetime of the class.
       */
      shared_ptr<model::Sequence> sequence_;
      
      
      
      /* == View State == */
      
      /** ViewWindow for the TimelineWidget display */
      timeline::TimelineViewWindow viewWindow_;
      
      
      /* == Selection State == */
      
      /** currently selected time period. */
      TimeSpan selection_;
      
      /** listens for a selection change */
      SelectionListener<TimeSpan> selectionListener_;
      
      /** current playback period. */
      TimeSpan playbackPeriod_;
      
      /** current playback position.
       * @todo very likely to be handled differently
       *       when actually integrated with the Player
       */
      TimeVar playbackPoint_;
      
      bool isPlayback_;
      
      
      /* == Signals == */
      
      /**
       * A signal to notify when the selected period has changed.
       */
      sigc::signal<void> selectionChangedSignal_;
      
      /**
       * A signal to notify when the playback point or playback periods have
       * changed.
       */
      sigc::signal<void> playbackChangedSignal_;
    };
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_STATE_H*/
