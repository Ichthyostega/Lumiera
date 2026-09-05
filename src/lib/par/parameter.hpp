/*
  PARAMETER.hpp  -  generic parameter values

   Copyright (C)
     2008,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file parameter.hpp
 ** A generic typed value that can be discovered and automated.
 ** The actual data type of the parameter is an opaque implementation detail,
 ** yet value ranges and conversions are handled automatically. It is thus
 ** both a descriptor and an opaque container. Parameter elements can be copied,
 ** yet the presence of some specific parameter at a given point in the model
 ** constitutes an identity -- and the parameter descriptor can be tagged
 ** with that identity.
 ** 
 ** @todo this is a draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_PARAMETER_H
#define LIB_PAR_PARAMETER_H


#include "lib/par/disposition.hpp"
#include "lib/polymorphic-value.hpp"
#include "lib/util.hpp"

#include <utility>


namespace lib {
namespace par {
  
  namespace {
    static constexpr size_t PAYLOAD_SIZ = sizeof(ParamData<double>) - sizeof(void*); ////////////////////////TICKET #1197 : this is a mess! PolymorphicValue adds the ADMIN_OVERHEAD (but our payload has already a VTable)
    using ParameterImplAPI = Disposition;
    using CopySupportMarker = polyvalue::CopySupport<ParameterImplAPI>;
    using ParamContainer = PolymorphicValue<ParameterImplAPI, PAYLOAD_SIZ, CopySupportMarker>;
  }
  
  using util::unConst;
  
  
  
  /**
   * Descriptor and container to hold a generic parameter.
   * The value can be fixed or supplied by an automation function.
   */
  class Parameter
    : protected ParamContainer
    {
    public:
      template<typename VAL>
      VAL getVal() const;
      
      template<typename VAL>
      void setVal (VAL&&);
      
      /* === Builder API === */
      
      template<typename VAL>
      class Builder;
      
      template<typename VAL>
      static Builder<VAL>
      forType() { return {}; }
      
      template<typename VAL>
      static Builder<std::decay_t<VAL>>
      forValue (VAL&& initVal)
        {
          using PayloadType = std::decay_t<VAL>;
          return forType<PayloadType>()
                  .withValue (std::forward<VAL> (initVal));
        }
      
      class ProtoBuilder;
      static ProtoBuilder from (Parameter const& prototype);
      
    protected:
      template<class IMP, typename...ARGS>
      Parameter (IMP* typeTag, ARGS&&... args)
        : PolymorphicValue (typeTag, std::forward<ARGS>(args)...)
        { }
    };
  
  
  template<typename VAL>   requires par_basetype<VAL>
  class Parameter::Builder
    {
      VAL initVal_{};
      
    public:
      Parameter
      build()
        {
          using ImplPackage = ParamData<VAL, CopySupportMarker>;   //////////////////////////////////////////TICKET #1197 : remove the necessity to inject the CopySupportMarker
          ImplPackage* typeTag{nullptr};
          Parameter resParam{typeTag, std::move (initVal_)};
          return resParam;
        }
      
      template<typename X>
      Builder&&
      withValue (X&& initVal)
        {
          initVal_ = std::forward<X> (initVal);
          return std::move(*this);
        }
    };
  
  
  class Parameter::ProtoBuilder
    {
      Parameter ref_;
      
    public:
      ProtoBuilder (Parameter const& prototype)
        : ref_{prototype}
        { }
        
      Parameter
      build()
        {
          return ref_;
        }
      
      template<typename X>
      ProtoBuilder&&
      maxVal (X&& upperBound)
        {
          /////////////////////////////////////////////OOO how to remould a scale?
          return std::move(*this);
        }
    };
  
  inline Parameter::ProtoBuilder
  Parameter::from (Parameter const& prototype)
  {
    return ProtoBuilder{prototype};
  }

  
  
  template<typename VAL>
  inline VAL
  Parameter::getVal()  const
    {
      static_assert (isBaseType<VAL>, "only preconfigured parameter BaseTypes are supported");
      
      return unConst(this)->getPayload().extract<VAL>();
    }
  
  
  template<typename VAL>
  inline void
  Parameter::setVal (VAL&& changedVal)
    {
      static_assert (isBaseType<VAL>, "only preconfigured parameter BaseTypes are supported");
      
      getPayload().setVal (changedVal);
    }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PARAMETER_H*/
