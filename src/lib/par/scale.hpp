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
  
  using util::_Fmt;
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
      bool isLogarithmic() const;
      
      /* ===== conforming operations ===== */
      
      VAL getDefault()  const;
      VAL conform (VAL const&)  const;
      
      template<typename SRC>
      VAL join (VAL const&, SRC const&, Scale<SRC> const&) const;
      VAL join (VAL const&, VAL const&)                    const;
      
      /* ===== conversion helpers ===== */
      
      template<typename TAR>
      auto asFactor (VAL const&)  const;
      
      template<typename TAR>
      auto baseScale (Metric targetMetric)  const;
      
      template<typename TAR>
      auto asLogarithm (Metric targetMetric, VAL const&)  const;
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
  
  template<typename VAL>
  inline bool
  Scale<VAL>::isLogarithmic()  const
  {
    return metric == DEC
        or metric == NAT
        or metric == BIN;
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
            throw err::Invalid {_Fmt{"Parameter value %4.2g beyond supported numeric precision "
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
    using JoinT = NumberJoinType<VAL,SRC>;
    
    if (NOM == metric)
      UNIMPLEMENTED ("nominal and ordinal scales");
    if (LIN == metric)
      {
        if (feedScale.isLogarithmic())
          assignConverted (res, value * feedScale.template asFactor<VAL> (srcFeed));
        else
        if (feedScale.isFactor())
          assignConverted (res, JoinT(value) * JoinT(srcFeed));
        else
          assignConverted (res, JoinT(value) + JoinT(srcFeed));
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
  }     // last step: re-establish target scale rules
  
  
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
  
  
  
  
  /** transform a logarithmic value into an exponential factor */
  template<typename VAL>
  template<typename TAR>
  inline auto
  Scale<VAL>::asFactor (VAL const& value)  const
  {
    using WorkType = CommonComputeType<VAL,TAR>;
    switch (metric)
      {
        case NAT: return std::exp (WorkType(value));
        case BIN: return std::exp2 (WorkType(value));
        case DEC: return std::pow<WorkType> (10, value/10.0);
        default:
          throw err::Logic{_Fmt{"Attempt to compute exponential "
                                "from a non-logarithmic value with metric %d"}
                               % metric};
        break;
      }
  }
  
  /** transform this value into a suitable logarithm for the target scale */
  template<typename VAL>
  template<typename TAR>
  inline auto
  Scale<VAL>::asLogarithm (Metric targetMetric, VAL const& value)  const
  {
    if (isNeg (value))
      throw err::Invalid{_Fmt{"Attempt to take logarithm from %f < 0"}
                             % value};
    using WorkType = CommonComputeType<VAL,TAR>;
    switch (targetMetric)
      {
        case NAT: return std::log (WorkType(value));
        case BIN: return std::logb (WorkType(value));
        case DEC: return std::log10 (WorkType(value)) * 10;
        default:
          throw err::Logic{_Fmt{"Attempt to compute logarithm "
                                "for a non-logarithmic metric %d"}
                               % targetMetric};
        break;
      }
  }
  
  /** provide the adaptation factor from this logarithmic scale
   *  to the given other logarithmic scale and target type \a VAL
   * @remarks
   *   - use the logarithm conversion formula logb(x) ≡ logb(k)·logk(x)
   *   - if target is in decibel, have to apply factor 10; asLogarithm() does that already
   *   - if we are in decibel, log10 = dB /10; fold that factor in the logarithm of the base,
   *     by using log(b)·x ≡ log(b^x) and rewrite that as b^x = exp(ln(b)·x)
   */
  
  template<typename VAL>
  template<typename TAR>
  inline auto
  Scale<VAL>::baseScale (Metric targetMetric)  const
  {
    using WorkType = CommonComputeType<VAL,TAR>;
    if (metric == targetMetric)
      return WorkType(1);
    
    auto myBase = [&]{switch (metric)
                        {
                          case NAT: return std::exp(WorkType(1));
                          case BIN: return WorkType(2);
                          case DEC: return std::exp(std::log(WorkType(10)) / 10);
                          default:
                            throw err::Logic{"Attempt to treat non-logarithmic scales as logarithms"};
                          break;
                        }};
    return asLogarithm<WorkType> (targetMetric, myBase());
  }
  
  
}} // namespace lib::par
#endif /*LIB_PAR_SCALE_H*/
