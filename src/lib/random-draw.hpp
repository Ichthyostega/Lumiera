/*
  RANDOM-DRAW.hpp  -  randomly pick limited values

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/** @file random-draw.hpp
 ** Build a component to select limited values randomly.
 ** Prototyping to find a suitable DSL to configure drawing of random numbers and mapping results.
 ** The underlying implementation shall be extracted from (and later used by) TestChainLoad; the
 ** random numbers will be derived from node hash values and must be mapped to yield parameters
 ** limited to a very small value range. While numerically simple, this turns out to be rather
 ** error-prone, hence the desire to put a DSL in front. The challenge however arises from
 ** the additional requirement to support various usage patters, all with minimal specs.
 ** 
 ** The following code lays out the ground structure, while treating Spec as a distinct
 ** type, which is then mixed into Draw. This logical separation basically was led me to the
 ** final solution: Draw both _is_ a function and _embodies_ the implementation of this function.
 ** This somewhat surprising layout is what enables use as a DSL builder, because it allows both
 ** to have the _builder use_ and the _converter use_ in the same class, even allowing to _define_
 ** a Draw by giving a function which _produces_ a (dynamically parametrised) Draw.
 ** 
 ** In this prototype, all of the functor adaptation is also part of the Draw template; for the
 ** real implementation this will have to be supplied at usage site through a traits template,
 ** otherwise it would not be possible to integrate seamlessly with custom data sources (as
 ** happens in the intended use case, where actually a Node is the data source)
 ** 
 ** @see RandomDraw_test
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 */


#ifndef LIB_RANDOM_DRAW_H
#define LIB_RANDOM_DRAW_H


#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>


namespace lib {
  
  using lib::meta::_Fun;
  using std::function;
  using std::forward;
  using std::move;
  
  
  
  /**
   * A Result Value confined into fixed bounds.
   * @tparam T underlying base type (number like)
   * @tparam max maximum allowed param value (inclusive)
   * @tparam max minimum allowed param value (inclusive) - defaults to "zero".
   */
  template<typename T, T max, T min =T(0)>
  struct Limited
    {
      static constexpr T minVal() { return min; }
      static constexpr T maxVal() { return max; }
      
      T val;
      
      template<typename X>
      Limited (X raw)
        : val(util::limited (X(minVal()), raw, X(maxVal())))
        { }
      
      operator T&()
        {
          return val;
        }
      operator T const&()  const
        {
          return val;
        }
    };
  
  
  namespace random_draw { // Policy definitions
    
    /**
     * Default policy for RandomDraw: generate limted-range random numbers.
     * @tparam max result values will be `uint` in the range `[0 ... max]`
     */
    template<uint max>
    struct LimitedRandomGenerate
      : function<Limited<uint, max>(void)>
      {
        static double defaultSrc() { return rand()/double(RAND_MAX); }
      };
    
  }//(End)Policy definitions
  
  
  
  /**
   * A component and builder to draw limited parameter values
   * based on some source of randomness (or hash input).
   * Effectively this is a function which "draws" on invocation.
   * @tparam POL configuration policy baseclass
   */
  template<class POL>
  class RandomDraw
    : public POL
    {
      using Fun = typename _Fun<POL>::Functor;
      using Tar = typename _Fun<POL>::Ret;
      
      Tar    maxResult_{Tar::maxVal()};      ///< maximum parameter val actually to produce < max
      double probability_{0};                ///< probability that value is in [1 .. m]
      
      
      /** @internal quantise into limited result value */
      Tar
      limited (double val)
        {
          if (probability_ == 0.0 or val == 0.0)
            return Tar{0};
          double q = (1.0 - probability_);
          auto org = Tar::minVal();
          val -= q;                          // [0 .. [q .. 1[
          val /= probability_;               // [0 .. 1[
          val *= maxResult_ - org;           // [0 .. m[
          val += org+1;                      // [1 .. m]
          val += CAP_EPSILON;                // round down yet absorb dust
          return Tar{val};
        }
      
      static size_t constexpr QUANTISER  = 1 << 8;
      static double constexpr CAP_EPSILON = 1/(2.0 * QUANTISER);
      
      /** @internal draw from source of randomness */
      double
      asRand (size_t hash)
        {
          return double(hash % QUANTISER) / QUANTISER;
        }
      
      /** @internal core operation: draw and quantise into limited value */
      Tar
      drawLimited (size_t hash)
        {
           return limited (asRand (hash));
        }
      
      
    public:
      /** Drawing is _disabled_ by default, always yielding "zero" */
      RandomDraw()
        : Fun{adaptOut(POL::defaultSrc)}
        { }
      
      /**
       * Build a RandomDraw by adapting a value-processing function,
       * which is adapted to accept the nominal input type. The effect
       * of the given function is determined by its output value
       * - `size_t`: the function output is used as source of randomness
       * - `double`: output is directly used as draw value `[0.0..1.0[`
       * - `RandomDraw` : the function yields a parametrised instance,
       *   which is directly used to produce the output, bypassing any
       *   further local settings (#probability_, #maxResult_)
       */
      template<class FUN>
      RandomDraw(FUN&& fun)
        : probability_{1.0}
        , Fun{adaptOut(adaptIn(std::forward<FUN> (fun)))}
        { }
      
      
      /* ===== Builder API ===== */
      
      RandomDraw&&
      probability (double p)
        {
          probability_ = p;
          return move (*this);
        }
      
      RandomDraw&&
      maxVal (Tar m)
        {
          maxResult_ = m;
          return move (*this);
        }
  
      template<class FUN>
      RandomDraw&&
      mapping (FUN&& fun)
        {
          Fun(*this) = adaptOut(adaptIn(std::forward<FUN> (fun)));
          return move (*this);
        }
      
      
      
      
    private:
      
      template<class FUN>
      decltype(auto)
      adaptIn (FUN&& fun)
        {
          static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
          
          using Sig     = typename lib::meta::_Fun<FUN>::Sig;
          using Adaptor = typename POL::template Adaptor<Sig>;
          
          return Adaptor::build (forward<FUN> (fun));
        }
      
      template<class FUN>
      decltype(auto)
      adaptOut (FUN&& fun)
        {
          static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
          
          using Res = typename lib::meta::_Fun<FUN>::Ret;
          using lib::meta::func::chained;
          
          if constexpr (std::is_same_v<Res, Tar>)//  ◁──────────────────────────┨ function produces result directly
            return std::forward<FUN>(fun);
          else
          if constexpr (std::is_same_v<Res, size_t>)//  ◁───────────────────────┨ function yields random source to draw value
            return chained (std::forward<FUN>(fun)
                           ,[this](size_t hash){ return drawLimited(hash); });
          else
          if constexpr (std::is_same_v<Res, double>)//  ◁───────────────────────┨ function yields random value to be quantised
            return chained (std::forward<FUN>(fun)
                           ,[this](double rand){ return limited(rand); });
          else
          if constexpr (std::is_same_v<Res, RandomDraw>)// ◁────────────────────┨ function yields parametrised RandomDraw to invoke
            return [functor=std::forward<FUN>(fun), this]
                   (auto&& ...inArgs)
                      {                              // invoke with copy
                        RandomDraw parametricDraw = functor(inArgs...);
                        return parametricDraw (forward<decltype(inArgs)> (inArgs)...);
                      };
          else
            static_assert (not sizeof(Res), "unable to adapt / handle result type");
          NOTREACHED("Handle based on return type");
        }
      
    };
  
  
} // namespace lib
#endif /*LIB_RANDOM_DRAW_H*/
