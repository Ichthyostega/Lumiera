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
 ** Generally speaking, RandomDraw uses some suitable source of randomness to "draw" a result
 ** value with a limited target domain. The intended usage scenario is to parametrise some
 ** configuration or computation »randomly«, with well defined probabilities and value ranges.
 ** A DSL is provided to simplify the common configuration and value mapping scenarios.
 ** @paragraph The underlying implementation was extracted 11/2023 from (and later used by)
 **     TestChainLoad; there, random numbers are derived from node hash values and must be mapped
 **     to yield control parameters governing the topology of a DAG datastructure. Notably, a
 **     draw is performed on each step to decide if the graph should fork. While numerically
 **     simple, this turned out to be rather error-prone, and resulting code is dense and
 **     difficult to understand, hence the desire to wrap it into a library component.
 ** 
 ** # Implementation structure
 ** RandomDraw inherits from a _policy template_, which in turn is-a std::function. The signature
 ** of this function defines the input to work on; its output is assumed to be some variation
 ** of a [»limited value«](\ref Limited). Notably, results are assumed to conform to an ordered
 ** interval of integral values. The [core functionality](\ref drawLimited) is to use the value
 ** from the random source (a `size_t` hash), break it down by some _modulus_ to create an arbitrary
 ** selection, followed by mapping this _drawn value_ into the target value range. This mapping allows
 ** to discard some of the _possible drawn values_ however — which equates to define a probability of
 ** producing a result different than "zero" (the neutral value of the result range). Moreover, the
 ** actual value mapping can be limited and configured within the confines of the target type.
 ** 
 ** Additional flexibility can be gained by _binding a functor,_ thereby defining further mapping and
 ** transformations. A wide array of function signatures can be accepted, as long as it is possible
 ** somehow to _adapt_ those functions to conform to the overall scheme as defined by the Policy base.
 ** Such a mapping function can be given directly at construction, or it can be set up later through
 ** the configuration DSL. As a special twist, it is even possible to bind a function to _manipulate_
 ** the actual instance of RandomDraw dynamically. Such a function takes `RandomDraw&` as first
 ** argument, plus any sequence of further arguments which can be adapted from the overall input;
 ** it is invoked prior to evaluating each input value and can tweak the instance by side-effect.
 ** After that, the input value is passed to the adapted instance.
 ** 
 ** ## Policy template
 ** For practical use, the RandomDraw template must be instantiated with a custom provided
 ** policy template. This configuration allows to attach to locally defined types and facilities.
 ** The policy template is assumed to conform to the following requirements:
 ** - its base type is std::function, with a result value similar to \ref Limited
 ** - more specifically, the result type must be number-like and expose extension points
 **   to determine the `minVal()`, `maxVal()` and `zeroVal()`
 ** - moreover, the policy must define a function `defaultSrc(args...)`; this function must
 **   accept input arguments in accordance to the function signature of the Policy (i.e. it
 **   must read "the randomness source") and produce a result that can be adapted and fed
 **   into the regular processing chain (the same as for any mapping function)
 ** - optionally, this policy may also define a template `Adaptor<Sig>`, possibly with
 **   specialisations for various function signatures. These adaptors are used to
 **   conform any mapping function and thus allow to simplify or widen the
 **   possible configurations at usage site.
 ** 
 ** ## Copy inhibition
 ** The configuration of the RandomDraw processing pipeline makes heavy use of function composition
 ** and adaptation to handle a wide selection of input types and usage patterns. Unfortunately this
 ** requires to like the generated configuration-λ to the object instance (capturing by reference);
 ** not allowing this would severely limit the possible configurations. This implies that an object
 ** instance must not be moved anymore, once the processing pipeline has been configured. And this
 ** in turn would severely limit it's usage in a DSL. As a compromise, RandomDraw relies on
 ** [lazy on-demand initialisation](\ref lazy-init.hpp): as long as the processing function has
 ** not been invoked, the internal pipeline is unconfigured, and the object can be moved and copied.
 ** Once invoked, the prepared configuration is assembled and the function »engaged«; from this point
 ** on, any attempt to move or copy the object will throw an exception, while it is still possible
 ** to assign other RandomDraw instances to this object.
 ** @todo 11/2023 This is a first draft and was extracted from an actual usage scenario.
 **   It remains to be seen if the scheme as defined is of any further use henceforth.
 ** @see RandomDraw_test
 ** @see lazy-init.hpp
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 */


#ifndef LIB_RANDOM_DRAW_H
#define LIB_RANDOM_DRAW_H


#include "lib/error.h"
#include "lib/lazy-init.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/util-quant.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>


namespace lib {
  namespace err = lumiera::error;
  
  using lib::meta::disable_if_self;
  using lib::meta::_Fun;
  using std::function;
  using std::forward;
  using std::move;
  
  
  
  /**
   * A Result Value confined into fixed bounds.
   * @tparam T underlying base type (number like)
   * @tparam max maximum allowed param value (inclusive)
   * @tparam max minimum allowed param value (inclusive) - defaults to "zero".
   * @tparam zero the _neutral value_ in the value range
   */
  template<typename T, T max, T min =T(0), T zero =min>
  struct Limited
    {
      static_assert (min < max);
      static_assert (min <= zero and zero < max);
      
