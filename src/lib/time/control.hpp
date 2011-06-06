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
//#include "lib/symbol.hpp"

//#include <boost/noncopyable.hpp>
//#include <iostream>
//#include <boost/operators.hpp>
#include <boost/utility/enable_if.hpp>
#include <tr1/functional>
#include <vector>
//#include <string>


namespace lib {
namespace time {
  
//using lib::Symbol;
//using std::string;
//using lib::Literal;
  
  
//LUMIERA_ERROR_DECLARE (INVALID_MUTATION); ///< Changing a time value in this way was not designated
  
  namespace mutation {
    
    using boost::enable_if;
    using boost::disable_if;
    using lumiera::typelist::is_sameType;
    using std::tr1::placeholders::_1;
    using std::tr1::function;
    using std::tr1::bind;
    using std::tr1::ref;
    
    
    template<class TI>
    struct Mutabor;
    
    
    /**
     * Implementation building block: impose changes to a Time element.
     * The Mutator supports attaching a target time entity (through
     * the Mutation interface), which then will be subject to any
     * received value changes, offsets and grid nudging.
     * 
     * @todo WIP-WIP-WIP
     */
    template<class TI>
    class Mutator
      : public Mutation
      {
        typedef function<TI(TI const&)>     ValueSetter;
        typedef function<TI(Offset const&)> Ofsetter;
        typedef function<TI(int)>           Nudger;
        
        static TI imposeValueChange(TimeValue& target, TI const&);
        static TI imposeOffset (TimeValue& target, Offset const&);
        static TI imposeNudge (TimeValue& target, int);
        
        static TI changeDuration (Duration& target, TI const&);
//      static TI nudgeDuration (Duration& target, int);
        
        template<class SRC, class TAR>
        struct MutationPolicy;
        
        friend class Mutabor<TI>;

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
        
      public:
        // using default construction and copy
        
        template<class TAR>
        void bind_to (TAR& target)  const;
        
        void unbind();
        
      };
    
    template<class TI>
    struct Mutabor
      {
        
        static void
        imposeChange (TimeValue& target, TI const& newVal)
          {
            Mutator<TI>::imposeChange (target,newVal);
          }
      };
    
    
    template<class TI, class TAR>
    struct Builder
      {
        
        static TI
        buildChangedValue (TAR& target)
          {
            return TI(target);
          }
      };
    template<class TAR>
    struct Builder<TimeSpan, TAR>
      {
        static TimeSpan
        buildChangedValue (TAR& target)
          {
            return TimeSpan (target, Duration::ZERO);  /////////////TODO how to feed the "new value" duration????
          }
      };
    template<>
    struct Builder<TimeSpan, TimeSpan>
      {
        static TimeSpan
        buildChangedValue (TimeSpan& target)
          {
            return target;
          }
      };
#ifdef LIB_TIME_TIMEQUQNT_H
    template<class TAR>
    struct Builder<QuTime, TAR>
      {
        static QuTime
        buildChangedValue (TAR& target)
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
        buildChangedValue (QuTime& target)
          {
            return target;
          }
      };
#endif
    
    template<class TI, class TAR>
    struct Adap
      : Mutator<TI>
      , Builder<TI,TAR>
      {
        
        static TI
        imposeValueChange (TAR& target, TI const& newVal)
          {
            imposeChange (target,newVal);
            return buildChangedValue(target);
          }
        
        static TI
        imposeOffset (TAR& target, Offset const& off)
          {
            imposeChange (target,off);
            return buildChangedValue(target);
          }
        
        static TI
        imposeNudge (TAR& target, int off_by_steps)
          {
            imposeChange (target,off_by_steps);
            return buildChangedValue(target);
          }
      };
    
    template<class TI, class SRC, class TAR>
    struct Policy;
    
