/*
  ERROR.hpp  -  Lumiera Exception Interface (C++)

  Copyright (C)         Lumiera.org
    2008,2010           Hermann Vosseler <Ichthyostega@web.de>

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


/** @file error.hpp
 ** Lumiera error handling (C++ interface).
 ** This header declares the Lumiera exception hierarchy,
 ** plus some of the most commonly used error flag values.
 ** Within Lumiera, C-style error states and C++-style exceptions
 ** are tightly integrated. Creating an exception sets the error flag,
 ** and there are helpers available to throw an exception automatically
 ** when a unclear error state is detected.
 ** 
 ** @see error-state.c
 ** @see error.hpp
 **
 */


#ifndef LUMIERA_ERROR_HPP_
#define LUMIERA_ERROR_HPP_

#define _STDBOOL_H // prevent <atomic> from including stdbool.h

#include "include/logging.h"
#include "lib/hash-standard.hpp"
#include "lib/error.h"

#include <exception>
#include <string>


namespace lumiera {
  
  using std::string;
  using CStr = const char*;
  
  
#define LERR_(_NAME_) LUMIERA_ERROR_##_NAME_
  
  /** error-ID for unspecified exceptions */
  LUMIERA_ERROR_DECLARE(EXCEPTION);
  
  /**
   * Interface and Base definition for all Lumiera Exceptions.
   * Provides common operations for getting a diagnostic message
   * and to obtain the _root cause_ message, i.e. the message
   * from the first exception encountered in a chain of exceptions.
   */
  class Error
    : public std::exception
    {
      lumiera_err const id_;  ///< an LUMIERA_ERROR id, which is set as errorstate on construction
      string           msg_;  ///< friendly message intended for users (to be localised)
      string          desc_;  ///< detailed description of the error situation for the developers
      mutable string  what_;  ///< buffer for generating the detailed description on demand
      const string   cause_;  ///< description of first exception encountered in the chain
      
      
    public:
      virtual ~Error ()   noexcept { };  ///< this is an interface
      
      
      Error (string description=""
                   ,lumiera_err const id =LERR_(EXCEPTION))  noexcept;
      Error (std::exception const& cause
                   ,string description=""
                   ,lumiera_err const id =LERR_(EXCEPTION))  noexcept;
      
      Error (Error &&)                = default;
      Error (Error const&)            = default;
      Error& operator= (Error &&)     = delete;
      Error& operator= (Error const&) = delete;
      
      /** std::exception interface : yield a diagnostic message */
      virtual CStr
      what ()  const noexcept override;
      
      /** the internal Lumiera-error-ID
       * (was set as C-errorstate in ctor) */
      lumiera_err
      getID ()  const noexcept
        {
          return id_;
        }
      
      /** extract the message to be displayed for the user */
      string const&
      getUsermsg ()  const noexcept
        {
          return msg_;
        }
      
      /** If this exception was caused by a chain of further exceptions,
       *  return the description of the first one registered in this throw sequence.
       *  This works only if every exceptions thrown as a consequence of another exception
       *  is properly constructed by passing the original exception to the constructor
       *  @return the description string, maybe empty (if there is no known root cause)
       */
      string const&
      rootCause ()  const noexcept
        {
          return cause_;
        }
      
      /** replace the previous or default friendly message for the user.
       * @note to be localised / translated.
       */
      Error&
      setUsermsg (string const& newMsg)  noexcept
        {
          msg_ = newMsg;
          return *this;
        }
      
      /** give additional developer info. Typically used at intermediate handlers to add context. */
      Error&
      prependInfo (string const& text)  noexcept
        {
          desc_.insert (0, text);
          return *this;
        }
      
      
    private:
      static const string
      extractCauseMsg (std::exception const&)  noexcept;
    };
  
  
  
  
  
  
  /* === Exception sub-categories === */
  
  namespace error {
    
