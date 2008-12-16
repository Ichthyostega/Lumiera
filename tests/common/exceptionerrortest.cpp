/*
  Exceptionhandlin(Test)  -  throwing and catching our exception type
 
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
 
* *****************************************************/



#include "lib/error.h"
#include "include/error.hpp"

#include "common/test/run.hpp"
#include "common/util.hpp"


#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>

using std::runtime_error;
using std::exception;
using std::string;
using std::cout;



namespace lumiera
  {
  namespace test
    {
        
    /** local specific error-constant for use in the 
     *  construcor of the nested SpecificError class.
     */
    LUMIERA_ERROR_DEFINE(LIFE_AND_UNIVERSE, "and everything?");
    LUMIERA_ERROR_DEFINE(DERIVED, "convoluted exception");
    
    /** declare a specific Error class with parent class error::external */
    LUMIERA_EXCEPTION_DECLARE (DerivedError, error::External,  LUMIERA_ERROR_DERIVED);
    
    
    /**********************************************************
     * Some aspects of C++ exception handling.
     * Not to be confused with the basic C-style error value 
     * mechanism used by the low-level parts of the backend.
     * Both approaches are laregely orthogonal, but the
     * C++ exception handling uses the C-style error constants.
     * 
     */
    class ExceptionError_test : public Test
      {
        typedef ExceptionError_test test;
        virtual void run (Arg arg)
          {
            if (0 < arg.size() && arg[1]=="terminate")
              terminateUnknown ();
            
            
            catcher (&test::throwSpecial,  "");
            catcher (&test::throwDerived,  "test-1");
            catcher (&test::throwFatal,    "test-2");
            catcher (&test::throwInvalid,  "test-3");
            catcher (&test::throwExternal, "test-4");
            catcher (&test::throwRuntime,  "test-5");
            catcher (&test::throwExceptn,  "test-6");
            
            catcher (&test::nestedThrower, "test-7");
            catcher (&test::doubleNestedTh,"test-8");
            
            checkErrorIntegration();
            checkRootCauseChaining();
          }
        
        
        
        /** @test simply throw some exception and pass context info */
        void throwSpecial (string _) { throw SpecificError(); }
        void throwDerived (string _) { throw DerivedError(); }
        void throwFatal   (string _) { throw error::Fatal(_); }
        void throwInvalid (string _) { throw error::Invalid(_); }
        void throwExternal(string _) { throw error::External(_); }
        void throwRuntime (string _) { throw std::runtime_error(_); }
        void throwExceptn (string _) { throw std::exception(); }

        
        /** @test catching, repackaging and rethrowing of errors.
         *  This feature is important for passing exceptions transparentely
         *  over several layers. The nested operation will throw an error::External,
         *  which we are able to catch because it is derived from std::exception.
         *  We don't need to know the exact type, but we can classify the error situation
         *  as a "state error" and throw an error::State, passing on the root cause.
         *  Some levels up, this error get caught and the root cause can be 
         *  extracted successfully.
         */
        void nestedThrower (string msg) throw(Error)
        {
          try { throwExternal(msg); }
          catch (std::exception& e)
            {
              cout << "intermediate handler caught: " << e.what() 
                   << "....will rethrow as error::State\n";
              throw error::State (e);
            }
        }
        
        /** @test repeated repackaging and rethrowing */
        void doubleNestedTh (string msg) throw(error::Config)
        {
          try { nestedThrower (msg); }
          catch (Error& e)
            {
              cout << "2nd intermediate handler caught: " << e.what() 
                   << "....will rethrow as error::Config\n";
              throw error::Config (e);
            }
        }
        
        
        /** @test by constructing an lumiera::Error object,
         *  the corresponding lumiera_error state is set automatically
         */
        void checkErrorIntegration()
        {
          lumiera_error ();
          ASSERT (!lumiera_error ());
          
          Error err1;
          Error err2("boo",LUMIERA_ERROR_DERIVED);
          ASSERT (err1.getID () == lumiera_error ()); // (we didn't clear the first one!)
          
          Error err3("boooo",LUMIERA_ERROR_DERIVED);
          ASSERT (err3.getID () == lumiera_error ());
          
          SpecificError err4;
          ASSERT (err4.getID () == LUMIERA_ERROR_LIFE_AND_UNIVERSE);
          ASSERT (err4.getID () == lumiera_error ());
          
          ASSERT (!lumiera_error ());
        }
        
        /** @test the chaining of lumiera::Exception objects
         *  and the retrieval of the original root cause.
         */
        void checkRootCauseChaining()
        {
          error::Logic err1;
          error::Config err2(err1);
          error::Config err3(err2); //note: using copy ctor behaves like chaining
          Error err4(err1);        // note: copy ctor
          
          std::runtime_error rerr("what a shame");
          error::External err5(rerr);
          Error err6(err5);
          
          ASSERT (err2.rootCause() == err1.what());
          ASSERT (err3.rootCause() == err1.what());
          ASSERT (err4.rootCause() == err1.what());

          ASSERT (err5.rootCause() == rerr.what());
          ASSERT (err6.rootCause() == rerr.what());
        }
        
        
        /** @test terminate the Application by throwing an undclared exception.
         *        this should result in the global unknown() handler to be called,
         *        so usually it will terminate the testrun. 
         *  @note inside error.hpp, an initialisation hook has been installed into
         *        AppState, causing our own unknown() handler to be installed and
         *        invoked, which gives additional diagnostics.*/
        void terminateUnknown ()  throw() 
        {
          throw Error("You'll never get me, won't you?");
        }
        
        
        /** a very specific Exception class
         *  local to this scope and with 
         *  additional behaviour.
         */ 
        class SpecificError : public error::Invalid
          {
            int value;
          public:
            SpecificError () : Invalid("don't panic",LUMIERA_ERROR_LIFE_AND_UNIVERSE), value(42) {}
            int revealIt ()   { return value; }
          };
        
        
        
        /** helper: provides a bunch of catch-clauses and 
         *  runs the given member functions within
         */
        void catcher (void (ExceptionError_test::*funky)(string), string context)
        {
          try
            {
              (this->*funky) (context);
            }
          
          catch (SpecificError& e) { cout << "caught: " << e.what() << "..the answer is: " << e.revealIt() << "\n"; }                
          catch (error::Logic&  e) { cout << "caught error::Logic: " << e.what() << "\n"; }                
          catch (error::Invalid&e) { cout << "caught error::Invalid: " << e.what() << "\n"; }                
          catch (Error&         e) { cout << "caught lumiera::Error: " << e.what() << "\n"; }                
          catch (runtime_error& e) { cout << "caught std::runtime_error: " << e.what() << "\n"; }                
          catch (exception&     e) { cout << "caught std::exception. (unspecific)" << "\n"; }
          catch (...)              { cout << "caught an unknown exception\n"; }
        }
      };

      
      
      /** register this test class... */
      LAUNCHER (ExceptionError_test, "function common");

      
  } // namespace test
    
} // namespace util