    template<class TI, class TAR>
    struct Policy<TI,TI,TAR>
      {
        static function<TI(TI const&)>
        buildChangeHandler (TAR& target)
          {
            return bind (Adap<TI,TAR>::imposeValueChange, ref(target), _1 );
          }
      };
    
    template<class TI, class TAR>
    struct Policy<TI,Offset,TAR>
      {
        static function<TI(Offset const&)>
        buildChangeHandler (TAR& target)
          {
            return bind (Adap<TI,TAR>::imposeOffset, ref(target), _1 );
          }
      };
    
    template<class TI, class TAR>
    struct Policy<TI,int,TAR>
      {
        static function<TI(int)>
        buildChangeHandler (TAR& target)
          {
            return bind (Adap<TI,TAR>::imposeNudge, ref(target), _1 );
          }
      };
    
      
    template<class TI>
    TI
    Mutator<TI>::imposeValueChange (TimeValue& target, TI const& newVal)
    {
      return TI (Mutation::imposeChange (target,newVal));
    }
    template<class TI>
    TI
    Mutator<TI>::imposeOffset (TimeValue& target, Offset const& off)
    {
      return TI (Mutation::imposeChange (target, TimeVar(target)+off));
    }
    template<class TI>
    TI
    Mutator<TI>::imposeNudge (TimeValue& target, int off_by_steps)
    {
      return TI (Mutation::imposeChange (target, TimeVar(target)+Time(FSecs(off_by_steps))));     //////////////////TICKET #810
    }
    
    // special cases...
    template<class TI>
    TI
    Mutator<TI>::changeDuration (Duration&, TI const&)
    {
      return TI (Time::ZERO);
    }
    template<>
    Duration
    Mutator<Duration>::changeDuration (Duration& target, Duration const& changedDur)
    {
      return Duration (Mutation::imposeChange (target,changedDur));
    }
    template<>
    TimeSpan
    Mutator<TimeSpan>::changeDuration (Duration& target, TimeSpan const& timeSpan_to_impose)
    {
      return TimeSpan (timeSpan_to_impose.start()
                      ,Duration(Mutation::imposeChange (target,timeSpan_to_impose.duration()))
                      );
    }
    
    template<>
    TimeSpan
    Mutator<TimeSpan>::imposeValueChange (TimeValue& target, TimeSpan const& newVal)
    {
      Mutation::imposeChange (target,newVal);
      return newVal;
    }
    template<>
    TimeSpan
    Mutator<TimeSpan>::imposeOffset (TimeValue& target, Offset const& off)
    {
      return TimeSpan (Mutation::imposeChange (target, TimeVar(target)+off), Duration::ZERO);
    }
    template<>
    TimeSpan
    Mutator<TimeSpan>::imposeNudge (TimeValue& target, int off_by_steps)
    {
      return TimeSpan (Mutation::imposeChange (target, TimeVar(target)+Time(FSecs(off_by_steps))), Duration::ZERO);
    }
    

#ifdef LIB_TIME_TIMEQUQNT_H
    template<>
    QuTime
    Mutator<QuTime>::changeDuration (Duration&, QuTime const& irrelevantChange)
    {
      return QuTime (Time::ZERO, PQuant(irrelevantChange));
    }
    template<>
    QuTime
    Mutator<QuTime>::imposeValueChange (TimeValue& target, QuTime const& grid_aligned_Value_to_set)
    {
      PQuant quantiser (grid_aligned_Value_to_set);
      TimeValue appliedChange = quantiser->materialise(grid_aligned_Value_to_set);
      Mutation::imposeChange (target, appliedChange);
      return QuTime (target, quantiser);
    }
    template<>
    QuTime
    Mutator<QuTime>::imposeOffset (TimeValue& target, Offset const& off)
    {
      return QuTime (Mutation::imposeChange (target, TimeVar(target)+off)
                    ,getDefaultGridFallback()                                                     //////////////////TICKET #810
                    );
    }
    template<>
    QuTime
    Mutator<QuTime>::imposeNudge (TimeValue& target, int off_by_steps)
    {
      return QuTime (Mutation::imposeChange (target, TimeVar(target)+Time(FSecs(off_by_steps)))
                    ,getDefaultGridFallback()                                                     //////////////////TICKET #810
                    );
    }
#endif //(End)quantisation special case    
    
    
    namespace { // metaprogramming helpers to pick the suitable Instantiation...
      
