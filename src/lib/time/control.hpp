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
 ** \par implementation notes
 ** - the validity of a given combination of change and target is checked immediately,
 **   when connecting to the target. Depending on the situation, the actual changes later
 **   are subject to specific treatment (e.g. frame quantisation)
 ** - by default time::Control is <b>not threadsafe</b>. But, as each change is basically
 **   processed within its own call context (function invocation), parallelism is only
 **   a concern with respect to the value finally visible within the target.
 ** - the change notification is processed right away, after applying the change to the
 **   target; in all cases, the effective change value is what will be propagated, \em not
 **   the content of the target after applying the change
 ** 
 ** @todo WIP-WIP-WIP
 **
 */

#ifndef LIB_TIME_CONTROL_H
#define LIB_TIME_CONTROL_H

#include "lib/error.hpp"
#include "lib/meta/util.hpp"
#include "lib/time/mutation.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/utility/enable_if.hpp>
#include <tr1/functional>
#include <vector>


namespace lib {
namespace time {
  
  namespace mutation {
    
    using boost::enable_if;
    using boost::disable_if;
    using lumiera::typelist::is_sameType;
    using std::tr1::placeholders::_1;
    using std::tr1::function;
    using std::tr1::bind;
    using std::tr1::ref;
    
    
    
    /**
     * Implementation building block: impose changes to a Time element.
     * The purpose of the Mutator is to attach a target time entity,
     * which then will be subject to any received value changes,
     * offsets and grid nudging. The actual attachment is to be
     * performed in a subclass, by using the Mutation interface.
     * When attaching to a target, the Mutator will be outfitted 
     * with a set of suitable functors, incorporating the specific
     * behaviour for the concrete combination of input changes
     * ("source values") and target object type. This works by
     * binding to the appropriate implementation functionality,
     * guided by a templated policy class. After installing
     * these functors, these decisions remains opaque and
     * encapsulated within the functor objects, so the
     * mutator object doesn't need to carry this 
     * type information on the interface
     */
    template<class TI>
    class Mutator
      : public Mutation
      {
        typedef function<TI(TI const&)>     ValueSetter;
        typedef function<TI(Offset const&)> Ofsetter;
        typedef function<TI(int)>           Nudger;
        
      protected:
        mutable ValueSetter setVal_;
        mutable Ofsetter    offset_;
        mutable Nudger      nudge_;
        
        void
        ensure_isArmed()
          {
            if (!setVal_)
              throw error::State("feeding time/value change "
                                 "while not (yet) connected to any target to change"
                                ,error::LUMIERA_ERROR_UNCONNECTED);
          }
        
        
        template<class TAR>
        void bind_to (TAR& target)  const;
        
        void unbind();
        
        // using default construction and copy
      };
    
    
    
    /**
     * Implementation building block: propagate changes to listeners.
     * The Propagator manages a set of callback signals, allowing to
     * propagate notifications for changed Time values.
     * 
     * There are no specific requirements on the acceptable listeners,
     * besides exposing a function-call operator to feed the changed
     * time value to. Both Mutator and Propagator employ one primary
     * template parameter, which is the type of the time values
     * to be fed in and propagated. 
     */
    template<class TI>
    class Propagator
      {
        typedef function<void(TI const&)> ChangeSignal;
        typedef std::vector<ChangeSignal> ListenerList;
        
        ListenerList listeners_;
        
      public:
        /** install notification receiver */
        template<class SIG>
        void
        attach (SIG const& toNotify)
          {
            ChangeSignal newListener (ref(toNotify));
            listeners_.push_back (newListener);
          }
        
        /** disconnect any observers */
        void
        disconnnect()
          {
            listeners_.clear();
          }
        
        /** publish a change */
        TI
        operator() (TI const& changedVal)  const
          {
            typedef typename ListenerList::const_iterator Iter;
            Iter p = listeners_.begin();
            Iter e = listeners_.end();
            
            for ( ; p!=e; ++p )
              (*p) (changedVal);
            return changedVal;
          }
      };
    
    
    
