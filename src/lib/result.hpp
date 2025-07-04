/*
  RESULT.hpp  -  intermediary token representing the result of an operation

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file result.hpp
 ** Intermediary value object to represent »either« an operation result or a failure.
 ** Some operation might have produced a value result or failed with an exception.
 ** Typically, the Result token is used _inline_ — immediately either invoking
 ** one of the member function or employing the built-in result type conversion.
 ** It will be copyable iff the result value is copyable. There is an implicit
 ** valid or failure state, which can be tested. Any attempt to get the value
 ** of an invalid result token will cause an exception to be thrown.
 ** - `Result<void>(bool)` can be used as a success marker
 ** - a `Result` instance can be created by _perfect forwarding_ from any type
 ** - any exception is supported for failure, wile direct construction is limited
 **   to lumiera::Error (to avoid ambiguities in ctor overload resolution)
 ** - an arbitrary functor or _callable_ can be invoked, capturing the result.
 ** @todo an _option-style_ interface could be provided for the »right value«
 **       (i.e. the exception caught), in case this turns out to be of any use;
 **       this kind of API design however is anything than trivial, given that
 **       any value can be thrown as exception in C++
 ** @see vault::ThreadJoinable usage example
 ** @see Result_test
 */



#ifndef LIB_RESULT_H
#define LIB_RESULT_H

#include "lib/error.hpp"
#include "lib/item-wrapper.hpp"
#include "lib/meta/util.hpp"

#include <type_traits>
#include <exception>
#include <utility>



namespace lib {
  
  namespace error = lumiera::error;
  
  
  /**
   * Representation of the result of some operation, _EITHER_ a value or a failure.
   * It can be created for passing a result produced by the operation, or the failure
   * to do so. The value can be retrieved by implicit or explicit conversion.
   * @tparam RES the nominal result type to be captured from the function invocation.
   * @throw error::State on any attempt to access the value in case of failure
   * @warning this class has a lot of implicit conversions;
   *          care should be taken when defining functions
   *          to take Result instances as parameter....
   */
  template<typename RES>
  class Result;
  
  
  /**
   * The base case is just to capture success or failure,
   * without returning any value result.
   */
  template<>
  class Result<void>
    {
    protected:
      std::exception_ptr failure_;
      
    public:
      /** mark either failure (default) or success */
      Result (bool success =false)
        : failure_{success? nullptr: std::make_exception_ptr (error::State{"operation failed"})}
        { }
      
      /** failed result, with reason given.*/
      Result (lumiera::Error const& reason)
        : failure_{std::make_exception_ptr (reason)}
        { }
       
      /** invoke a _callable_ and mark success or failure */
      template<class FUN, typename...ARGS,        typename=lib::meta::enable_if<std::is_invocable<FUN,ARGS...>>>
      Result (FUN&& callable, ARGS&& ...args)     noexcept
        : failure_{}
        {
          try {
              static_assert (std::is_invocable_v<FUN,ARGS...>);
              std::invoke (std::forward<FUN>(callable)
                          ,std::forward<ARGS>(args)...
                          );
            }
          catch(...)
            {
              failure_ = std::current_exception();
            }
        }
      
      explicit
      operator bool() const { return isValid(); }
      bool isValid()  const { return not failure_; }
      
      void
      maybeThrow()  const
        {
          if (failure_)
            std::rethrow_exception(failure_);
        }
    };
  
  
  /**
   * Optional Result value or status of some operation.
   * It can be created for passing a result produced by the operation, or the
   * failure to do so. The value can be retrieved by implicit or explicit conversion.
   * @throw error::State on any attempt to access the value in case of failure
   * @warning this class has a lot of implicit conversions;
   *          care should be taken when defining functions
   *          to take Result instances as parameter....
   */
  template<typename RES>
  class Result
    : public Result<void>
    {
      wrapper::ItemWrapper<RES> value_;
      
    public:
      /** mark failed result, with reason given.*/
      Result (lumiera::Error const& reason)
       : Result<void>{reason}
       { }
      
      /** standard case: valid result */
      template<                                     typename=lib::meta::disable_if<std::is_invocable<RES>>>
      Result (RES&& value)
       : Result<void>{true}
       , value_{std::forward<RES> (value)}
       { }
      
      /** invoke a _callable_ and capture result in one shot */
      template<class FUN, typename...ARGS,          typename=lib::meta::enable_if<std::is_invocable<FUN,ARGS...>>>
      Result (FUN&& callable, ARGS&& ...args)       noexcept
        : Result<void>{true}
        , value_{}
        {
          try {
              static_assert (std::is_invocable_r_v<RES,FUN,ARGS...>);
              value_ = std::invoke_r<RES> (std::forward<FUN>(callable)
                                          ,std::forward<ARGS>(args)...
                                          );
            }
          catch(...)
            {
              failure_ = std::current_exception();
            }
        }
      
      // is or is not copyable depending on RES
      
      
      operator RES()  const
        {
          maybeThrow();
          return *value_;
        }
      
      template<typename TY =RES>
      TY
      get()  const
        {
          maybeThrow();
          return static_cast<TY> (*value_);
        }
      
      template<typename O>
      RES
      value_or (O&& defaultVal)
        {
          return isValid()? *value_ : std::forward<O> (defaultVal);
        }
      
      template<typename MAKE, typename...ARGS>
      RES
      or_else (MAKE&& producer, ARGS ...args)
        {
          if (isValid())
            return *value_;
          else
            return std::invoke(std::forward<MAKE> (producer), std::forward<ARGS> (args)...);
        }
    };
  
  /** deduction guide: allow _perfect forwarding_ of a any result into the ctor call. */
  template<typename VAL,                             typename=lib::meta::disable_if<std::is_invocable<VAL>>>
  Result (VAL&&) -> Result<VAL>;

  /** deduction guide: find out about result value to capture from a generic callable. */
  template<typename FUN, typename...ARGS>
  Result (FUN&&, ARGS&&...) -> Result<std::invoke_result_t<FUN,ARGS...>>;
  
  
  
} // namespace lib
#endif