      template<class T>
      struct isDuration
        {
          static const bool value = is_sameType<T,Duration>::value;
        };
      template<class T>
      struct isTimeSpan
        {
          static const bool value = is_sameType<T,TimeSpan>::value;
        };
      template<class T>
      struct isQuTime
        {
          static const bool value = is_sameType<T,QuTime>::value;
        };
      
      template<class T>
      struct isSpecialCase
        {
          static const bool value = isDuration<T>::value;
        };
      
    }
    
    template<class TI>
    template<class TAR>
    struct Mutator<TI>::MutationPolicy<                  typename disable_if< isSpecialCase<TAR>, 
                                       TI>::type, TAR>
      {
        static function<TI(TI const&)>
        buildChangeHandler (TAR& target)
          {
            return bind (Mutator<TI>::imposeValueChange, ref(target), _1 );
          }
      };
    
    template<class TI>
    template<class TAR>
    struct Mutator<TI>::MutationPolicy<Offset, TAR>
      {
        static function<TI(Offset const&)>
        buildChangeHandler (TAR& target)
          {
            return bind (Mutator<TI>::imposeOffset, ref(target), _1 );
          }
      };
    
    template<class TI>
    template<class TAR>
    struct Mutator<TI>::MutationPolicy<int, TAR>
      {
        static function<TI(int)>
        buildChangeHandler (TAR& target)
          {
            return bind (Mutator<TI>::imposeNudge, ref(target), _1 );
          }
      };
    
    
    //special cases for changing Durations....

    template<class TI>
    template<class TAR>
    struct Mutator<TI>::MutationPolicy<                  typename enable_if< isDuration<TAR>, 
                                       TI>::type, TAR>
      {
        static function<TI(TI const&)>
        buildChangeHandler (Duration& target)
          {
            return bind (Mutator<TI>::changeDuration, ref(target), _1 );
          }
      };
    
    //    
//    template<class TI>
//    template<class TAR>
//    struct Mutator<TI>::MutationPolicy<TI, TAR>
//      {
//        static function<TimeValue(TimeValue const&)>
//        buildChangeHandler (Duration& target)
//          {
//            return bind (Mutator<TI>::changeDuration, ref(target), _1 );
//          }
//      };
    
//    template<class TI>
//    template<>
//    struct Mutator<TI>::MutationPolicy<int, Duration>
//      {
//        static function<TI(int)>
//        buildChangeHandler (Duration& target)
//          {
//            return bind (Mutator<TI>::nudgeDuration, ref(target), _1 );
//          }
//      };
    
    
    
    
    
    template<class TI>
    template<class TAR>
    void
    Mutator<TI>::bind_to (TAR& target)  const
    {
      setVal_ = Policy<TI,TI,TAR>    ::buildChangeHandler (target);
      offset_ = Policy<TI,Offset,TAR>::buildChangeHandler (target);
      nudge_  = Policy<TI,int,TAR>   ::buildChangeHandler (target);
    }
    
    template<class TI>
    void
    Mutator<TI>::unbind()
    {
      setVal_ = ValueSetter();
      offset_ = Ofsetter();
      nudge_  = Nudger();
    }
    
    
    
    /**
     * Implementation building block: propagate changes to listeners.
     * The Propagator manages a set of callback signals, allowing to
     * propagate notifications for changed Time values.
     * 
     * @todo WIP-WIP-WIP
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
  }
  
  
  /**
   * Frontend/Interface: controller-element for retrieving and
   * changing running time values
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
  
  
  
  
  /* === implementation === */
  
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
