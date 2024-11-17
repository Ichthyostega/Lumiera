/*
  CONTROL-POLICY.hpp  -  detail definition of actual time changing functionality  

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file control-policy.hpp
 ** Definition of special cases when imposing a change onto concrete time values.
 ** The time::Control element allows to impose modifications to a connected
 ** time value entity and at the same time publish the changes to registered
 ** listeners. Due to the various flavours of actual time value entities, this
 ** is a complex undertaking, which is implemented here based on policies and
 ** template metaprogramming.
 ** 
 ** The header control-impl.hpp defines the building blocks for time::Control
 ** and then includes this header here to get the concrete template specialisations
 ** for time::mutation::Policy. This policy class is templated by time entity types
 ** - for \c TI, the _nominal_ value type used on the time::Control interface
 ** - for \c SRC, the actual type of values to impose _as change_
 ** - for \c TAR, the target time value's type, receiving those changes.
 ** 
 ** # mutating a time value entity
 ** 
 ** Actually imposing a change to the attached time value entity involves several
 ** steps. Each of these steps might be adapted specifically, in accordance to
 ** the concrete time value types involved.
 ** - TimeValue, Time
 ** - Offset
 ** - Duration
 ** - TimeSpan
 ** - QuTime (grid aligned time value)
 ** - QuTimeSpan (planned as of 6/2011)
 ** 
 ** Moreover, the combination of types needs to be taken into account. For example,
 ** it doesn't make sense to apply a Duration value as change to a TimeValue, which
 ** has no duration (temporal extension). While a TimeSpan might receive a Duration
 ** change, but behaves differently when imposing a Time to manipulate the starting
 ** point of the time interval given by the TimeSpan.
 ** 
 ** Incoming changes might be of any of the aforementioned types, and in addition,
 ** we might receive *nudging*, which means to increment or decrement the target
 ** time value in discrete steps. After maybe adapting these incoming change values,
 ** they may be actually _imposed_ on the target. In all cases, this is delegated
 ** to the time::Mutation base class, which is declared fried to TimeValue and thus
 ** has the exceptional ability to manipulate time values, which otherwise are defined
 ** to be immutable. Additionally, these protected functions in the time::Mutation
 ** baseclass also know how to handle _nudge values_, either by using the native
 ** (embedded) time grid of a quantised time value, or by falling back to a standard
 ** nudging grid, defined in the session context (TODO as of 6/2011).                     //////////////////////TICKET #810
 ** 
 ** After (maybe) imposing a change to the target, the _change notification_ value
 ** needs to be built. This is the time value entity to be forwarded to registered
 ** listeners. This notification value has to be given as the type `TI`, in accordance
 ** to the `time::Control<TI>` frontend definition used in the concrete usage situation.
 ** As this type `TI` might be different to the actual target type, and again different
 ** to the type of the change handed in, in some cases this involves a second conversion
 ** step, to represent the current state of the target `TAR` in terms of the interface
 ** type `TI`.
 ** 
 ** ## changing quantised (grid aligned) time entities
 ** 
 ** The time::Control element includes the capability to handle grid aligned time values,
 ** both as target and as change/notification value. This ability is compiled in conditionally,
 ** as including mutation.hpp causes several additional includes, which isn't desirable when
 ** it comes just to changing plain time values. Thus, to get these additional specialisations,
 ** the LIB_TIME_TIMEQUANT_H header guard needs to be defined, which happens automatically
 ** if lib/time/mutation.hpp is included prior to lib/time/control.hpp.
 ** 
 ** As a special convention, any \em quantised (grid aligned) types involved in these
 ** time changes will be \em materialised, whenever a type conversion happens. Generally
 ** speaking, a quantised time value contains an (opaque) raw time value, plus a reference
 ** to a time grid definition to apply. In this context \em materialising means actually
 ** to apply this time grid to yield a grid aligned value. Thus, when using a quantised
 ** value to impose as change (or to receive a change), its grid aligning nature
 ** becomes effective, by applying the \em current definition of the grid to
 ** create a fixed (materialised) time value, aligned to that current grid.
 ** 
 ** @todo 6/2011 include all the special cases for QuTimeSpan                               ////////////////////TICKET #760
 ** 
 ** @see TimeControl_test
 **
 */

#ifndef LIB_TIME_CONTROL_POLICY_H
#define LIB_TIME_CONTROL_POLICY_H

#include "lib/time/mutation.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/utility/enable_if.hpp>
#include <type_traits>
#include <functional>


namespace lib {
namespace time {
  
namespace mutation {
  
