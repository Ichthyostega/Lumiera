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
 ** Intermediary value object to represent the result of an operation.
 ** This operation might have produced a value result or failed with an exception.
 ** Typically, the Result token is used _inline_ -- immediately either invoking
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

#include <utility>
#include <string>



namespace lib {
  
  using util::isnil;
  using std::string;
  namespace error = lumiera::error;
  
  
  
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
    {
      string failureLog_;
      wrapper::ItemWrapper<RES> value_;
      
    public:
      /** mark an invalid/failed result */
      Result (string const& failureReason ="no result")
        : failureLog_{failureReason}
        { }
      
      /** failed result, with reason given.*/
      Result (lumiera::Error const& reason)
       : failureLog_{reason.what()}
       { }
      
      /** standard case: valid result */
      Result (RES&& value)
       : failureLog_{}
       , value_{std::forward<RES> (value)}
       { }
      
      
      
      bool
      isValid()  const
        {
          return value_.isValid();
        }
      
      void
      maybeThrow()  const
        {
          if (not isValid())
            throw error::State (failureLog_, lumiera_error_peek());
        }
      
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
    };
  
  
  /**
   * Specialisation for signalling success or failure,
   * without returning any value result.
   */
  template<>
  class Result<void>
    {
      string failureLog_;
      
    public:
      /** mark either failure (default) or success */
      Result (bool success =false)
        : failureLog_{success? "": "operation failed"}
        { }
      
      /** failed result, with reason given.*/
      Result (lumiera::Error const& reason)
       : failureLog_{reason.what()}
       { }
      
      
      
      bool
      isValid()  const
        {
          return isnil (failureLog_);
        }
      
      void
      maybeThrow()  const
        {
          if (not isValid())
            throw error::State (failureLog_, lumiera_error_peek());
        }
    };
  
  
  
  
} // namespace lib
#endif
