/*
  DOMAIN.hpp  -  represent the value domain of parameters

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file domain.hpp
 ** Specification of a base type and a value domain for parameters.
 ** A Domain descriptor implicitly also encodes the concrete value type
 ** and exposes a set of virtual functions to handle all supported value conversions.
 ** Implementing this interface creates a conversion matrix and allows to implement
 ** generic conversions by double-dispatch. Since virtual interfaces must be complete
 ** at compile time however, such an approach requires to define a finite set of
 ** supported parameter value types, which seems adequate, since the universe
 ** of possible values is limited anyway: elementary values in parameters
 ** must be primitive base types that can be created and destroyed
 ** without side effect. Relying on the further features of
 ** the parameter type system, compound types (vectors)
 ** and limited sub-scales can be defined.
 ** 
 ** @todo WIP-WIP-WIP this is a draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_DOMAIN_H
#define LIB_PAR_DOMAIN_H


#include "lib/meta/typelist.hpp"
//#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"


namespace lib {
namespace par {
      
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
  
  template<typename X, class BAS>
  class TypeHandler
    : public BAS
    {
      public:
        virtual void extractAs (X&, ValBuff&)  =0;
    };
  
//  using lib::meta::typeseq;
  
  
  using BaseTypes = meta::Types<int
                               ,int64_t
                               ,uint
                               ,uint64_t
                               ,double
                               ,bool
                               >;
  /** build a generic visitor interface for all types in list */
  using TypeHandlerInterface = meta::InstantiateChained<BaseTypes::List, TypeHandler>;
  
  
  /**
   * Interface: mark the allowed value domain of a parameter value.
   * The identity of the actual implementation can be used as type descriptor,
   * since all concrete full base domains are registered at compile time; the
   * virtual Domain interface exposes the possible conversions.
   * @todo 7/2026 not clear if we can support all conversions unconditionally,
   *       or if some notion of an _impossible conversion_ is required. It is
   *       clear however, that the access to an actual parameter **must fail**
   *       at compile time, if a sensible conversion can not be provided.
   */
  class Domain
    : public TypeHandlerInterface
    {
    public:
      virtual ~Domain();  ///< this is an interface
      
    protected:
      
    };
  
  
  template<typename X>
  class BaseDomain
    : public Domain
    {
      void extractAs (uint&, ValBuff& valBuff) override { UNIMPLEMENTED("ex uint"); }
      void extractAs (double&, ValBuff& valBuff) override { UNIMPLEMENTED("ex double"); }
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_DOMAIN_H*/
