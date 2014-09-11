/*
  MultiFact(Test)  -  unittest for the configurable object-family creating factory

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/muttifac.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
//#include <iostream>
#include <functional>
#include <string>



namespace lib {
namespace test{
  
  using boost::lexical_cast;
//  using lib::test::showSizeof;
  using util::isSameObject;
  using util::isnil;
//  using std::ostream;
  using std::string;
  using std::function;
  using std::bind;
//  using std::cout;
//  using std::endl;
  
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  namespace { // hierarchy of test dummy objects
    
    struct Interface
      {
        virtual ~Interface() {};
        virtual operator string () =0;
      };
    
    inline ostream& operator<< (ostream& os, Interface& ifa) { return os << string(ifa); }
    
    
    enum theID
      { ONE = 1
      , TWO
      , THR
      , FOU
      };
    
    
    
    template<theID ii>
    class Implementation
      : public Interface
      {
        operator string()
          {
            return "Impl-"+lexical_cast<string> (ii);
          }
        
      public:
//        static theID getTypeID() { return ii; }
      };
    
    
    
    template<typename X>
    string
    buildSome (X rawVal)
    {
      return lexical_cast<string> (rawVal);
    }
    
    string
    buildOne()
    {
      return buildSome(ONE);
    }
  }
  
  
  
  
  
  /*************************************************************************//**
   * @test verify the basic usage patterns of the configurable factory template.
   *       - Depending on the concrete fabrication signature, the factory can produce
   *         "things" by invoking suitable fabrication functions. These functions
   *         are to be installed beforehand, and can be addressed through an ID.
   *       - these fabrication functions are installed per instance of the factory.
   *         Such a concrete factory configuration can be copied
   *       - optionally each created product can be passed through a wrapper function
   *       - there is a preconfigured wrapper for creating refcounting smart ptrs.
   *       - it is possible to define a custom wrapper function on factory setup. 
   * @see  lib::MultiFact
   */
  class MultiFact_test : public Test
    {
      void
      run (Arg) 
        {
          produce_simple_values();
          produce_smart_pointers();
          pass_additional_arguments();
          fed_a_custom_finishing_functor();
        }
      
      string
      callMe (string val)
        {
          ++invocations_;
          return val;
        }
      uint invocations_ = 0;
      
      
      void
      produce_simple_values()
        {
          using TestFactory = factory::MuttiFac<string, theID>;
          
          TestFactory theFact;
          
          // the first "production line" is wired to a free function
          theFact.defineProduction (ONE, buildOne);
          
          // second "production line" uses a explicit partial closure
          theFact.defineProduction (TWO, bind (buildSome, TWO));
          
          // for the third "production line" we set up a function object
          auto memberFunction = bind (&MultiFact_test::callMe, this, "lalü");
          theFact.defineProduction (THR, memberFunction);
          
          // and the fourth "production line" uses a lambda, closed with a local reference
          string backdoor("backdoor");
          theFact.defineProduction (FOU, [&] {
                                            return backdoor;
                                         });
          
          CHECK (!isnil (theFact));
          CHECK (theFact(ONE) == "1");
          CHECK (theFact(TWO) == "2");
          
          CHECK (theFact(THR) == "lalü");
          CHECK (invocations_ == 1);
          
          CHECK (theFact(FOU) == "backdoor");
          backdoor = "I am " + backdoor.substr(0,4);
          CHECK (theFact(FOU) == "I am back");
          
          
          TestFactory anotherFact;
          CHECK (isnil (anotherFact));
          VERIFY_ERROR (INVALID, anotherFact(ONE) );
          
          anotherFact.defineProduction (ONE, memberFunction);
          CHECK (anotherFact(ONE) == "lalü");
          CHECK (invocations_ == 2);
          
          CHECK (theFact(THR) == "lalü");
          CHECK (invocations_ == 3);
          
          
          CHECK ( theFact.contains (FOU));
          CHECK (!anotherFact.contains (FOU));
          
          anotherFact = theFact;
          CHECK (anotherFact.contains (FOU));
          CHECK (!isSameObject(theFact, anotherFact));
          
          CHECK (anotherFact(ONE) == "1");
          CHECK (anotherFact(TWO) == "2");
          CHECK (anotherFact(THR) == "lalü");
          CHECK (anotherFact(FOU) == "I am back");
          CHECK (invocations_ == 4);
        }
      
      
      void
      produce_smart_pointers()
        {
        }
      
      
      void
      pass_additional_arguments()
        {
        }
      
      
      void
      fed_a_custom_finishing_functor()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFact_test, "unit common");
  
  
  
}} // namespace lib::test