      static constexpr T maxVal() { return max; }
      static constexpr T minVal() { return min; }
      static constexpr T zeroVal(){ return zero;}
      
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
  
  
  
  
  /**********************************************************//**
   * A component and builder to draw limited parameter values
   * based on some source of randomness (or hash input).
   * Effectively this is a function which "draws" on invocation.
   * Probabilities and ranges can be configured by builder API.
   * @tparam POL configuration policy baseclass
   */
  template<class POL>
  class RandomDraw
    : public LazyInit<POL>
    {
      using Lazy = LazyInit<POL>;
      using Disabled = typename Lazy::MarkDisabled;
      
      using Sig = typename _Fun<POL>::Sig;
      using Fun = function<Sig>;
      using Tar = typename _Fun<POL>::Ret;
      
      Tar    maxResult_{Tar::maxVal()};      ///< maximum result val actually to produce < max
      Tar    minResult_{Tar::minVal()};      ///< minimum result val actually to produce > min
      double probability_{0};                ///< probability that value is in [min .. max] \ neutral
      size_t shuffle_{0};                    ///< statefull additional randomisation to inject into hash 
      
      
      /** @internal quantise into limited result value */
      Tar
      limited (double val)
        {
          if (probability_ == 0.0)
            return Tar::zeroVal();
          //
          REQUIRE (Tar::minVal() <= minResult_);
          REQUIRE (Tar::maxVal() >= maxResult_);
          REQUIRE (minResult_ < maxResult_);
          REQUIRE (0.0 <= probability_);
          REQUIRE (probability_ <= 1.0);
          double q = (1.0 - probability_);
          if (val < q) // control probability of values ≠ neutral
            return Tar::zeroVal();
          if (val > 1.0)
            val = 1.0;
          val -= q;                          // [0 .. [q .. 1[
          val /= probability_;               // [0 .. 1[
          auto org = Tar::zeroVal();
          if  (org == minResult_)
            { // simple standard case
              val *= maxResult_ - org;       // [0 .. m[
              val += org+1;                  // [1 .. m]
              val += CAP_EPSILON;            // round down yet absorb dust
              return Tar{floor (val)};
            }
          else
          if (org < minResult_ or org > maxResult_)
            { // disjoint form origin, but compact
              org = minResult_;              // ensure all values covered
              val *= maxResult_ - org + 1;   // [o .. m]
              val += org;
              val += CAP_EPSILON;
              return Tar{floor (val)};
            }
          else// Origin is somewhere within value range
            {//  ==> wrap "negative" part above max
            //       to map 0.0 ⟼ org (≙neutral)
              val *= maxResult_ - minResult_;
              val += org+1;                  // max inclusive but <0 ⟼ org
              if (val >= maxResult_+1)       // wrap the "negatives"
                val -= maxResult_+1 - minResult_;
              val += CAP_EPSILON;
              return Tar{floor (val)};
            }
        }                             //----headroom to accommodate low probabilities
      static size_t constexpr QUANTISER   = 1 << 4 + util::ilog2 (Tar::maxVal()-Tar::minVal());
      static double constexpr CAP_EPSILON = 1/(2.0 * QUANTISER);
      
      
      /** @internal draw from source of randomness */
      double
      asRand (size_t hash)
        {
          if (shuffle_)
            hash *= shuffle_++;
          return double(hash % QUANTISER) / QUANTISER;
        }
      
      /** @internal core operation: draw and quantise into limited value */
      Tar
      drawLimited (size_t hash)
        {
           return limited (asRand (hash));
        }
      
      
    public:
      /**
       * Drawing is _disabled_ by default, always yielding "zero"
       */
      RandomDraw()
        : Lazy{Disabled()}
        {
          mapping (POL::defaultSrc);
        }
      
      /**
       * Build a RandomDraw by attaching a value-processing function,
       * which is adapted to accept the nominal input type. The effect
       * of the given function is determined by its output value...
       * - `size_t`: the function output is used as source of randomness
       * - `double`: output is directly used as draw value `[0.0..1.0[`
       * - `void(RandomDraw&, ...)` : the function manipulates the current
       *   instance, to control parameters dynamically, based on input.
       */
      template<class FUN,      typename =disable_if_self<FUN, RandomDraw>>
      RandomDraw(FUN&& fun)
        : Lazy{Disabled()}
        , probability_{1.0}
        {
          mapping (forward<FUN> (fun));
        }
      
      
      
      /* ===== Builder API ===== */
      
      RandomDraw&&
      probability (double p)
        {
          probability_ = util::limited (0.0, p ,1.0);
          return move (*this);
        }
      
      RandomDraw&&
      maxVal (Tar m)
        {
          maxResult_ = util::min (m, Tar::maxVal());
          if (minResult_>=maxResult_)
            minVal (--m);
          return move (*this);
        }
      
      RandomDraw&&
      minVal (Tar m)
        {
          minResult_ = util::max (m, Tar::minVal());
          if (maxResult_<=minResult_)
            maxVal (++m);
          return move (*this);
        }
      
      RandomDraw&&
      shuffle (size_t seed =55)
        {
          shuffle_ = seed;
          return move (*this);
        }
      
      template<class FUN>
      RandomDraw&&
      mapping (FUN&& fun)
        {
          Fun& thisMapping = static_cast<Fun&> (*this);
          Lazy::installInitialiser (thisMapping
                                   ,[theFun = forward<FUN> (fun)]
                                    (RandomDraw* self)
                                       { // when lazy init is performed....
                                         self->installAdapted (theFun);
                                       });
          return move (*this);
        }
      
      
      
      
    private:
      /// metafunction: the given function wants to manipulate `*this` dynamically
      template<class SIG>
      struct is_Manipulator
        : std::false_type { };
      
      template<typename...ARGS>
      struct is_Manipulator<void(RandomDraw&, ARGS...)>
        : std::true_type { };
      
      
      
      /** @internal adapt a function and install it to control drawing and mapping */
      template<class FUN>
      void
      installAdapted (FUN&& fun)
        {
          Fun& thisMapping = static_cast<Fun&> (*this);
          thisMapping = adaptOut(adaptIn(std::forward<FUN> (fun)));
        }
      
      
      /** @internal adapt input side of a given function to conform to the
       *            global input arguments as defined in the Policy base function.
       *  @return a function pre-fitted with a suitable Adapter from the Policy */
      template<class FUN>
      auto
      adaptIn (FUN&& fun)
        {
          using lib::meta::func::applyFirst;
          using _Fun = lib::meta::_Fun<FUN>;
          static_assert (_Fun(), "Need something function-like.");
          
          using Sig     = typename _Fun::Sig;
          using Args    = typename _Fun::Args;
          using BaseIn  = typename lib::meta::_Fun<POL>::Args;
          
          if constexpr (std::is_same_v<Args, BaseIn>)
             // function accepts same arguments as this RandomDraw
            return forward<FUN> (fun); // pass-through directly
          else
          if constexpr (is_Manipulator<Sig>())
             // function wants to manipulate *this dynamically...
            return adaptIn (applyFirst (forward<FUN> (fun), *this));
          else
            {// attempt to find a custom adaptor via Policy template
              using Adaptor = typename POL::template Adaptor<Sig>;
              return Adaptor::build (forward<FUN> (fun));
            }
        }
      
      /** @internal adapt output side of a given function, allowing to handle it's results
       *   - a function producing the overall result-type is installed as-is
       *   - a `size_t` result is assumed be a hash and passed into #drawLimited
       *   - likewise a `double` is assumed to be already a random val to be #limited
       *   - special treatment is given to a function with `void` result, which is assumed
       *     to perform some manipulation on this RandomDraw instance by side-effect;
       *     this allows to changes parameters dynamically, based on the input data.
       * @return adapted function which produces a result value of type #Tar
       */
      template<class FUN>
      auto
      adaptOut (FUN&& fun)
        {
          static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
          
          using Res = typename lib::meta::_Fun<FUN>::Ret;
          using lib::meta::func::chained;
          using lib::meta::_FunRet;
          
          if constexpr (std::is_same_v<Res, Tar>)//  ◁──────────────────────────┨ function produces result directly
            return std::forward<FUN>(fun);
          else
          if constexpr (std::is_same_v<Res, size_t>)//  ◁───────────────────────┨ function yields random source to draw value
            return chained (std::forward<FUN>(fun)
                           ,[this](size_t hash){ return drawLimited(hash); }
                           );
          else
          if constexpr (std::is_same_v<Res, double>)//  ◁───────────────────────┨ function yields mapping value to be quantised
            return chained (std::forward<FUN>(fun)
                           ,[this](double rand){ return limited(rand); }
                           );
          else
          if constexpr (std::is_same_v<Res, void>)  // ◁────────────────────────┨ function manipulates parameters by side-effect
            return [functor=std::forward<FUN>(fun)
                   ,processDraw=getCurrMapping()]
                   (auto&& ...inArgs) -> _FunRet<RandomDraw>
                      {
                        functor(inArgs...);     //  invoke manipulator with copy
                        return processDraw (forward<decltype(inArgs)> (inArgs)...);
                      };                      //    forward arguments to mapping-fun
          else
            static_assert (not sizeof(Res), "unable to adapt / handle result type");
          NOTREACHED("Handle based on return type");
        }
      
      
      /** @internal capture the current mapping processing-chain as function.
       *   RandomDraw is-a function to process and map the input argument into a
       *   limited and quantised output value. The actual chain can be re-configured.
       *   This function picks up a snapshot copy of the current configuration; it is
       *   used to build a chain of functions, which incorporates this current function.
       *   If no function was configured yet, the default processing chain is returned.
       */
      Fun
      getCurrMapping()
        {
          Fun& currentProcessingFunction = *this;
          if (currentProcessingFunction)
            return Fun{currentProcessingFunction};
          else
            return Fun{adaptOut(POL::defaultSrc)};
        }
    };
  
  
} // namespace lib
#endif /*LIB_RANDOM_DRAW_H*/