    /** global function for handling unknown exceptions
     *  encountered at functions declaring not to throw
     *  this kind of exception. Basically, any such event
     *  can be considered a severe design flaw; we can just
     *  add some diagnostics prior to halting.
     */
    void lumiera_unexpectedException ()  noexcept;
    
    /** throw an error::Fatal indicating "assertion failure" */
    void assertion_terminate (const string& location);
    
    
    /* constants to be used as error IDs */
    LUMIERA_ERROR_DECLARE (LOGIC    );    ///< contradiction to internal logic assumptions detected
    LUMIERA_ERROR_DECLARE (FATAL    );    ///< unable to cope with, internal logic floundered
    LUMIERA_ERROR_DECLARE (CONFIG   );    ///< execution aborted due to misconfiguration
    LUMIERA_ERROR_DECLARE (STATE    );    ///< unforeseen internal state
    LUMIERA_ERROR_DECLARE (FLAG     );    ///< non-cleared lumiera errorstate from C code
    LUMIERA_ERROR_DECLARE (INVALID  );    ///< invalid input or parameters encountered
    LUMIERA_ERROR_DECLARE (EXTERNAL );    ///< failure in external service the application relies on
    LUMIERA_ERROR_DECLARE (ASSERTION);    ///< assertion failure
    
    /* generic error situations */
    LUMIERA_ERROR_DECLARE (LIFECYCLE);    ///< Lifecycle assumptions violated
    LUMIERA_ERROR_DECLARE (WRONG_TYPE);   ///< runtime type mismatch
    LUMIERA_ERROR_DECLARE (ITER_EXHAUST); ///< end of sequence reached
    LUMIERA_ERROR_DECLARE (CAPACITY);     ///< predefined fixed storage capacity
    LUMIERA_ERROR_DECLARE (SAFETY_LIMIT); ///< exceeding fixed internal safety limit
    LUMIERA_ERROR_DECLARE (INDEX_BOUNDS); ///< index out of bounds
    LUMIERA_ERROR_DECLARE (BOTTOM_VALUE); ///< invalid or NIL value
    LUMIERA_ERROR_DECLARE (UNCONNECTED);  ///< missing connection
    LUMIERA_ERROR_DECLARE (UNIMPLEMENTED);///< unimplemented feature
    
    
    
    /**
     * Derived specific exceptions within Lumiera's exception hierarchy.
     */
    template<lumiera_err const& eID, class PAR =Error>
    class LumieraError
      : public PAR
      {
      public:
        LumieraError (std::string description=""
                     ,lumiera_err const id=eID)  noexcept
          : PAR{description, id? id:eID}
        { }
        LumieraError (std::exception const& cause
                     ,std::string description=""
                     ,lumiera_err const id=eID)  noexcept
          : PAR{cause, description, id? id:eID}
        { }
      };
    
    //----CLASS-------------------ID--------------PARENT------
    using Logic    = LumieraError<LERR_(LOGIC)>;
    using Fatal    = LumieraError<LERR_(FATAL),   Logic>;
    using State    = LumieraError<LERR_(STATE)>;
    using Flag     = LumieraError<LERR_(FLAG),    State>;
    using Invalid  = LumieraError<LERR_(INVALID)>;
    using Config   = LumieraError<LERR_(CONFIG),  Invalid>;
    using External = LumieraError<LERR_(EXTERNAL)>;
    
    
    /** install our own handler for undeclared exceptions. Will be
     *  called automatically ON_BASIC_INIT when linking exception.cpp */
    void install_unexpectedException_handler();
    
    /** @return error detail-info if currently set, a default message else */
    CStr detailInfo();
    
  } // namespace error
  
  
  
  /**
   * Check the lumiera error state, which maybe was set by C-code.
   * @throw Errorflag exception to signal an detected lumiera error
   * @note specific error code and information is enclosed in
   *       the raised exception; the error state is _not cleared_.
   */
  inline void
  throwOnError()
  {
    if (lumiera_err errorFlag =lumiera_error())
      {
        throw error::Flag( error::detailInfo()
                         , errorFlag);
  }   }
  
