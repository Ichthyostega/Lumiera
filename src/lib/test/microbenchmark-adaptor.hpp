/*
  MICROBENCHMARK-ADAPTOR.hpp  -  helper to support microbenchmarks

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


/** @file microbenchmark-adaptor.hpp
 ** Helpers and wrappers so simplify usage of \ref micobenchmark.hpp.
 ** Notably the benchmark functions expect the actual »test subject« as a
 ** function or lambda with signature `size_t(size_t)`. The argument will be
 ** the loop index and the result value will be added into a checksum, which
 ** also ensures that the optimiser can not unroll the benchmark loop.
 ** However, in practical use this strict requirement for the signature
 ** turned out as a nuisance; this header provides some automatic adaption
 ** - accept any numeric type as argument
 ** - accept any numeric type as checksum contribution (cast to `size_t`)
 ** - accept signature `void(void)`
 **
 */


#ifndef LIB_TEST_MICROBENCHMARK_ADAPTOR_H
#define LIB_TEST_MICROBENCHMARK_ADAPTOR_H


#include "lib/meta/function.hpp"
#include "lib/meta/util.hpp"

namespace lib {
namespace test{
namespace microbenchmark {
  
  using lib::meta::enable_if;
  using std::is_arithmetic;
  using std::is_same;
  using std::__and_;
  using std::__not_;
  
  
  /**
   * @internal helper to expose the signature `size_t(size_t)`
   *           by wrapping a given lambda or functor.
   */
  template<class SIG, typename SEL=void>
  struct Adaptor
    {
      static_assert (not sizeof(SIG), "Unable to adapt given functor.");
    };
  
  template<>
  struct Adaptor<size_t(size_t)>
    {
      template<typename FUN>
      static decltype(auto)
      wrap (FUN&& fun)
        {
          return std::forward<FUN>(fun);
        }
    };
  
  template<>
  struct Adaptor<void(void)>
    {
      template<typename FUN>
      static auto
      wrap (FUN&& fun)
        {
          return [functor=std::forward<FUN>(fun)]
                 (size_t)
                    {
                      functor();
                      return size_t(1);
                    };
        }
    };
  
  template<typename ON, typename IN>
  struct Adaptor<ON(IN),               enable_if<__and_<is_arithmetic<IN>, __not_<is_same<IN,size_t>>
                                                       ,is_arithmetic<ON>, __not_<is_same<ON,size_t>>
                                                       >>>
    {
      template<typename FUN>
      static auto
      wrap (FUN&& fun)
        {
          return [functor=std::forward<FUN>(fun)]
                 (size_t i)
                    {
                      return size_t(functor(i));
                    };
        }
    };
  
  template<typename ON>
  struct Adaptor<ON(void),             enable_if<__and_<is_arithmetic<ON>, __not_<is_same<ON,size_t>>
                                                       >>>
    {
      template<typename FUN>
      static auto
      wrap (FUN&& fun)
        {
          return [functor=std::forward<FUN>(fun)]
                 (size_t)
                    {
                      return size_t(functor());
                    };
        }
    };
  
  template<typename IN>
  struct Adaptor<void(IN),             enable_if<__and_<is_arithmetic<IN>, __not_<is_same<IN,size_t>>
                                                       >>>
    {
      template<typename FUN>
      static auto
      wrap (FUN&& fun)
        {
          return [functor=std::forward<FUN>(fun)]
                 (size_t i)
                    {
                      functor(i);
                      return size_t(1);
                    };
        }
    };
  
  
  
  /**
   * Adapter to expose the signature `size_t(size_t)`
   * from any suitable source functor or lambda
   * @note requirements
   *     - arity must be either zero or one argument
   *     - if argument or return type are present,
   *       they must be plain arithmetic types
   *     - no references allowed
   *     - can be `void(void)`
   */
  template<typename FUN>
  inline decltype(auto)
  adapted4benchmark (FUN&& fun)
  {
    static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
    static_assert (lib::meta::_Fun<FUN>::ARITY <=1, "Function with zero or one argument required.");
    
    using Sig = typename lib::meta::_Fun<FUN>::Sig;
    
    return Adaptor<Sig>::wrap (std::forward<FUN> (fun));
  }
  
  
  
}}} // namespace lib::test::microbenchmark
#endif /*LIB_TEST_MICROBENCHMARK_ADAPTOR_H*/
