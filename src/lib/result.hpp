/*
  RESULT.hpp  -  intermediary token representing the result of an operation

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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
 */



#ifndef LIB_RESULT_H
#define LIB_RESULT_H

#include "lib/error.hpp"
#include "lib/wrapper.hpp"
#include "lib/meta/util.hpp"
#include "lib/null-value.hpp"

#include <type_traits>
#include <exception>
#include <utility>



namespace lib {
  
  namespace error = lumiera::error;

  /**
   * Helper to invoke an arbitrary callable in a failsafe way.
   * @param capturedFailure *reference* to a std::exeption_ptr served by side-effect
   * @param callable anything std::invoke can handle
   * @return _if_ the invokable has a return type, the result is returned,
   *         _otherwise_ this is a void function
   * @todo with C++20 the body of the implementation can be replaced by std::invoke_r  //////////////////////TICKET #1245
   */
  template<class FUN, typename...ARGS>
  inline auto
  failsafeInvoke (std::exception_ptr& capturedFailure
                 ,FUN&& callable
                 ,ARGS&& ...args)
  {
    using Res = std::invoke_result_t<FUN,ARGS...>;
    try {
        capturedFailure = nullptr;
        if constexpr (std::is_void_v<Res>)
          std::invoke (std::forward<FUN>(callable), std::forward<ARGS>(args)...);
        else
          return std::invoke (std::forward<FUN>(callable), std::forward<ARGS>(args)...);
      }
    catch(...)
      {
        capturedFailure = std::current_exception();
        if constexpr (not std::is_void_v<Res>)
          return lib::NullValue<Res>::get();
      }
  }
  
  
  
  /**
   * Representation of the result of some operation, _EITHER_ a value or a failure.
   * It can be created for passing a result produced by the operation, or the failure
   * to do so. The value can be retrieved by implicit or explicit conversion.
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
      Result (FUN&& callable, ARGS&& ...args)
        : failure_{}
        {
          failsafeInvoke (failure_
                         ,std::forward<FUN> (callable)
                         ,std::forward<ARGS>(args)...);
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
      Result (FUN&& callable, ARGS&& ...args)
        : Result<void>{true}
        , value_{failsafeInvoke (failure_
                                ,std::forward<FUN> (callable)
                                ,std::forward<ARGS>(args)...)}
        { }
      
      // is or is not copyable depending on RES
      
      
      operator RES()  const
        {
          maybeThrow();
          return *value_;
        }
      
      template<typename TY>
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
  
  /** deduction guard: allow _perfect forwarding_ of a any result into the ctor call. */
  template<typename VAL,                             typename=lib::meta::disable_if<std::is_invocable<VAL>>>
  Result (VAL&&) -> Result<VAL>;

  /** deduction guard: find out about result value to capture from a generic callable. */
  template<typename FUN, typename...ARGS>
  Result (FUN&&, ARGS&&...) -> Result<std::invoke_result_t<FUN,ARGS...>>;

  
  
} // namespace lib
#endif
