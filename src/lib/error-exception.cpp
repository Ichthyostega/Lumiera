/*
  ERROR-EXCEPTION  -  Lumiera exception classes

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file error-exception.cpp
 ** implementation of C++-style error handling.
 ** This compilation unit defines a custom exception class hierarchy,
 ** and is tightly integrated with the C-style error handling.
 ** 
 ** @see error-state.c
 ** @see error.hpp
 **
 */


#include "lib/error.hpp"
#include "include/lifecycle.h"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <exception>
#include <typeinfo>
#include <iostream>

using util::cStr;
using util::isnil;
using std::exception;


namespace lumiera {
  namespace error {
    
    /** the message shown to the user per default
     *  if an exception reaches one of the top-level
     *  catch clauses.
     *  @todo to be localised
     *  @todo develop a framework to set more specific yet friendly messages
     */
    inline const string
    default_usermsg (Error*)  noexcept
    {
      return "Sorry, Lumiera encountered an internal error.";
    }
    
    CStr
    detailInfo ()
    {
      CStr detailinfo = lumiera_error_extra();
      return isnil (detailinfo)? "Lumiera errorstate detected"
                               : detailinfo;
    }
    
    
    /* constants to be used as error IDs */
    LUMIERA_ERROR_DEFINE (LOGIC    , "internal logic broken");
    LUMIERA_ERROR_DEFINE (FATAL    , "floundered");
    LUMIERA_ERROR_DEFINE (CONFIG   , "misconfiguration");
    LUMIERA_ERROR_DEFINE (STATE    , "unforeseen state");
    LUMIERA_ERROR_DEFINE (FLAG     , "non-cleared lumiera errorstate");
    LUMIERA_ERROR_DEFINE (INVALID  , "invalid input or parameters");
    LUMIERA_ERROR_DEFINE (EXTERNAL , "failure in external service");
    LUMIERA_ERROR_DEFINE (ASSERTION, "assertion failure");
    
    /* some further generic error situations */
    LUMIERA_ERROR_DEFINE (LIFECYCLE,    "Lifecycle assumptions violated");
    LUMIERA_ERROR_DEFINE (WRONG_TYPE,   "runtime type mismatch");
    LUMIERA_ERROR_DEFINE (ITER_EXHAUST, "end of sequence reached");
    LUMIERA_ERROR_DEFINE (CAPACITY,     "predefined fixed storage capacity");
    LUMIERA_ERROR_DEFINE (INDEX_BOUNDS, "index out of bounds");
    LUMIERA_ERROR_DEFINE (BOTTOM_VALUE, "invalid or NIL value");
    LUMIERA_ERROR_DEFINE (UNCONNECTED,  "missing connection");
    LUMIERA_ERROR_DEFINE (UNIMPLEMENTED,"using a feature not yet implemented....");
    
  } // namespace error
  
  LUMIERA_ERROR_DEFINE (EXCEPTION, "generic Lumiera exception");
  
  
  
  
  Error::Error (string description, lumiera_err const id)  noexcept
    : std::exception{}
    , id_{id}
    , msg_{error::default_usermsg (this)}
    , desc_{description}
    , cause_{}
    { }
  
  
  Error::Error (std::exception const& cause, 
                string description, lumiera_err const id)  noexcept
    : std::exception{}
    , id_{id}
    , msg_{error::default_usermsg (this)}
    , desc_{description}
    , cause_{extractCauseMsg(cause)}
    {
      string detailInfo{description + (isnil(cause_)? "" : " | cause = "+cause_)};
    }
  
  
  
  
  /** Description of the problem, including the internal char constant
   *  in accordance to Lumiera's error identification scheme.
   *  If a root cause can be obtained, this will be included in the
   *  generated output as well. 
   */
  CStr
  Error::what()  const noexcept
  {
    if (isnil (this->what_))
      {
        what_ = string(id_);
        if (!isnil (desc_))  what_ += " ("+desc_+").";
        if (!isnil (cause_)) what_ += string(" -- caused by: ") + cause_;
      }
    return what_.c_str(); 
  }
  
  
  /** @internal get at the description message of the 
   *  first exception encountered in a chain of exceptions
   */
  const string
  Error::extractCauseMsg (const exception& cause)  noexcept
  {
    const Error* err = dynamic_cast<const Error*> (&cause);
    if (err)
      {
        if (isnil (err->cause_))
          return cause.what(); // cause is root cause
        else
          return err->cause_; // cause was caused by another exception
      }
    // unknown other exception type
    return cause.what ();
  }
  
  
  
  
  
  
  
  namespace error {
    namespace {
      void install_unexpectedException_handler ()
      {
        std::set_terminate (lumiera_unexpectedException);
      }
      LifecycleHook schedule_ (ON_BASIC_INIT, &install_unexpectedException_handler);
      
      std::terminate_handler nextHandler = nullptr;
    }

    
    void lumiera_unexpectedException ()  noexcept
    {
      CStr is_halted
        = "### Lumiera halted due to an unexpected Error ###";
      
      ERROR (NOBUG_ON, "%s", is_halted);
      std::cerr << "\n" << is_halted << "\n\n";
      
      
      try { // -----find-out-about-any-Exceptions--------
          auto lastException = std::current_exception();
          if (lastException) {
              std::rethrow_exception (lastException);
          }
      } catch(const lumiera::Error& lerr) {
          std::cout << "\n+++ Caught Exception " << lerr.getID() << "\n\n";
          ERROR (NOBUG_ON, "+++ caught %s\n+++ messg: %s\n+++ descr: %s"
                         , cStr(util::typeStr(lerr))
                         , cStr(lerr.getUsermsg())
                         , cStr(lerr.what())
                         );
          if (not isnil(lerr.rootCause()))
            ERROR (NOBUG_ON, "+++ cause: %s",cStr(lerr.rootCause()));
          
      } catch(const std::exception& e) {
          ERROR (NOBUG_ON, "Generic Exception: %s", e.what());
          std::cout << "+++ Caught Exception \"" << e.what() << "\"\n";
      } catch(...) {
          ERROR (NOBUG_ON, "FATAL -- unknown exception");
      }
      
      if (CStr errorstate = lumiera_error ())
        ERROR (NOBUG_ON, "last registered error was....\n%s", errorstate);
      
      if (nextHandler)
        nextHandler();
      else
        std::abort();
    }
    
    
    void assertion_terminate (const string& location)
    {
      throw Fatal (location, LUMIERA_ERROR_ASSERTION)
                 .setUsermsg("Program terminated because of violating "
                             "an internal consistency check.");
    }
    
  } // namespace error

} // namespace lumiera
