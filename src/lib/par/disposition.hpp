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


#include "lib/par/spec.hpp"
#include "lib/par/provision.hpp"
#include "lib/par/param-type.hpp"


namespace lib {
namespace par {

  using ValStorage = ValBuffStorage<BASETYPE_MAX_SIZ>;
  
  
  /**
   * Implementation-Service: complete Parameter functionality.
   * Includes the ability to access / set a »value«, attach automation,
   * identify and convert underlying value data types and handle
   * trait- and identity tags and the specifics of the value scale.
   */
  class Disposition
    : public Domain
    {
      IDRecord idr_;
      
    public:
      template<typename X>
      X extract()  const;
      
      template<typename X>
      void setVal (X);
      
    protected:
      template<typename X>
      bool
      isBaseDomainMatch()  const
        {
          return idr_.baseTypeID == BaseTypeID<X>();
        }
      
      template<typename X>
      void
      markBaseDomain()
        {
          idr_.baseTypeID = BaseTypeID<X>();
        }
    };
  
  
  
  /**
   * A complete configuration and setup of a parameter, including a type specification
   * with domain and scale, and a value provision. Can be generated through a builder
   * notation and stored within the Parameter object (as a container).
   */
  template<typename VAL, class IFA =Disposition>        /////////////////////////////////////////////////////TICKET #1197 : the second template argument is quite confusing and became necessary due to the limitations of PolymorphicValue
  class ParamData
    : public TypedEntity<VAL, IFA>
    {
      VAL val_;
      Provision* src_;
      
    public:
      ParamData (VAL initVal)
        : val_{std::move (initVal)}
        , src_{nullptr}
        {
          Disposition::markBaseDomain<VAL>();
        }
      
      /* === Provision Interface === */
      void retrieveInto (ValBuff&)  const override final;    ///< @note is sometimes invoked non-virtually for sake of optimisation 
      void setValFrom (ValBuff const&)    override final;
    };
  
  template<typename X>
  using FullDomain = BaseDomain<X, Disposition>;
  
  template<typename X>
  using SimpleData = ParamData<X, Disposition>;
  
  
  
  template<typename VAL, class IFA>
  void
  ParamData<VAL, IFA>::retrieveInto (ValBuff& target)  const
  {
    asValue<VAL>(target) = val_;
  }
  
  template<typename VAL, class IFA>
  void
  ParamData<VAL, IFA>::setValFrom (ValBuff const& newVal)
  {
    val_ = asValue<VAL>(newVal);
  }
  
  
  
  /**
   * @warning optimisation to a direct access will be applied
   *   when the baseTypeID in the IDRecord matches the type \a X.
   *   It is assumed in this case that the actual implementation of
   *   the Disposition interface is a ParamData<X>, allowing to side-step
   *   any virtual dispatch; since the implementation of #retrieveInto and
   *   #setValFrom is defined in the same source file, the optimiser will
   *   inline the call, resulting in a direct value access.
   */
  template<typename X>
  X
  Disposition::extract()  const
  {
    X result; // RVO
    
    if (isBaseDomainMatch<X>())
      { // short-circuit by direct force-cast, bypassing virtual dispatch
        auto& paramData = * static_cast<SimpleData<X> const *> (this);
        paramData.SimpleData<X>::retrieveInto (asValBuff (result));
      }
    else
      { // run full type-conversion dispatch
        ValStorage valueBuffer;
        this->retrieveInto (valueBuffer);
        this->extractAs (result, valueBuffer);
      }
    return result;
  }
  
  
  template<typename X>
  void
  Disposition::setVal (X changedVal)
  {
    if (isBaseDomainMatch<X>())
      { // short-circuit by direct force-cast, bypassing virtual dispatch
        auto& paramData = * static_cast<SimpleData<X>*> (this);
        paramData.SimpleData<X>::setValFrom (asValBuff (changedVal));
      }
    else
      { // run full type-conversion dispatch
        ValStorage valueBuffer;
        this->conform (valueBuffer, changedVal);
        this->setValFrom (valueBuffer);
      }
  }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_DISPOSITION_H*/
