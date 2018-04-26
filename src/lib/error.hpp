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
 ** when a non-cleare error state is detected.
 ** 
 ** @see error-state.c
 ** @see error.hpp
 **
 */


#ifndef LUMIERA_ERROR_HPP_
#define LUMIERA_ERROR_HPP_

#include "include/logging.h"
#include "include/lifecycle.h"
#include "lib/error.h"
#include <exception>
#include <string>

namespace lumiera {
  
  using std::string;
  
  
  
  /** error-ID for unspecified exceptions */
  LUMIERA_ERROR_DECLARE(EXCEPTION);  
  
  /**
   * Interface and Base class of all Exceptions thrown
   * from within Lumiera (C++) code. Common operations
   * for getting an diagnostic message and for obtaining
   * the root cause, i.e. the first exception encountered
   * in a chain of exceptions.
   */
  class Error : public std::exception
    {
    public:
      Error (string description="", const char* const id=LUMIERA_ERROR_EXCEPTION) noexcept;
      Error (std::exception const& cause,
             string description="", const char* const id=LUMIERA_ERROR_EXCEPTION) noexcept;
      
      Error (const Error&) noexcept;
      virtual ~Error () noexcept {};
      
      /** yield a diagnostic message characterising the problem */
      virtual const char* what () const noexcept;
      
      /** the internal Lumiera-error-ID (was set as C-errorstate in ctor) */
      const char* getID () const noexcept { return this->id_; }
      
      /** extract the message to be displayed for the user */
      const string& getUsermsg () const noexcept;
      
      /** If this exception was caused by a chain of further exceptions,
       *  return the description of the first one registered in this throw sequence.
       *  This works only if every exceptions thrown as a consequence of another exception 
       *  is properly constructed by passing the original exception to the constructor
       *  @return the description string, maybe empty (if there is no known root cause)
       */
      const string& rootCause () const noexcept  { return this->cause_; }
      
      /** replace the previous or default friendly message for the user. To be localised. */
      Error& setUsermsg (const string& newMsg) noexcept { this->msg_ = newMsg; return *this; }
      
      /** give additional developer info. Typically used at intermediate handlers to add context. */
      Error& prependInfo (const string& text) noexcept { this->desc_.insert (0,text); return *this; }
      
      
    private:
      const char* id_;       ///< an LUMIERA_ERROR id, which is set as errorstate on construction
      string msg_;           ///< friendly message intended for users (to be localised)
      string desc_;          ///< detailed description of the error situation for the developers
      mutable string what_;  ///< buffer for generating the detailed description on demand
      const string cause_;   ///< description of first exception encountered in the chain
      
      static const string extractCauseMsg (std::exception const&)  noexcept;
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
    LUMIERA_ERROR_DECLARE (INDEX_BOUNDS); ///< index out of bounds
    LUMIERA_ERROR_DECLARE (BOTTOM_VALUE); ///< invalid or NIL value
    LUMIERA_ERROR_DECLARE (UNCONNECTED);  ///< missing connection
    LUMIERA_ERROR_DECLARE (UNIMPLEMENTED);///< unimplemented feature
    
    
    
/** Macro for creating derived exception classes properly
 *  integrated into Lumiera's exception hierarchy. Using
 *  this macro assures that the new class will get the full
 *  set of constructors and behaviour common to all exception
 *  classes, so it should be used when creating an derived
 *  exception type for more then strictly local purposes
 */
#define LUMIERA_EXCEPTION_DECLARE(CLASS, PARENT, _ID_) \
    class CLASS : public PARENT                         \
      {                                                 \
      public:                                           \
        CLASS (std::string description="",              \
               const char* id=_ID_) noexcept            \
        : PARENT (description, id)  {}                  \
                                                        \
        CLASS (std::exception const& cause,             \
               std::string description="",              \
               const char* id=_ID_) noexcept            \
        : PARENT (cause, description, id)   {}          \
      };
    
    //-------------------------CLASS-----PARENT--ID----------------------
    LUMIERA_EXCEPTION_DECLARE (Logic,    Error,  LUMIERA_ERROR_LOGIC);
    LUMIERA_EXCEPTION_DECLARE (Fatal,    Logic,  LUMIERA_ERROR_FATAL);
    LUMIERA_EXCEPTION_DECLARE (Config,   Error,  LUMIERA_ERROR_CONFIG);
    LUMIERA_EXCEPTION_DECLARE (State,    Error,  LUMIERA_ERROR_STATE);
    LUMIERA_EXCEPTION_DECLARE (Flag,     State,  LUMIERA_ERROR_FLAG);
    LUMIERA_EXCEPTION_DECLARE (Invalid,  Error,  LUMIERA_ERROR_INVALID);
    LUMIERA_EXCEPTION_DECLARE (External, Error,  LUMIERA_ERROR_EXTERNAL);
    
    
    /** install our own handler for undeclared exceptions. Will be
     *  called automatically ON_BASIC_INIT when linking exception.cpp */
    void install_unexpectedException_handler ();
    
    /** @return error detail-info if currently set, a default message else */
    const char* detailInfo ();
    
  } // namespace error
  
  
  
  /**
   * Check the lumiera error state, which maybe was set by C-code.
   * @throw Errorflag exception to signal an detected lumiera error
   * @note specific error code and information is enclosed in
   *       the raised exception; the error state is \em not cleared.
   */
  inline void
  throwOnError()
  {
    if (lumiera_err errorFlag =lumiera_error())
      {
        throw error::Flag( error::detailInfo()
                         , errorFlag);
  }   }                  //causes the error state to be set
  
  /** Check the lumiera error state and throw a specific exception
   *  in case a non-cleared errorflag is detected. No-op else.
   *  @throw instance of the lumiera::Error subclass provided as
   *         template parameter, containing an lumiera::error::Flag
   *         as root cause to denote the detected error-flag state.
   */
  template<class EX>
  inline void
  maybeThrow(string description)
  {
    if (lumiera_err errorFlag =lumiera_error())
      {
        throw EX (error::Flag( error::detailInfo()
                             , errorFlag)
                 ,description);
  }   }
  
  template<class EX>
  inline void
  maybeThrow()
  {
    maybeThrow<EX>("");
  }
  
  
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
                   , _OP_DESCR_, errID);                    \
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
