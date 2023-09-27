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
 ** 
 ** @see vault::ThreadJoinable usage example
 */



#ifndef LIB_RESULT_H
#define LIB_RESULT_H

#include "lib/error.hpp"
#include "lib/wrapper.hpp"
#include "lib/util.hpp"

#include <exception>
#include <utility>



namespace lib {
  
  using util::isnil;
  namespace error = lumiera::error;

  
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
      Result (RES&& value)
       : Result<void>{true}
       , value_{std::forward<RES> (value)}
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
      
      template<typename MAKE, typename...ARGS>
      RES
      getOrElse (MAKE&& producer, ARGS ...args)
        {
          if (isValid())
            return *value_;
          else
            return std::invoke(std::forward<MAKE> (producer), std::forward<ARGS> (args)...);
        }
    };
  
  /** deduction guard: allow _perfect forwarding_ of a any result into the ctor call. */
  template<typename VAL>
  Result (VAL&&) -> Result<VAL>;
  
  
  
} // namespace lib
#endif