    namespace { // metaprogramming helpers to pick suitable implementation branch...
      
      template<class T>
      inline bool
      isDuration()
      {
        return is_sameType<T,Duration>::value;
      }
      
      template<class T>
      inline bool
      isTimeSpan()
      {
        return is_sameType<T,TimeSpan>::value;
      }
      
      template<class T>
      inline T const&
      maybeMaterialise (T const& non_grid_aligned_TimeValue)
      {
        return non_grid_aligned_TimeValue;
      }
      
#ifdef LIB_TIME_TIMEQUQNT_H
      inline QuTime
      maybeMaterialise (QuTime const& alignedTime)
      {
        PQuant const& grid(alignedTime);
        return QuTime(grid->materialise(alignedTime), grid);
      }
#endif
    }
    
    
    template<class TI, class TAR>
    struct Builder
      {
        static TI
        buildChangedValue (TAR const& target)
          {
            return TI(target);
          }
      };
    template<class TAR>
    struct Builder<TimeSpan, TAR>
      {
        static TimeSpan
        buildChangedValue (TAR const& target)
          {
            return TimeSpan (target, Duration::NIL);
          }
      };
    template<>
    struct Builder<TimeSpan, Duration>
      {
        static TimeSpan
        buildChangedValue (Duration const& targetDuration)
          {
            return TimeSpan (Time::ZERO, targetDuration);
          }
      };
    template<>
    struct Builder<TimeSpan, TimeSpan>
      {
        static TimeSpan
        buildChangedValue (TimeSpan const& target)
          {
            return target;
          }
      };
#ifdef LIB_TIME_TIMEQUQNT_H
    template<class TAR>
    struct Builder<QuTime, TAR>
      {
        static QuTime
        buildChangedValue (TAR const& target)
          {
            return QuTime (target
                          ,getDefaultGridFallback()                                                     //////////////////TICKET #810
                          );
          }
      };
    template<>
    struct Builder<QuTime, QuTime>
      {
        static QuTime
        buildChangedValue (QuTime const& target)
          {
            return target;
          }
      };
#endif
    
    template<class TI, class TAR>
    struct Link
      : Mutator<TI>
      , Builder<TI,TAR>
      {
        
        template<class SRC>
        static TI
        processValueChange (TAR& target, SRC const& change)
          {
            imposeChange (target, maybeMaterialise(change));
            return buildChangedValue (maybeMaterialise(target));
          }
        
        static TI
        useLengthAsChange (TAR& target, TimeSpan const& change)
          {
            return processValueChange(target, change.duration());
          }
        
        static TI
        mutateLength (TimeSpan& target, Duration const& change)
          {
            Mutator<TimeSpan>::imposeChange (target.duration(), change);
            return Builder<TI,TimeSpan>::buildChangedValue(target);
          }
        
        static TimeSpan
        mutateTimeSpan (TimeSpan& target, TimeSpan const& change)
          {
            Mutator<TimeSpan>::imposeChange (target.duration(), change.duration());
            Mutator<TimeSpan>::imposeChange (target,change.start());
            return Builder<TimeSpan,TimeSpan>::buildChangedValue(target);
          }
        
        static TI
        dontChange (TAR& target)
          {
            // note: not touching the target
            return buildChangedValue(target);
          }
      };
    
    
    
    template<class TI, class SRC, class TAR>
    struct Policy
      {
        static function<TI(SRC const&)>
        buildChangeHandler (TAR& target)
          {
            return bind (Link<TI,TAR>::template processValueChange<SRC>, ref(target), _1 );
          }
      };
    
    
    // special treatment of Durations as target...
    
    namespace {
      template<class T>
      struct canMutateDuration
        {
          static const bool value = is_sameType<T,Duration>::value
                               ||   is_sameType<T,Offset>::value
                               ||   is_sameType<T,int>::value;
        };
      
