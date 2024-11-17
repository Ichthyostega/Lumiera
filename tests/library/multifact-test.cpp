/*
  MultiFact(Test)  -  cover the configurable object-family creating factory

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file multifact-test.cpp
 ** unit test \ref MultiFact_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/multifact.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <functional>
#include <memory>
#include <string>



namespace lib {
namespace test{
  
  using boost::lexical_cast;
  using util::isSameObject;
  using util::isnil;

  using std::shared_ptr;
  using std::function;
  using std::string;
  using std::bind;
  
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  namespace { // hierarchy of test dummy objects
    
    struct Interface
      {
        virtual ~Interface() {};
        virtual operator string () =0;
      };
    
    
    
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
        string instanceID_;
        
        operator string()
          {
            return instanceID_ + lexical_cast<string> (ii);
          }
        
      public:
        Implementation(string id = "Impl-")
          : instanceID_(id)
        { }
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
  
  
  
  
  
  /******************************************************************************//**
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
          using TestFactory = factory::MultiFact<string, theID>;
          
          TestFactory theFact;
          
          // the first "production line" is wired to a free function
          theFact.defineProduction (ONE, buildOne);
          
          // second "production line" uses a explicit partial closure
          theFact.defineProduction (TWO, bind (buildSome<theID>, TWO));
          
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
          using TestFactory = factory::MultiFact<Interface, theID, factory::BuildRefcountPtr>;
          using PIfa = shared_ptr<Interface>;
          
          TestFactory theFact;
          
          // set up the "production lines" by lambda
          theFact.defineProduction (ONE, [] { return new Implementation<ONE>; });
          theFact.defineProduction (TWO, [] { return new Implementation<TWO>; });
          theFact.defineProduction (THR, [] { return new Implementation<THR>; });
          theFact.defineProduction (FOU, [] { return new Implementation<FOU>; });
          CHECK (!isnil (theFact));
          
          PIfa p1 = theFact(ONE);
          PIfa p2 = theFact(TWO);
          PIfa p3 = theFact(THR);
          PIfa p4 = theFact(FOU);
          
          PIfa p11 = theFact(ONE);
          
          CHECK ("Impl-1" == string(*p1));
          CHECK ("Impl-2" == string(*p2));
          CHECK ("Impl-3" == string(*p3));
          CHECK ("Impl-4" == string(*p4));
          
          CHECK ("Impl-1" == string(*p11));
          CHECK (!isSameObject(*p1, *p11));
          
          PIfa p12(p11);
          CHECK (isSameObject(*p11, *p12));
          CHECK ("Impl-1" == string(*p12));
          CHECK (1 == p1.use_count());
          CHECK (2 == p11.use_count());
          CHECK (2 == p12.use_count());
        }
      
      
      void
      pass_additional_arguments()
        {
          using TestFactory = factory::MultiFact<Interface*(string), theID>;
          
          TestFactory theFact;
          
          // set up the "production lines"
          theFact.defineProduction (ONE, [](string   ) { return new Implementation<ONE>;        });
          theFact.defineProduction (TWO, [](string   ) { return new Implementation<TWO>("X");   });
          theFact.defineProduction (THR, [](string id) { return new Implementation<THR>(id);    });
          theFact.defineProduction (FOU, [](string id) { return new Implementation<FOU>("Z"+id);});
          
          Interface *p1 = theFact(ONE, "irrelevant"),
                    *p2 = theFact(TWO, "ignored"),
                    *p3 = theFact(THR, "idiocy"),
                    *p4 = theFact(FOU, "omg"),
                    *p5 = theFact(FOU, "z");
          
          // does not compile...
          // theFact(ONE);
          // theFact(ONE, "foo", bar);
          
          CHECK ("Impl-1" == string(*p1));
          CHECK ("X2"     == string(*p2));
          CHECK ("idiocy3"== string(*p3));
          CHECK ("Zomg4"  == string(*p4));
          CHECK ("Zz4"    == string(*p5));
          
          CHECK (!isSameObject(*p4, *p5));
          CHECK (INSTANCEOF(Implementation<ONE>, p1));
          CHECK (INSTANCEOF(Implementation<TWO>, p2));
          CHECK (INSTANCEOF(Implementation<THR>, p3));
          CHECK (INSTANCEOF(Implementation<FOU>, p4));
          CHECK (INSTANCEOF(Implementation<FOU>, p5));
          
          delete p1;
          delete p2;
          delete p3;
          delete p4;
          delete p5;
        }
      
      
      void
      fed_a_custom_finishing_functor()
        {
          using TestFactory = factory::MultiFact<int(int), theID, factory::Build<long>::Wrapper>;
          
          TestFactory theFact;
          
          // Setup(1): each "production line" does a distinct calculation
          theFact.defineProduction (ONE, [](int par) { return par;     });
          theFact.defineProduction (TWO, [](int par) { return 2 * par; });
          theFact.defineProduction (THR, [](int par) { return par*par; });
          theFact.defineProduction (FOU, [](int par) { return 1 << par;});
          
          // Setup(2): and a common "wrapper functor" finishes
          //           the output of the chosen "production line"
          theFact.defineFinalWrapper([](int raw) { return raw + 1; });
          
          CHECK (long(1 + 1) == theFact(ONE, 1));
          CHECK (long(1 + 2) == theFact(ONE, 2));
          CHECK (long(1 + 3) == theFact(ONE, 3));
          
          CHECK (long(1 + 2) == theFact(TWO, 1));
          CHECK (long(1 + 4) == theFact(TWO, 2));
          CHECK (long(1 + 6) == theFact(TWO, 3));
          
          CHECK (long(1 + 1) == theFact(THR, 1));
          CHECK (long(1 + 4) == theFact(THR, 2));
          CHECK (long(1 + 9) == theFact(THR, 3));
          
          CHECK (long(1 + 2) == theFact(FOU, 1));
          CHECK (long(1 + 4) == theFact(FOU, 2));
          CHECK (long(1 + 8) == theFact(FOU, 3));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFact_test, "unit common");
  
  
  
}} // namespace lib::test
