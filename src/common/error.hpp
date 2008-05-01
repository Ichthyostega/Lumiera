/*
  ERROR.hpp  -  Lumiera Exception Interface
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef LUMIERA_ERROR_HPP_
#define LUMIERA_ERROR_HPP_

#include <string>
#include "proc/nobugcfg.hpp"
#include "lib/error.h"


namespace lumiera
  {
  using std::string;


  
  /** error-ID for unspecified exceptions */
  LUMIERA_ERROR_DECLARE(EXCEPTION);  
  
  /**
   * Interface and Baseclass of all Exceptions thrown 
   * from within Lumiera (C++) code. Common operations
   * for getting an diagnostic message and for obtaining
   * the root cause, i.e. the first exception encountered
   * in a chain of exceptions. 
   */
  class Error : public std::exception
    {
    public:
      Error (string description="", const char* id=LUMIERA_ERROR_EXCEPTION) throw();
      Error (std::exception& cause,
             string description="", const char* id=LUMIERA_ERROR_EXCEPTION) throw();
      
      Error (const Error&) throw();
      virtual ~Error () throw() {};
      
      /** yield a diagnostic message characterizing the problem */
      virtual const char* what () const throw();

      /** the internal Lumiera-error-ID (was set as C-errorstate in ctor) */
      const char* getID () const throw() { return this->id_; }
      
      /** extract the message to be displayed for the user */
      const string& getUsermsg () const throw();
      
      /** If this exception was caused by a chain of further exceptions,
       *  return the description of the first one registered in this throw sequence.
       *  This works only if every exceptions thrown as a consequence of another exception 
       *  is propperly constructed by passing the original exception to the constructor
       *  @return the description string, maybe empty (if there is no known root cause)
       */
      const string& rootCause () const throw()  { return this->cause_; }

      /** replace the previous or default friendly message for the user. To be localized. */
      Error& setUsermsg (const string& newMsg) throw() { this->msg_ = newMsg; return *this; }

      /** give additional developer info. Typically used at intermediate handlers to add context. */
      Error& prependInfo (const string& text) throw() { this->desc_.insert (0,text); return *this; }
      
      
    private:
      const char* id_;       ///< an LUMIERA_ERROR id, which is set as errorstate on construction
      string msg_;           ///< friendly message intended for users (to be localized)
      string desc_;          ///< detailed description of the error situation for the developers
      mutable string what_;  ///< buffer for generating the detailed description on demand
      const string cause_;   ///< descriptoin of first exception encountered in the chain

      static const string extractCauseMsg (const std::exception&)  throw();
    };
  
    
    
    
    
    
  /* === Exception Subcategories === */
    
  namespace error
    {
    
    /** global function for handling unknown exceptions
     *  encountered at functions declaring not to throw 
     *  this kind of exception. Basically, any such event
     *  can be considered a severe design flaw; we can just
     *  add some diagnostics prior to halting.
     */
    void lumiera_unexpectedException ()  throw();

    /** throw an error::Fatal indicating "assertion failure" */
    void assertion_terminate (const string& location);
    

    /* constants to be used as error IDs */
    LUMIERA_ERROR_DECLARE (LOGIC    );    ///< contradiction to internal logic assumptions detected  
    LUMIERA_ERROR_DECLARE (FATAL    );    ///< unable to cope with, internal logic floundered     
    LUMIERA_ERROR_DECLARE (CONFIG   );    ///< execution aborted due to misconfiguration
    LUMIERA_ERROR_DECLARE (STATE    );    ///< unforeseen internal state
    LUMIERA_ERROR_DECLARE (INVALID  );    ///< invalid input or parameters encountered
    LUMIERA_ERROR_DECLARE (EXTERNAL );    ///< failure in external service the application relies on
    LUMIERA_ERROR_DECLARE (ASSERTION);    ///< assertion failure
    
/** Macro for creating derived exception classes properly 
 *  integrated into Lumiera's exception hierarchy. Using
 *  this macro asures that the new class will get the full
 *  set of constructors and behaviour common to all exception
 *  classes, so it should be used when creating an derived
 *  exception type for more then stricly local purposes
 */
#define LUMIERA_EXCEPTION_DECLARE(CLASS, PARENT, _ID_) \
    class CLASS : public PARENT                         \
      {                                                 \
      public:                                           \
        CLASS (std::string description="",              \
               const char* id=_ID_) throw()             \
        : PARENT (description, id)  {}                  \
                                                        \
        CLASS (std::exception& cause,                   \
               std::string description="",              \
               const char* id=_ID_) throw()             \
        : PARENT (cause, description, id)   {}          \
      };

    //-------------------------CLASS-----PARENT--ID----------------------
    LUMIERA_EXCEPTION_DECLARE (Logic,    Error,  LUMIERA_ERROR_LOGIC);
    LUMIERA_EXCEPTION_DECLARE (Fatal,    Logic,  LUMIERA_ERROR_FATAL);
    LUMIERA_EXCEPTION_DECLARE (Config,   Error,  LUMIERA_ERROR_CONFIG);
    LUMIERA_EXCEPTION_DECLARE (State,    Error,  LUMIERA_ERROR_STATE);
    LUMIERA_EXCEPTION_DECLARE (Invalid,  Error,  LUMIERA_ERROR_INVALID);
    LUMIERA_EXCEPTION_DECLARE (External, Error,  LUMIERA_ERROR_EXTERNAL);
    
    
    /** install our own handler for undeclared exceptions. Will be
     *  called automatically ON_BASIC_INIT when including errror.hpp
     *  @see appconfig.hpp  */
    void install_unexpectedException_handler ();
    namespace {
      LifecycleHook schedule_ (ON_BASIC_INIT, &install_unexpectedException_handler);         
    }
    
  } // namespace error

} // namespace lumiera



/******************************************************
 * if NoBug is used, redefine some macros 
 * to rather throw Lumiera Errors instead of aborting
 */
#ifdef NOBUG_ABORT
#undef NOBUG_ABORT
#define LUMIERA_NOBUG_LOCATION \
  std::string (NOBUG_BASENAME(__FILE__)) +":"+ NOBUG_STRINGIZE(__LINE__) + ", function " + __func__
#define NOBUG_ABORT \
  lumiera::error::assertion_terminate (LUMIERA_NOBUG_LOCATION);
#endif


#endif // LUMIERA_ERROR_HPP_
