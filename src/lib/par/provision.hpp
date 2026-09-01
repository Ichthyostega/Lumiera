/*
  PROVISION.hpp  -  determine how a parameter's value is given

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file provision.hpp
 ** Provide access to a parameter's value, either directly or by automation.
 ** Each parameter has a _base value_ and a time-based current value. The latter
 ** is retrieved by evaluating an automation function.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_PROVISION_H
#define LIB_PAR_PROVISION_H


#include "lib/time/timevalue.hpp"


namespace lib {
namespace par {
  
  using time::Time;
  
  /** Marker for an opaque value buffer with
   *  undisclosed size and content type */
  struct ValBuff { /*placeholder*/ };
  
  
  template<typename X>
  constexpr inline ValBuff&
  asValBuff (X& something)
  {
    void* rawMem{& something};
    return * static_cast<ValBuff*> (rawMem);
  }
  
  template<typename X>
  constexpr inline ValBuff const&
  asValBuff (X const& something)
  {
    void const * rawMem{& something};
    return * static_cast<ValBuff const *> (rawMem);
  }
  
  template<typename X>
  constexpr inline X&
  asValue (ValBuff& storage)
  {
    void* rawMem{& storage};
    return * static_cast<X*> (rawMem);
  }
  
  template<typename X>
  constexpr inline X const&
  asValue (ValBuff const& storage)
  {
    void const * rawMem{& storage};
    return * static_cast<X const *> (rawMem);
  }
  
  
  
  
  /**
   * Interface: access to a parameter's value, to adjust or evaluate automation.
   * Conceptually, the value is decomposed into a _base value_ and possibly a
   * time-controlled adjustment (automation). The base value can be retrieved
   * and assigned, while the current time-based value is the result of evaluation.
   * Any parameter value is implicitly associated with a Type that defines the
   * underlying [value domain](\ref Domain) and possibly a \ref Scale that
   * defines range limits and metric constraints.
   * @remark the value access and manipulation works _backwards_ from an implicit
   *         value that is conceptually located _within_ this provision towards
   *         an ValBuff for external access. The reason for this indirect
   *         formulation is that the »value« is assumed to comply to some data type,
   *         which however remains an opaque internal detail and is not disclosed.
   */
  class Provision
    {
    public:
      virtual ~Provision();  ///< this is an interface
      
      virtual void retrieveInto (ValBuff&)  const { /*NOP*/ }
      virtual void pullAt (Time, ValBuff&)  const { /*NOP*/ }
      virtual void setValFrom (ValBuff const&)    { /*NOP*/ }
      virtual void connect (Provision&)           { /*NOP*/ }
      virtual void disconnnect()                  { /*NOP*/ }
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PROVISION_H*/
