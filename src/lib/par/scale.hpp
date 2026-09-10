/*
  SCALE.hpp  -  value scale to use for parameters

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file scale.hpp
 ** Specification of the allowed value scale of a parameter, including range and metric.
 ** The scale descriptor is used as delegate from the ParamType descriptor and maintained
 ** within a global registry for sake of deduplication. It is expected that a lot of parameters
 ** will share some common scales, like e.g. percentage values or decibels. The setup of a scale
 ** may limit the value range, provide hints for the UI representation -- yet may possibly also
 ** encompass nominal scales that comprise a selection of ID values.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_SCALE_H
#define LIB_PAR_SCALE_H


#include "lib/par/domain.hpp"

#include <optional>


namespace lib {
namespace par {
  
  using std::optional;
  
  
  enum Metric {LIN  ///< linear ordinary numeric scale
              ,BIN  ///< binary logarithm
              ,NAT  ///< natural logarithm
              ,DEC  ///< decibels (1/10 of 10-based log)
              ,NOM  ///< value encodes a nominal or ordinal scale
              };
  
  /**
   * Interface: describe the properties of a value scale to use for parameters,
   * including range limits and metric (linear, logarithmic). The core operation
   * is to conform a value to this specific scale.
   * @todo not clear if we want a virtual interface here....?
   */
  template<typename VAL>
  class Scale
    {
    public:
      Metric metric{LIN};
      /////////////////////////////OOO Quantiser?
      optional<VAL> minVal{};
      optional<VAL> maxVal{};
      optional<VAL> minUse{};
      optional<VAL> maxUse{};
      optional<VAL> neutral{};
      optional<VAL> defaultVal{};
      /////////////////////////////OOO »Sentinels« and nominal scales?
      bool zyclic{false};
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_SCALE_H*/
