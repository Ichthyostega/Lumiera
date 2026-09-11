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
#include "lib/util.hpp"

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
      optional<VAL> defVal{};
      /////////////////////////////OOO »Sentinels« and nominal scales?
      bool cyclic{false};
      
      /* ===== information functions ===== */
      
      bool isValid()  const;
      bool isFactor() const;
      
      /* ===== conforming operations ===== */
      
      VAL getDefault()  const;
      VAL conform (VAL const&)  const;
      
      template<typename SRC>
      VAL join (VAL const&, SRC const&, Scale<SRC> const&) const;
      VAL join (VAL const&, VAL const&)                    const;
    };
  
  
  /* ===== information functions ===== */
  
  /** self consistency check */
  template<typename VAL>
  inline bool
  Scale<VAL>::isValid()  const
  {
    return ((not minVal or not maxVal)
           or *minVal <= *maxVal
           )
       and (not minUse
           or (not minVal or *minVal <= *minUse)
           )
       and (not maxUse
           or (not maxVal or *maxUse <= *maxVal)
           )
       and ((not minUse or not maxUse)
           or *minUse <= *maxUse
           )
       and (not neutral
           or (   (not maxVal or *neutral <= *maxVal)
              and (not minVal or *minVal <= *neutral)
              )
           )
       and (not defVal
           or (   (not maxVal or *defVal <= *maxVal)
              and (not minVal or *minVal <= *defVal)
              )
           )
       and (not cyclic
           or (minVal and maxVal and *minVal < *maxVal)
           )
           ;
  }
  
  template<typename VAL>
  inline bool
  Scale<VAL>::isFactor()  const
  {
    return metric == LIN
       and neutral
       and *neutral == VAL(1);
  }
  
  
  
  /* ===== conforming operations ===== */
  
  /** Build initial value that fits into the scale. */
  template<typename VAL>
  inline VAL
  Scale<VAL>::getDefault()  const
  {
    return defVal? *defVal
         : neutral? *neutral
         : conform (VAL{});
  }
  
  /**
   * Accommodate a raw value from the underlying domain,
   * so that it conforms with this Scale definition.
   */
  template<typename VAL>
  inline VAL
  Scale<VAL>::conform (VAL const& rawVal)  const
  {
    if (cyclic)
      {
        REQUIRE (minVal and maxVal);
        REQUIRE (*minVal < *maxVal);
        const VAL PERIOD = *maxVal - *minVal;
        ENSURE (0 < PERIOD);
        return (rawVal - *minVal) % PERIOD;
      }
    if (minVal and rawVal < *minVal)
      return *minVal;
    if (maxVal and rawVal > *maxVal)
      return *maxVal;
    // no further constraint to enforce...
    return rawVal;
  }
  
  /**
   * Join and combine an additional feed value with an anchor value.
   * @note actual joining operation is picked based on both scales and types involved.
   * @remark notably multiplicative contributions and logarithmic scales to be considered.
   */
  template<typename VAL>
  template<typename SRC>
  inline VAL
  Scale<VAL>::join (VAL const& value, SRC const& feed, Scale<SRC> const& feedScale) const
  {
    UNIMPLEMENTED ("join a feed with a base value, observing Scale constraints");
  }
  
  /**
   * Simplified join() variant, assuming the feed is based on the same scale.
   */
  template<typename VAL>
  inline VAL
  Scale<VAL>::join (VAL const& value, VAL const& feed)  const
  {
    return conform (isFactor()? value * feed
                              : value + feed
                   );
  }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_SCALE_H*/
