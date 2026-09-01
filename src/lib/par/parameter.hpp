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


namespace lib {
namespace par {
  
  namespace {
    static constexpr size_t BUFF_SIZ = sizeof(ParamData<double>);
    using ParameterImplAPI = Disposition;
    using CopySupportMarker = polyvalue::CopySupport<ParameterImplAPI>;
    using ParamContainer = PolymorphicValue<ParameterImplAPI, BUFF_SIZ, CopySupportMarker>;
  }
  
  
  /**
   * Descriptor and container to hold a generic parameter.
   * The value can be fixed or supplied by an automation function.
   */
  class Parameter
    : protected ParamContainer
    {
    public:
      class Builder;
      
      template<typename VAL>
      VAL getVal();
      
      template<typename VAL>
      void setVal (VAL&&);
      
      /* === Builder API === */
      
      template<typename VAL>
      static Builder forType();
      
    protected:
      
    };
  
  
  class Parameter::Builder
    {
    public:
      Parameter
      build()
        {
          UNIMPLEMENTED ("terminal builder operation: package a disposition into the Parameter");
        }
    };
  
  
  template<typename VAL>
  Parameter::Builder
  Parameter::forType()
    {
      UNIMPLEMENTED ("how to setup the base type configuration? do we need a type parameter in the Builder class?");
    }
  
  
  template<typename VAL>
  VAL
  Parameter::getVal()
    {
      UNIMPLEMENTED ("extract the »value«, possibly applying suitable type conversions");
    }
  
  
  template<typename VAL>
  void
  Parameter::setVal (VAL&& changedVal)
    {
      UNIMPLEMENTED ("apply suitable type conversions and clamping, then transfer the new value into the Provision interface");
    }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PARAMETER_H*/
