/*
  DISPOSITION.hpp  -  actual constitution of a parameter descriptor

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file disposition.hpp
 ** Concrete configuration of a parameter descriptor with type and value provision.
 ** This is the full definition, with all details spelled out, and what is actually
 ** stored within the _outer shell_ of a Parameter. The publicly visible interface
 ** of the latter is implemented by delegating to the constituents of the disposition.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_DISPOSITION_H
#define LIB_PAR_DISPOSITION_H


#include "lib/par/provision.hpp"
#include "lib/par/param-type.hpp"


namespace lib {
namespace par {

  
  /**
   * Implementation-Service: complete Parameter functionality.
   * Includes the ability to access / set a »value«, attach automation,
   * identify and convert underlying value data types and handle
   * trait- and identity tags and the specifics of the value scale.
   */
  class Disposition
    : public Domain
    {
    public:
      template<typename X>
      X extract()  const;
      
      template<typename X>
      void setVal (X);
      
    private:
      template<typename X>
      bool isBaseDomainMatch()  const;
    };
  
  
  /**
   * A complete configuration and setup of a parameter, including a type specification
   * with domain and scale, and a value provision. Can be generated through a builder
   * notation and stored within the Parameter object (as a container).
   */
  template<typename VAL>
  class ParamData
    : public TypedEntity<VAL, Disposition>
    {
      VAL val_;
      Provision* src_;
      
      /* === Provision Interface === */
      void retrieveInto (ValBuff&)  const override;
      void setValFrom (ValBuff const&)    override;
      
    public:
      ParamData (VAL initVal)
        : val_{std::move (initVal)}
        , src_{nullptr}
        { }
    };
  
  template<typename X>
  using FullDomain = BaseDomain<X, Disposition>;
  
  
  
  template<typename VAL>
  void
  ParamData<VAL>::retrieveInto (ValBuff& target)  const
  {
    asValue<VAL>(target) = val_;
  }
  
  template<typename VAL>
  void
  ParamData<VAL>::setValFrom (ValBuff const& newVal)
  {
    val_ = asValue<VAL>(newVal);
  }
  
  
  
  template<typename X>
  X
  Disposition::extract()  const
  {
    X result; // RVO
    
    if (isBaseDomainMatch<X>())
      { // short-circuit by direct force-cast
        retrieveInto (asValBuff (result));
      }
    else
      { // run full type-conversion double-dispatch
        uint64_t valueBuffer; /////////////////////////////////////////////////OOO need a way to get a suitably sized intermediary buffer (without heap-allocation)
        this->retrieveInto (asValBuff(valueBuffer));
        
        FullDomain<X> targetDomain;
        this->transferTo (asValBuff(valueBuffer), targetDomain, asValBuff(result));
      }
    return result;
  }
  
  
  template<typename X>
  void
  Disposition::setVal (X changedVal)
  {
    if (isBaseDomainMatch<X>())
      { // short-circuit by direct force-cast
        setValFrom (asValBuff (changedVal));
      }
    else
      { // run full type-conversion double-dispatch
        uint64_t valueBuffer; /////////////////////////////////////////////////OOO need a way to get a suitably sized intermediary buffer (without heap-allocation)
        FullDomain<X> targetDomain;
        
        targetDomain.transferTo (asValBuff(changedVal), *this, asValBuff(valueBuffer));
        this->setValFrom (asValBuff(valueBuffer));
      }
  }
  
  
  template<typename X>
  bool
  Disposition::isBaseDomainMatch()  const
  {
    return false; /////////////////////////OOO running into a dead end here? how can we possibly access a type tag marker without indirection??
  }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_DISPOSITION_H*/