  /** Check the lumiera error state and throw a specific exception
   *  in case a non-cleared errorflag is detected. No-op else.
   *  @throw instance of the lumiera::Error subclass provided as
   *         template parameter, containing an lumiera::error::Flag
   *         as root cause to denote the detected error-flag state.
   */
  template<class EX>
  inline void
  maybeThrow (string description ="")
  {
    if (lumiera_err errorFlag =lumiera_error())
      {
        throw EX (error::Flag{error::detailInfo(), errorFlag}
                 ,description);
  }   }
  
  
} // namespace lumiera

/**************************************************//**
 * convenience shortcut for a sequence of catch blocks
 * just logging and consuming an error. Typically
 * this sequence will be used within destructors,
 * which, by convention, must not throw
 */
#define ERROR_LOG_AND_IGNORE(_FLAG_,_OP_DESCR_) \
  catch (std::exception& problem)                \
    {                                             \
      const char* errID = lumiera_error();         \
      WARN (_FLAG_, "%s failed: %s", _OP_DESCR_, problem.what()); \
      TRACE (debugging, "Error flag was: %s", errID);\
    }                                                 \
  catch (...)                                          \
    {                                                   \
      const char* errID = lumiera_error();               \
      ERROR (_FLAG_, "%s failed with unknown exception; " \
                     "error flag is: %s"                   \
                   , _OP_DESCR_, errID?errID:"??");         \
    }

#define ERROR_LOG_AND_RETHROW(_FLAG_,_OP_DESCR_) \
  catch (std::exception& problem)                 \
    {                                              \
      const char* errID = lumiera_error();          \
      WARN (_FLAG_, "%s failed: %s", _OP_DESCR_, problem.what()); \
      TRACE (debugging, "Error flag was: %s", errID); \
      throw;                                           \
    }                                                   \
  catch (...)                                            \
    {                                                     \
      const char* errID = lumiera_error();                 \
      ERROR (_FLAG_, "%s failed with unknown exception; "   \
                     "error flag is: %s"                     \
                   , _OP_DESCR_, errID?errID:"??");           \
      throw;                                                   \
    }

/******************************************************//**
 * convenience shortcut to catch and absorb any exception,
 * then returning a default value instead. This scheme is
 * typically used within signal handlers in the GTK UI,
 * since GTK (written in C) can not propagate exceptions
 */
#define ON_EXCEPTION_RETURN(_VAL_,_OP_DESCR_)   \
  catch (std::exception& problem)                \
    {                                             \
      const char* errID = lumiera_error();         \
      WARN (stage, "%s (Handler) failed: %s",       \
                   _OP_DESCR_, problem.what());      \
      TRACE (debugging, "Error flag was: %s", errID); \
      return (_VAL_);                                  \
    }                                                   \
  catch (...)                                            \
    {                                                     \
      const char* errID = lumiera_error();                 \
      ERROR (stage, "(Handler) %s failed with "             \
                    "unknown exception; error flag is: %s"   \
                   , _OP_DESCR_, errID?errID:"??");           \
      return (_VAL_);                                          \
    }



/**************************************************//**
 * if NoBug is used, redefine some macros
 * to rather throw Lumiera Errors instead of aborting
 */
#if 0 ///////////////////////////////////TODO disabled for now. NoBug aborts are hard and may hold some locks. There are hooks to allow throwing from NoBug  TODO use them....
#ifdef NOBUG_ABORT
#undef NOBUG_ABORT
#define LUMIERA_NOBUG_LOCATION \
  std::string (NOBUG_BASENAME(__FILE__)) +":"+ NOBUG_STRINGIZE(__LINE__) + ", function " + __func__
#define NOBUG_ABORT \
  lumiera::error::assertion_terminate (LUMIERA_NOBUG_LOCATION);
#endif
#endif

#endif // LUMIERA_ERROR_HPP_
