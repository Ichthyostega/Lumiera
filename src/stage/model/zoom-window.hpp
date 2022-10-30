/*
  ZOOM-WINDOW.hpp  -  generic translation from domain to screen coordinates

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file zoom-window.hpp
 ** Abstraction: the current zoom- and navigation state of a view, possibly in multiple
 ** dimensions. This is a generic component to represent and handle the zooming and
 ** positioning of views within an underlying model space. This model space is conceived
 ** to be two fold:
 ** - it is a place or excerpt within the model topology (e.g. the n-th track in the fork)
 ** - it has a temporal extension within a larger temporal frame (e.g. some seconds within
 **   the timeline)
 ** This component is called »Zoom Window«, since it represents a window-like local visible
 ** interval, embedded into a larger time span covering a complete timeline.
 ** @note as of 10/2022 this component is in an early stage of development and just used
 **       to coordinate the horizontal extension of the timeline view.
 ** 
 ** # Rationale
 ** 
 ** Working with and arranging media requires a lot of navigation and changes of zoom detail level.
 ** More specifically, the editor is required to repeatedly return to the same locations and show
 ** arrangements at the same alternating scale levels. Most existing editing applications approach
 ** this topic naively, by just responding to some coarse grained interaction controls — thereby
 ** creating the need for a lot of superfluous and tedious search and navigation activities,
 ** causing constant grind for the user. And resolving these obnoxious shortcomings turns out
 ** as a never ending task, precisely due to the naive and ad-hoc approach initially taken.
 ** Based on these observations, the design of the Lumiera UI calls for centralisation of
 ** all zoom- and navigation handling into a single component, instantiated once for every
 ** visible context, outfitted with the ability to capture and maintain a history of zoom
 ** and navigation activities. The current zoom state is thus defined by
 ** - the overall TimeSpan of the timeline, defining a start and end time
 ** - the visible interval („window“), likewise modelled as time::TimeSpan
 ** - the scale defined as pixels per second
 ** 
 ** # Interactions
 ** 
 ** The basic parameters can be changed and adjusted through various setters, dedicated
 ** to specific usage scenarios. After invoking any setter, one of the mutating functions
 ** is invoked to adjust the base parameters and then re-establish the *invariant*
 ** - visible window lies completely within the overall range
 ** - scale factor and visible window line up logically
 ** - scale factor produces precise reproducible values
 ** 
 ** ## Change listener
 ** 
 ** A single change listener lambda can be installed (as of 10/2022 this is considered sufficient,
 ** since only the TimelineLayout was identified as collaborator requiring push notification).
 ** This callback will be invoked after any effective change and serves as notification; the
 ** receiver is expected to read the current settings by invoking the getters.
 ** 
 ** @todo WIP-WIP as of 10/2022 the usage is just emerging, while the actual meaning
 **               of this interface still remains somewhat nebulous...
 ** @see \ref ZoomWindow_test
 ** 
 */


#ifndef STAGE_MODEL_ZOOM_WINDOW_H
#define STAGE_MODEL_ZOOM_WINDOW_H


#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"

//#include <utility>
//#include <string>


namespace stage {
namespace model {
  
//  using std::string;
//  using lib::Symbol;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using lib::time::TimeVar;
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::time::Time;
  
  namespace {
    /** the deepest zoom is to use 2px per micro-tick */
    const uint ZOOM_MAX_RESOLUTION = 2 * TimeValue::SCALE;
  }
  