      template<class T>
      struct canReceiveDuration
        {
          static const bool value = is_sameType<T,Duration>::value
                               ||   is_sameType<T,TimeSpan>::value;
        };
    }
    
    
    template<class TI, class SRC>
    struct Policy<TI,SRC,                  typename disable_if< canMutateDuration<SRC>, 
                         Duration>::type>
      {
        static function<TI(SRC const&)>
        buildChangeHandler (Duration& target)
          {
            return bind (Link<TI,Duration>::dontChange, ref(target) );
          }
      };
    
    template<class TAR>
    struct Policy<Duration,                typename disable_if< canReceiveDuration<TAR>, 
                     Duration>::type, TAR>
      {
        static function<Duration(Duration const&)>
        buildChangeHandler (TAR&)
          {
            return bind ( ignore_change_and_return_Zero );
          }
        
        static Duration
        ignore_change_and_return_Zero()
          {
            return Duration::NIL;
          }
      };
    
    template<class TI>
    struct Policy<TI,TimeSpan,Duration>
      {
        static function<TI(TimeSpan const&)>
        buildChangeHandler (Duration& target)
          {
            return bind (Link<TI,Duration>::useLengthAsChange, ref(target), _1 );
          }
      };
    
    // special treatment for TimeSpan values...
    
    template<class TI>
    struct Policy<TI,Duration,TimeSpan>
      {
        static function<TI(Duration const&)>
        buildChangeHandler (TimeSpan& target)
          {
            return bind (Link<TI,TimeSpan>::mutateLength, ref(target), _1 );
          }
      };
    
    template<>
    struct Policy<TimeSpan,TimeSpan,TimeSpan>
      {
        static function<TimeSpan(TimeSpan const&)>
        buildChangeHandler (TimeSpan& target)
          {
            return bind (Link<TimeSpan,TimeSpan>::mutateTimeSpan, ref(target), _1 );
          }
      };
    
    
    
    
    
    
    template<class TI>
    template<class TAR>
    void
    Mutator<TI>::bind_to (TAR& target)  const
    {
      setVal_ = Policy<TI,TI,    TAR>::buildChangeHandler (target);
      offset_ = Policy<TI,Offset,TAR>::buildChangeHandler (target);
      nudge_  = Policy<TI,int,   TAR>::buildChangeHandler (target);
    }
    
    template<class TI>
    void
    Mutator<TI>::unbind()
    {
      setVal_ = ValueSetter();
      offset_ = Ofsetter();
      nudge_  = Nudger();
    }
    
  }
  
  
  /**
   * Frontend/Interface: controller-element to retrieve
   * and change running time values
   * 
   * @see time::Mutation
   * @see time::TimeSpan#accept(Mutation const&)
   * @todo WIP-WIP-WIP
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
      void operator() (TI const&);
      void operator() (Offset const&);
      void operator() (int);
      
      
      /** install a callback functor to be invoked as notification
       *  for any changes imposed onto the observed time entity.
       *  @param toNotify object with \c operator()(TI const&) */
      template<class SIG>
      void connectChangeNotification (SIG const& toNotify);
      
      /** disconnect from observed entity and
       *  cease any change notification */
      void disconnnect();
    };
  
  
  
  
  /* === forward to implementation === */
  
  template<class TI>
  void
  Control<TI>::operator () (TI const& newValue)
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->setVal_(newValue));
  }
  
  template<class TI>
  void
  Control<TI>::operator () (Offset const& adjustment)
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->offset_(adjustment));
  }
  
  template<class TI>
  void
  Control<TI>::operator () (int offset_by_steps)
  {
    this->ensure_isArmed();
    notifyListeners_(
        this->nudge_(offset_by_steps));
  }
  
  
  template<class TI>
  void
  Control<TI>::disconnnect()
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
