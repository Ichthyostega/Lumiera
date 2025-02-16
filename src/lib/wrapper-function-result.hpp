/*
  WRAPPER-FUNCTION-RESULT.hpp  -  some smart wrapping and reference managing helper templates

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file wrapper-function-result.hpp
 ** Helper to cache the result of function invocation.
 ** @todo 2025 initially created by direct need, it was used for a while,
 **       but became largely obsoleted by the ''transforming'' functionality
 **       provided by iter-explorer.hpp (which is implemented by the same
 **       basic technique, but without the std::function baseclass).
 */


#ifndef LIB_WRAPPER_FUNCTION_RESULT_H
#define LIB_WRAPPER_FUNCTION_RESULT_H

#include "lib/wrapper.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/util.hpp"

#include <functional>


namespace lib {
namespace wrapper {
  
  using lib::meta::_Fun;
  using std::function;
  
  
  
  /**
   * Extension of ItemWrapper: a function remembering the result of the
   * last invocation. Initially, the "value" is bottom (undefined, NIL),
   * until the function is invoked for the first time. After that, the
   * result of the last invocation can be accessed by `operator* ()`
   * @note deliberately non-copyable, since we capture a reference
   *       to `this` in order to write to the embedded ItemWrapper.
   *       (to alleviate, we'd have to re-link after copying/moving)
   */
  template<typename SIG>
  struct FunctionResult
    : public function<SIG>
    , util::NonCopyable
    {
      using Res = typename _Fun<SIG>::Ret;
      using ResWrapper = ItemWrapper<Res>;
      
      ResWrapper lastResult_;
      
    public:
      /** by default locked to _invalid state_  */
      FunctionResult()  = default;
      
      /**
       * Create result-remembering functor by outfitting a _copy_
       * of the given function with an adaptor to _capture_ each
       * produced result.
       * @warning if function result is a value, it is copied.
       */
      template<typename FUN>
      FunctionResult (FUN&& targetFunction)
        : function<SIG>{lib::meta::func::chained
                            ( std::forward<FUN> (targetFunction)
                            , [this](Res res) -> Res
                                    {
                                      lastResult_ = res;
                                      return std::forward<Res> (res);
                                    })}
        { }
      
      /** retrieve the last function result observed */
      Res& operator*() const { return *lastResult_; }
      bool isValid ()  const { return lastResult_.isValid(); }
     
      explicit
      operator bool()  const { return isValid(); }
    };
  
  
}} // namespace lib::wrap
#endif /*LIB_WRAPPER_FUNCTION_RESULT_H*/
