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


#include "lib/error.hpp"
#include "lib/par/domain.hpp"
#include "lib/par/type-handler.hpp"
#include "lib/util-quant.hpp"
#include "lib/format-string.hpp"
//#include "lib/util.hpp"

#include <optional>


namespace lib {
namespace par {
  namespace err = lumiera::error;
  
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
      VAL cyclicLim =0;
      
      /* ===== information functions ===== */
      
      bool isValid()  const;
      bool isFactor() const;
      bool isCyclic() const;
      
      /* ===== conforming operations ===== */
      
      VAL getDefault()  const;
      VAL conform (VAL const&)  const;
      
      template<typename SRC>
      VAL join (VAL const&, SRC const&, Scale<SRC> const&) const;
      VAL join (VAL const&, VAL const&)                    const;
      
    private:
      template<typename TAR>
      VAL asFactor (VAL const&)  const;
      
      template<typename TAR>
      VAL baseScale (Metric targetMetric)  const;
      
      template<typename TAR>
      VAL asLogarithm (Metric targetMetric, VAL const&)  const;
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
       and (not isCyclic()
           or (minVal and maxVal
               and *minVal < *maxVal
               and VAL(0) < cyclicLim)
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
  
  template<typename VAL>
  inline bool
  Scale<VAL>::isCyclic()  const
  {
    return VAL(0) != cyclicLim;
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
    if (isCyclic())
      {
        REQUIRE (minVal and maxVal);
        REQUIRE (*minVal < *maxVal);
        if (std::is_floating_point_v<VAL>)
          if (not (std::abs (rawVal) < cyclicLim))
            throw err::Invalid {util::_Fmt{"Parameter value %4.2g beyond supported numeric precision "
                                           "for cyclic wrapping into [%1.2g...%1.2g[ "}
                                          % rawVal % *minVal % *maxVal
                               };
        return util::cyclicWrap (rawVal, *minVal, *maxVal);
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
  Scale<VAL>::join (VAL const& value, SRC const& srcFeed, Scale<SRC> const& feedScale) const
  {
    VAL res{};
    if (NOM == metric)
      UNIMPLEMENTED ("nominal and ordinal scales");
    if (LIN == metric)
      {
        if (feedScale.isLogarithmic())
          assignConverted (res, value * feedScale.template asFactor<VAL> (srcFeed));
        else
        if (this->isFactor())
          assignConverted (res, value * srcFeed);
        else
          assignConverted (res, value + srcFeed);
      }
    else
    if (this->isLogarithmic())
      {
        if (feedScale.isLogarithmic())
          assignConverted (res, value + feedScale.template baseScale<VAL>(metric) * srcFeed);
        else
        if (feedScale.isFactor())
          assignConverted (res, value + feedScale.template asLogarithm<VAL> (metric, srcFeed));
        else
          throw err::Invalid {"adding a non-logarithmic data feed on top of a logarithmic base value is pointless."};
      }
    else
      NOTREACHED ("Unexpected metric conversion case");
    
    return conform (res);
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
  
  template<typename VAL>
  template<typename TAR>
  inline VAL
  Scale<VAL>::asFactor (VAL const& value)  const
  {
    UNIMPLEMENTED ("transform a logarithmic value into an exponential factor");
  }
  
  template<typename VAL>
  template<typename TAR>
  inline VAL
  Scale<VAL>::baseScale (Metric targetMetric)  const
  {
    UNIMPLEMENTED ("provide the adaptation factor from this logarithmic scale to the given other logarithmic scale");
  }
  
  template<typename VAL>
  template<typename TAR>
  inline VAL
  Scale<VAL>::asLogarithm (Metric targetMetric, VAL const& value)  const
  {
    UNIMPLEMENTED ("transform this value into a suitable logarithm for the target scale");
  }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_SCALE_H*/
