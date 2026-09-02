/*
  SPEC.hpp  -  parameter trait specification

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file spec.hpp
 ** Common definition and specification record for parameters.
 ** While most functionality of parameters is indirected in some way, allowing
 ** for future extension, some basic traits are marked as flag, so that frequently
 ** used code-paths might take a shortcut. Notably the disposition interface, which
 ** describes the full implementation service of a parameter container, explicitly
 ** reserves one «slot» of storage for flags, type markers and an ID record.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_SPEC_H
#define LIB_PAR_SPEC_H


#include "lib/symbol.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"



namespace lib {
namespace par {
  
  /**
   * Fixed collection of elementary types supported in parameters
   */
  using BaseTypes = meta::Types<int
                               ,int64_t
                               ,uint
                               ,uint64_t
                               ,double
                               ,float
                               ,bool
                               >;
  
  static constexpr uint BASETYPE_CNT     = meta::count<BaseTypes::List>();
  static constexpr uint BASETYPE_MAX_SIZ = meta::maxSize<BaseTypes::List>();
  
  template<typename T>
  static constexpr bool isBaseType = meta::isInList<T, BaseTypes::List>::value;
  
  /** @note Only some fixed base data types are supported as Parameter value type. */
  template<typename T>
  concept par_basetype = isBaseType<T>;
  
  template<typename T>
  using BaseTypeID = meta::TypeIdxID<T, BaseTypes::List>;
  
  
  
  /* ===== setup of ID space ===== */
  
  namespace {
    auto _bit_fit = [](uint bits){ return (1u << bits) - 1; };
  }
  
  static constexpr uint _BASE_TYPE_BITS = 4;
  static constexpr uint    _SCALES_BITS = 10;
  static constexpr uint  _PARAM_ID_BITS = 18;
  
  static constexpr uint MAX_BASETYPES = _bit_fit(_BASE_TYPE_BITS); ///< max number of parameter base types that can be configured
  static constexpr uint MAX_SCALES    = _bit_fit(_SCALES_BITS);    ///< max number of distinct scales that can be defined at runtime
  static constexpr uint MAX_PARAM_ID  = _bit_fit(_PARAM_ID_BITS);  ///< max number of distinct param identities that can be created
  
  static_assert (BASETYPE_CNT <= MAX_BASETYPES);
  
  
  
  struct IDRecord
    {
      uint baseTypeID :_BASE_TYPE_BITS  = 0;
      uint scaleID    :_SCALES_BITS     = 0;
      uint paramIDID  :_PARAM_ID_BITS   = 0;
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_SPEC_H*/