  /**
   * A component to ensure uniform handling of zoom scale
   * and visible interval on the timeline. Changes through
   * the mutator functions are validated and harmonised to
   * meet the internal invariants; a change listener is
   * possibly notified to pick up the new settings.
   * 
   * A ZoomWindow...
   * - is a #visible TimeSpan
   * - which is completely inside an #overalSpan
   * - and is rendered at a scale factor #px_per_sec
   * - 0 < px_per_sec <= ZOOM_MAX_RESOLUTION
   * - zoom operations are applied around an #anchorPoint
   */
  class ZoomWindow
    : util::NonCopyable
    {
      TimeVar startAll_, afterAll_,
              startWin_, afterWin_;
      uint px_per_sec_;
      
    public:
      ZoomWindow (TimeSpan timeline =TimeSpan{Time::ZERO, FSecs(23)})
        : startAll_{timeline.start()}
        , afterAll_{nonEmpty(timeline.end())}
        , startWin_{startAll_}
        , afterWin_{afterAll_}
        , px_per_sec_{25}
        { }
      
      TimeSpan
      overallSpan()  const
        {
          return TimeSpan{startAll_, afterAll_};
        }
      
      TimeSpan
      visible()  const
        {
          return TimeSpan{startWin_, afterWin_};
        }
      
      uint
      px_per_sec()  const
        {
          return px_per_sec_;
        }
      
      
      /* === Mutators === */
      
      void
      setMetric (uint px_per_sec)
        {
          UNIMPLEMENTED ("setMetric");
        }
      
      /**
       * scale up or down on a 2-logarithmic scale.
       * Each step either doubles or halves the zoom level,
       * and the visible window is adjusted accordingly, using
       * the current #anchorPoint as centre for scaling.
       * @note the zoom factor is limited to be between
       *       2px per µ-tick and 1px per second (~20min
       *       on a typical 1280 monitor)
       * @todo support for overview mode ////////////////////////////////////////////////////////////////////TICKET #1255 : implement overview mode
       */
      void
      nudgeMetric (int steps)
        {
          uint changedScale =
              steps > 0 ? px_per_sec_ << steps
                        : px_per_sec_ >> -steps;
          if (0 < changedScale
              and changedScale <= ZOOM_MAX_RESOLUTION)
            mutateScale (changedScale);
        }
      
      void
      setRanges (TimeSpan overall, TimeSpan visible)
        {
          UNIMPLEMENTED ("setOverallRange");
        }
      
      void
      setOverallRange (TimeSpan range)
        {
          UNIMPLEMENTED ("setOverallRange");
        }
      
      void
      setOverallStart (TimeValue start)
        {
          UNIMPLEMENTED ("setOverallStart");
        }
      
      void
      setOverallDuration (Duration duration)
        {
          UNIMPLEMENTED ("setOverallDuration");
        }
      
      void
      setVisibleRange (TimeSpan newWindow)
        {
          mutateWindow (newWindow.start(), newWindow.end());
        }
      
      void
      expandVisibleRange (TimeSpan target)
        {
          UNIMPLEMENTED ("zoom out to bring the current window at the designated time span");
        }
      
      void
      setVisibleDuration (Duration duration)
        {
          UNIMPLEMENTED ("setVisibleDuration");
        }
      
      void
      setVisiblePos (TimeValue posToShow)
        {
          UNIMPLEMENTED ("setVisiblePos");
        }
      
      void
      setVisiblePos (float percentage)
        {
          UNIMPLEMENTED ("setVisiblePos");
        }
      
      void
      offsetVisiblePos (Offset offset)
        {
          UNIMPLEMENTED ("offsetVisiblePos");
        }
      
      void
      nudgeVisiblePos (int steps)
        {
          FSecs dur{afterWin_-startWin_};      //    navigate half window steps
          setVisibleRange (TimeSpan{Time{startWin_ + dur*steps/2}
                                   , dur});
        }
      
      void
      navHistory()
        {
          UNIMPLEMENTED ("navigate Zoom History");
        }
      
    private:
      /* === adjust and coordinate === */
      
      TimeValue
      nonEmpty (TimeValue endPoint)
        {
          if (startAll_ < endPoint)
            return endPoint;
          if (startAll_ < Time::MAX)
            return TimeValue{startAll_ + 1}; 
          startAll_ = Time::MAX - TimeValue(1);
          return Time::MAX;
        }
      
      
      /** @internal change Window TimeSpan, validate and adjust all params */
      void
      mutateWindow (TimeVar start, TimeVar after)
        {
          if (not (start < after))
            {
              if (after == Time::MAX)
                start = Time::MAX - TimeValue(1);
              else
                after = start + TimeValue(1);
            }
          
          FSecs dur{after - start};
          if (dur > FSecs{afterAll_ - startAll_})
            {
              start = startAll_;
              after = afterAll_;
            }
          else
          if (start < startAll_)
            {
              start = startAll_;
              after = start + dur;
            }
          else
          if (after > afterAll_)
            {
              after = afterAll_;
              start = after - dur;
            }
          ASSERT (after-start <= afterAll_-startAll_);
          
          px_per_sec_  = adjustedScale (start,after, startWin_,afterWin_);
          startWin_ = start;
          afterWin_ = after;
          fireChangeNotification();
        }
      
      /** 
       * @internal adjust Window to match given scale,
       *           validate and adjust all params */
      void
      mutateScale (uint px_per_sec)
        {
          if (px_per_sec == 0) px_per_sec = 1;
          if (px_per_sec == px_per_sec_) return;
          
          FSecs changeFactor{px_per_sec, px_per_sec_};
          FSecs dur{afterWin_ - startWin_};
          dur /= changeFactor;
          if (dur > FSecs{afterAll_ - startAll_})
            {// limit to the overall timespan...
              px_per_sec_  = adjustedScale (startAll_,afterAll_, startWin_,afterWin_);
              startWin_ = startAll_;
              afterWin_ = afterAll_;
            }
          else
            {
              TimeVar start{anchorPoint() - dur*relativeAnchor()};
              if (start < startAll_)
                start = startAll_;
              TimeVar after{start + dur};
              if (after > afterAll_)
                {
                  after = afterAll_;
                  start = afterAll_ - dur;
                }
              ASSERT (after-start <= afterAll_-startAll_);
              
              if (start == startWin_ and after == afterWin_)
                return; // nothing changed effectively
              
              px_per_sec_ = adjustedScale (start,after, startWin_,afterWin_);
              startWin_ = start;
              afterWin_ = after;
            }
          fireChangeNotification();
        }
      
      void
      mutateDuration (Duration duration)
        {
          UNIMPLEMENTED ("change visible duration, validate and adjust all params");
        }
      
      
      /**
       * Adjust the display scale such as to match the given changed time interval
       * @param startNew changed start point
       * @param afterNew changed end point
       * @param startOld previous start point
       * @param afterOld previous end point
       * @return adapted scale factor in pixel per second, rounded half up to the next pixel.
       */
      uint
      adjustedScale (TimeVar startNew, TimeVar afterNew, TimeVar startOld, TimeVar afterOld)
        {
          REQUIRE (startOld < afterOld);
          FSecs factor = FSecs{afterNew - startNew} / FSecs{afterOld - startOld};
          return boost::rational_cast<uint>(px_per_sec_ / factor + 1/2); // rounding half pixels
        }
      
      /**
       * The anchor point or centre for zooming operations applied to the visible window
       * @return where the visible window should currently be anchored
       * @remark this point can sometimes be outside the current visible window,
       *         but any further zooming/scaling/scrolling operation should bring it back
       *         into sight. Moreover, the function #relativeAnchor() defines the position
       *         where this anchor point _should_ be placed relative to the visible window.
       * @todo 10/2022 we use a numerical rule currently, but that could be contextual state,
       *       like e.g. the current position of the play head or edit cursor or mouse.
       */
      FSecs
      anchorPoint()  const
        {
          return startWin_ + FSecs{afterWin_-startWin_} * relativeAnchor(); 
        }
      
      /**
       * define at which proportion to the visible window's duration the anchor should be placed
       * @return a fraction 0 ... 1, where 0 means at start and 1 means after end.
       * @note as of 10/2022 we use a numerical rule to place the anchor point in accordance
       *       to the current visible window's position within the overall timeline; if it's
       *       close to the beginning, the anchor point is also rather to the beginning...
       */
      FSecs
      relativeAnchor()  const
        {
          // the visible window itself has to fit in, which reduces the action range
          FSecs possibleRange = (afterAll_-startAll_) - (afterWin_-startWin_);
          if (possibleRange == 0) // if there is no room for scrolling...
            return FSecs{1,2};   //  then anchor zooming in the middle
          
          // use a 3rd degree parabola to favour positions in the middle
          FSecs posFactor = FSecs{startWin_-startAll_} / possibleRange;
          posFactor = (2*posFactor - 1);              // -1 ... +1
          posFactor = posFactor*posFactor*posFactor;  // -1 ... +1 but accelerating towards boundraries
          posFactor = (posFactor + 1) / 2;            //  0 ... 1
          return posFactor;
        }
      
      void
      fireChangeNotification()
        {
          TODO("really fire...");
        }
    };
  
  
  
  
  /** */

  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ZOOM_WINDOW_H*/
