/*
  CONTROL.hpp  -  a life time control for feedback and mutation  

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file control.hpp
 ** Manipulating and monitoring time entities with life changes.
 ** This is an control- and callback element to handle any kind of "running"
 ** time entities. This element is to be provided by the client and then attached
 ** to the target time entity as a time::Mutation. Internally, a life connection to
 ** the target is built, allowing both to 
 ** - to manipulate the target by invoking the function operator
 ** - to receive change notifications by installing a callback functor.
 ** 
 ** The actual type of the changes and modifications is specified as template parameter;
 ** when later attached to some time entity as a Mutation, the actual changes to be performed
 ** depend both on this change type and the type of the target time entity (double dispatch).
 ** The behaviour is similar to applying a static time::Mutation
 ** 
 ** \par relevance
 ** This control element is intended to be used for all kinds of editing and monitoring
 ** of time-like entities -- be it the running time display in a GUI widget, a ruler marker
 ** which can be dragged, a modifiable selection or the animated playhead cursor.
 ** 
 ** 
 ** # Usage scenarios
 ** 
 ** The time::Control element provides mediating functionality, but doesn't assume or provide
 ** anything special regarding the usage pattern or the lifecycle, beyond the ability to
 ** attach listeners, attach to a (different) target and to detach from all connections.
 ** Especially, no assumptions are made about which side is the server or the client
 ** and who owns the time::Control element.
 ** 
 ** Thus an interface might accept a time::Control element \em reference (e.g. the
 ** lumiera::Play::Controller uses this pattern) -- meaning that the client owns the
 ** Control element and might attach listeners, while the implementation (server side)
 ** will attach the Control to mutate a time value entity otherwise not disclosed
 ** (e.g. the playhead position of the playback process). Of course, in this case
 ** the client is responsible for keeping the Control element and all listeners
 ** alive, and to invoke Control#disconnect prior to destroying the element.
 ** 
 ** Of course, the reversed usage situation would be possible as well: an interface
 ** may expose a time::Control, thus allowing to attach target and listeners, while the
 ** actual changes will originate somewhere within the opaque service implementation.
 ** 
 ** Another usage pattern would be to expose a `time::Control const&`, allowing only to
 ** impose changes, but not to change the target or listener attachments. To the contrary,
 ** exposing only a time::Mutation \c const& through some interface allows only to attach
 ** new target elements, but not to change listeners or feed any value changes.
 ** 
 ** Using time::Control as an implementation building block and just exposing the
 ** change (function) operators or the listener attachment through an forwarding sub
 ** interface is another option.
 ** 
 ** @note time::Control is default constructible and freely copyable.
 ** 
 ** 
 ** ## Changing quantised (grid aligned) time entities
 ** 
 ** The time::Control element includes the functionality to handle grid aligned time values,
 ** both as target and as change/notification value. This ability is compiled in conditionally,
 ** as including mutation.hpp causes several additional includes, which isn't desirable when
 ** it comes just to changing plain time values. Thus, to get these additional specialisations,
 ** the LIB_TIME_TIMEQUANT_H header guard needs to be defined, which happens automatically
 ** if lib/time/mutation.hpp is included prior to lib/time/control.hpp.
 ** 
 ** 
 ** # implementation notes
 ** - the validity of a given combination of change and target is checked immediately,
 **   when connecting to the target. Depending on the situation, the actual changes later
 **   are subject to specific treatment (e.g. frame quantisation)
 ** - by default time::Control is <b>not threadsafe</b>. But, as each change is basically
 **   processed within its own call context (function invocation), parallelism is only
 **   a concern with respect to the value finally visible within the target.
 ** - the change notification is processed right away, after applying the change to the
 **   target; of course there is a race between applying the value and building the
 **   response value passed on as notification. In all cases, the effective change
 **   notification value is built from the state of the target after applying
 **   the change, which might or might not reflect the change value passed in.
 ** 
 ** @todo include support for QuTimeSpan values                               ////////////////////TICKET #760
 ** 
 ** @see TimeControl_test
 **
 */

#ifndef LIB_TIME_CONTROL_H
#define LIB_TIME_CONTROL_H

#include "lib/time/mutation.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/control-impl.hpp"



namespace lib {
namespace time {
  
  
  
  /**
   * Frontend/Interface: controller-element to retrieve
   * and change running time values. time::Control is
   * a mediator element, which can be attached to some
   * time value entities as \em mutation, and at the
   * same time allows to register listeners. When
   * configured this way, \em changes may be fed
   * to the function operator(s). These changes
   * will be imposed to the connected target
   * and the result propagated to the listeners.
   * 
   * @see time::Mutation
   * @see time::TimeSpan#accept(Mutation const&)
   */
  template<class TI>
  class Control
    : public mutation::Mutator<TI>
    {
      mutation::Propagator<TI> notifyListeners_;
      
      virtual void change (Duration&)  const;
      virtual void change (TimeSpan&)  const;
      virtual void change (QuTime&)    const;
        
    public:
      void operator() (TI const&)      const;
      void operator() (Offset const&)  const;
      void operator() (int)            const;
      
      
      /** install a callback functor to be invoked as notification
       *  for any changes imposed onto the observed time entity.
       *  @param toNotify object with \c operator()(TI const&) */
      template<class SIG>
      void connectChangeNotification (SIG const& toNotify);
      
      /** disconnect from observed entity and
       *  cease any change notification */
      void disconnect();
    };
  
  
  
  
  /* === forward to implementation === */
  
  /** impose a new value to the connected target.
   *  If applicable, the target will afterwards reflect
   *  that change, and listeners will be notified, passing
   *  the target's new state.
   * @throw error::State if not connected to a target
   * @note the actual change in the target also depends
   *       on the concrete target type and the type of
   *       the change. By default, the time value is
   *       changed; this may include grid alignment.
   */
  template<class TI>
  void
  Control<TI>::operator () (TI const& newValue)  const
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->setVal_(newValue));
  }
  
  /** impose an offset to the connected target.
   *  If applicable, the target will be adjusted by the
   *  time offset, and listeners will be notified.
   * @throw error::State if not connected to a target
   */
  template<class TI>
  void
  Control<TI>::operator () (Offset const& adjustment)  const
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->offset_(adjustment));
  }
  
  /** nudge the connected target by the given offset steps,
   *  using either the target's own grid (when quantised),
   *  or a 'natural' nudge grid
   * @throw error::State if not connected to a target
   */
  template<class TI>
  void
  Control<TI>::operator () (int offset_by_steps)  const
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->nudge_(offset_by_steps));
  }
  
  
  template<class TI>
  void
  Control<TI>::disconnect()
  {
    notifyListeners_.disconnect();
    this->unbind();
  }
  
  template<class TI>
  template<class SIG>
  void
  Control<TI>::connectChangeNotification (SIG const& toNotify)
  {
    if (this->offset_)
      { // we're already connected: thus propagate current value
        TI currentValue = this->offset_(Offset::ZERO);
        toNotify (currentValue);
      }
    notifyListeners_.attach (toNotify);
  }
  
  
  /* ==== Implementation of the Mutation interface ==== */
  
  template<class TI>
  void
  Control<TI>::change (Duration& targetDuration)  const
  {
    this->bind_to (targetDuration);
  }
  
  template<class TI>
  void
  Control<TI>::change (TimeSpan& targetInterval)  const
  {
    this->bind_to (targetInterval);
  }
  
  template<class TI>
  void
  Control<TI>::change (QuTime& targetQuTime)  const
  {
    this->bind_to (targetQuTime);
  }
  
}} // lib::time
#endif