  using boost::disable_if;
  using std::__or_;
  using std::is_same;
  using std::placeholders::_1;
  using std::function;
  using std::bind;
  using std::ref;
  
  
  
  namespace { // metaprogramming helpers to pick a suitable implementation branch...
    
    template<class T>
    inline bool
    isDuration()
    {
      return is_same<T, Duration>::value;
    }
    
    template<class T>
    inline bool
    isTimeSpan()
    {
      return is_same<T, TimeSpan>::value;
    }
    
    template<class T>
    inline T const&
    maybeMaterialise (T const& non_grid_aligned_TimeValue)
    {
      return non_grid_aligned_TimeValue;
    }
    
#ifdef LIB_TIME_TIMEQUANT_H
    inline QuTime
    maybeMaterialise (QuTime const& alignedTime)
    {
      PQuant const& grid(alignedTime);
      return QuTime(grid->materialise(alignedTime), grid);
    }
#endif //--quantised-time-support
  }
  
  
  /** 
   * Implementation policy: how to build a new
   * notification value of type \c TI, given a 
   * target time value entity of type \c TAR
   */
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
#ifdef LIB_TIME_TIMEQUANT_H
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
#endif //--quantised-time-support
  
  
  
  
  /**  
   * Policy to tie the various detail policies together
   * for providing actual value change operations.
   * The standard case uses the (inherited) time::Mutation
   * base implementation to impose a new value onto the
   * target entity and then uses the Builder policy to
   * create a notification value reflecting this change.
   */
  template<class TI, class TAR>
  struct Link
    : Mutation
    , Builder<TI,TAR>
    {
      
      template<class SRC>
      static TI
      processValueChange (TAR& target, SRC const& change)  ///< standard case: plain value change
        {
          imposeChange (target, maybeMaterialise(change));
          return Link::buildChangedValue (maybeMaterialise(target));
        }
      
      static TI
      useLengthAsChange (TAR& target, TimeSpan const& change)
        {
          return processValueChange(target, change.duration());
        }
      
      static TI
      mutateLength (TimeSpan& target, Duration const& change)
        {
          imposeChange (target.duration(), change);
          return Builder<TI,TimeSpan>::buildChangedValue(target);
        }
      
      static TimeSpan
      mutateTimeSpan (TimeSpan& target, TimeSpan const& change)
        {
          imposeChange (target.duration(), change.duration());
          imposeChange (target,change.start());
          return Builder<TimeSpan,TimeSpan>::buildChangedValue(target);
        }
      
      static TI
      dontChange (TAR& target)  ///< @note: not touching the target
        {
          return Link::buildChangedValue(target);
        }
    };
  
  
  
  /** 
   * Policy how to impose changes onto a connected target time value entity
   * This policy will be parametrised with the concrete time entity types
   * involved in the usage situation of time::Control. The purpose of the
   * policy is to \em bind a functor object to the concrete implementation
   * of the value change applicable for this combination of types.
   * This functor will then be stored within time::Control and
   * invoked for each actual value change.
   * @param TI the nominal (interface) type of the change, propagated to listeners
   * @param SRC the actual type of the change to be imposed
   * @param TAR the actual type of the target entity to receive the changes
   * @note typically either SRC is identical to TI, or it is an
   *       time::Offset, or an int for \em nudging the target 
   */
  template<class TI, class SRC, class TAR>
  struct Policy
    {
      static function<TI(SRC const&)>
      buildChangeHandler (TAR& target)
        {
          return bind (Link<TI,TAR>::template processValueChange<SRC>, ref(target), _1 );
        }
    };
  
  
  // special treatment of Durations as target------------------------------------
  
  namespace {
    template<class T>
    struct canMutateDuration
      : __or_< is_same<T,Duration>
             , is_same<T,Offset>
             , is_same<T,int>
             >
      { };
    
    template<class T>
    struct canReceiveDuration
      : __or_< is_same<T,Duration>
             , is_same<T,TimeSpan>
             >
      { };
  }
  
  
  /** 
   * special case: a Duration target value can't be changed by plain time values.
   * This specialisation is \em not used (\c disable_if ) when the given change (SRC)
   * is applicable to a Duration in a sensible way. We either define explicit
   * specialisations (for TimeSpan) or fall back to the default in these cases. 
   */
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
  
  /**
   * special case: a Duration change value can't be imposed to a plain time value.
   * In these cases, we even propagate a Duration::ZERO to the listeners.
   * As above, there are exceptions to this behaviour, where a Duration change
   * can sensibly be applied. 
   */
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
  
  
  // special treatment for TimeSpan values---------------------------------------
  
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
  
  
  
}}} // namespace lib::time::mutation
#endif
