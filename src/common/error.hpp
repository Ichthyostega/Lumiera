/*
  ERROR.hpp  -  Cinelerra Exception Interface
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef CINELERRA_ERROR_HPP_
#define CINELERRA_ERROR_HPP_

#include <string>
#include "nobugcfg.h"
#include "lib/error.h"


namespace cinelerra
  {
  using std::string;


  
  /** error-ID for unspecified exceptions */
  CINELERRA_ERROR_DECLARE(EXCEPTION);  
  
  /**
   * Interface and Baseclass of all Exceptions thrown 
   * from within cinelerra (C++) code. Common operations
   * for getting an diagnostic message and for obtaining
   * the root cause, i.e. the first exception encountered
   * in a chain of exceptions. 
   */
  class Error : public std::exception
    {
    public:
      Error (string description="", const char* id=CINELERRA_ERROR_EXCEPTION) throw();
      Error (std::exception& cause,
             string description="", const char* id=CINELERRA_ERROR_EXCEPTION) throw();
      
      Error (const Error&) throw();
      virtual ~Error () throw() {};
      
      /** yield a diagnostic message characterizing the problem */
      virtual const char* what () const throw();

      /** the internal cinelerra-error-ID (was set as C-errorstate in ctor) */
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
      const char* id_;       ///< an CINELERRA_ERROR id, which is set as errorstate on construction
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
    void cinelerra_unexpectedException ()  throw();

    /** throw an error::Fatal indicating "assertion failure" */
    void assertion_terminate (const string& location);
    

    /* constants to be used as error IDs */
    CINELERRA_ERROR_DECLARE (LOGIC    );    ///< contradiction to internal logic assumptions detected  
    CINELERRA_ERROR_DECLARE (FATAL    );    ///< unable to cope with, internal logic floundered     
    CINELERRA_ERROR_DECLARE (CONFIG   );    ///< execution aborted due to misconfiguration
    CINELERRA_ERROR_DECLARE (STATE    );    ///< unforeseen internal state
    CINELERRA_ERROR_DECLARE (INVALID  );    ///< invalid input or parameters encountered
    CINELERRA_ERROR_DECLARE (EXTERNAL );    ///< failure in external service the application relies on
    CINELERRA_ERROR_DECLARE (ASSERTION);    ///< assertion failure
    
/** Macro for creating derived exception classes properly 
 *  integrated into cinelerra's exception hierarchy. Using
 *  this macro ensures that the new class will get the full
 *  set of constructors and behaviour common to all exception
 *  classes, so it should be used when creating an derived
 *  exception type for more then stricly local purposes
 */
#define CINELERRA_EXCEPTION_DECLARE(CLASS, PARENT, _ID_) \
    class CLASS : public PARENT                          \
      {                                                  \
      public:                                            \
        CLASS (std::string description="",               \
               const char* id=_ID_) throw()              \
        : PARENT (description, id)  {}                   \
                                                         \
        CLASS (std::exception& cause,                    \
               std::string description="",               \
               const char* id=_ID_) throw()              \
        : PARENT (cause, description, id)   {}           \
      };

    //---------------------------CLASS-----PARENT--ID----------------------
    CINELERRA_EXCEPTION_DECLARE (Logic,    Error,  CINELERRA_ERROR_LOGIC);
    CINELERRA_EXCEPTION_DECLARE (Fatal,    Logic,  CINELERRA_ERROR_FATAL);
    CINELERRA_EXCEPTION_DECLARE (Config,   Error,  CINELERRA_ERROR_CONFIG);
    CINELERRA_EXCEPTION_DECLARE (State,    Error,  CINELERRA_ERROR_STATE);
    CINELERRA_EXCEPTION_DECLARE (Invalid,  Error,  CINELERRA_ERROR_INVALID);
    CINELERRA_EXCEPTION_DECLARE (External, Error,  CINELERRA_ERROR_EXTERNAL);
    
    
  } // namespace error

} // namespace cinelerra



/******************************************************
 * if NoBug is used, redefine some macros 
 * to rather throw Cinelerra Errors instead of aborting
 */
#ifdef NOBUG_ABORT
#undef NOBUG_ABORT
#define CIN_NOBUG_LOCATION \
  std::string (NOBUG_BASENAME(__FILE__)) +":"+ NOBUG_STRINGIZE(__LINE__) + ", function " + __func__
#define NOBUG_ABORT \
  cinelerra::error::assertion_terminate (CIN_NOBUG_LOCATION);
#endif


#endif // CINELERRA_ERROR_HPP_
