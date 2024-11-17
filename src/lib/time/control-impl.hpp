/*
  CONTROL-IMPL.hpp  -  time::control implementation building blocks  

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file control-impl.hpp
 ** Implementation building blocks for time modification and propagation.
 ** The time::Control element allows to impose modifications to a connected
 ** time value entity and at the same time publish the changes to registered
 ** listeners. Due to the various flavours of actual time value entities, this
 ** is a complex undertaking, which is implemented here based on policies and
 ** template metaprogramming. This header/include defines two building blocks:
 ** - the actual Mutator to apply the changes to the target entity
 ** - a Propagator to register listeners and forward the changes.
 ** 
 ** # implementation technique
 ** 
 ** The Mutator uses functor objects to encapsulate the actual modification
 ** operations. When attaching to a target time entity to be manipulated, these
 ** functor objects will be configured by binding them to the appropriate
 ** implementation function. And picking this actual implementation is done
 ** through a time::mutation::Policy element, using the concrete time entity
 ** types as template parameter. Thus, the actual implementation to be used
 ** is determined by the compiler, through the template specialisations
 ** contained in control-policy.hpp
 ** 
 ** @note the header control-policy.hpp with the template specialisations
 **       is included way down, after the class definitions. This is done
 **       so for sake of readability
 ** 
 ** @see TimeControl_test
 **
 */

#ifndef LIB_TIME_CONTROL_IMPL_H
#define LIB_TIME_CONTROL_IMPL_H

#include "lib/error.hpp"
#include "lib/time/mutation.hpp"
#include "lib/time/timevalue.hpp"

#include <functional>
#include <vector>


namespace lib {
namespace time {
namespace mutation {
  
  using std::function;
  
  
  
  
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
      ensure_isArmed()  const
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
          ChangeSignal newListener (std::ref(toNotify));
          listeners_.push_back (newListener);
        }
      
      /** disconnect any observers */
      void
      disconnect()
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
      
      // using default construction and copy
    };
  
}}} // lib::time::mutation


/* ===== Definition of actual operations ===== */
#include "lib/time/control-policy.hpp"




template<class TI>
template<class TAR>
void
lib::time::mutation::Mutator<TI>::bind_to (TAR& target)  const
{
  using lib::time::mutation::Policy;
  
  setVal_ = Policy<TI,TI,    TAR>::buildChangeHandler (target);
  offset_ = Policy<TI,Offset,TAR>::buildChangeHandler (target);
  nudge_  = Policy<TI,int,   TAR>::buildChangeHandler (target);
}

template<class TI>
void
lib::time::mutation::Mutator<TI>::unbind()
{
  setVal_ = ValueSetter();
  offset_ = Ofsetter();
  nudge_  = Nudger();
}


#endif
