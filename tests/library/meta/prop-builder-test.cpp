/*
  PropBuilder(Test)  -  verify helpers for working with tuples and type sequences

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file prop-builder-test.cpp
 ** Demonstration and explanation of compile-time generated data records.
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/prop-builder.hpp"
//#include "lib/meta/tuple-helper.hpp"
//#include "meta/typelist-diagnostics.hpp"
//#include "meta/tuple-diagnostics.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/hetero-data.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////TODO

#include <string>

using std::string;
using lib::test::showType;
//using lib::test::showSizeof;
//using util::toString;
//using util::_Fmt;
//using std::is_same_v;
//using std::make_tuple;
//using std::get;

namespace lib  {
namespace meta {
namespace test {
  
  
  
  namespace { // test data
    
    
  } // (End) test data
  
  #define TYPE(_TY_) showType<decltype(_TY_)>()

  
  
  /*********************************************************************//**
   * @test Demonstrate how data records with arbitrary, named fields
   *       can be generated at compile time, using C++20 features.
   *       - explain the underlying mechanics
   *       - show a usage scenario
   */
  class PropBuilder_test : public Test
    {
      virtual void
      run (Arg)
        {
          simpleUsage();
          explain_building_blocks();
          demonstrate_usage_scenario();
        }
      
      
      /** @test show syntax and usage
       */
      void
      simpleUsage()
        {
          auto record = PropBuilder()
                          .define (PROP_FIELD(one), 11u)
                          .define (PROP_FIELD(two), "2")
                          ;
          
          CHECK (record.one == 11u);
          CHECK (record.two == "2"_expect);
          
          CHECK (TYPE(record.one) == "uint"_expect);
          CHECK (TYPE(record.two) == "const char *"_expect);
          
          record.one = 55;
          CHECK (record.one == 55u);
          
          auto copy = record;
          copy.two = "something different";
          CHECK (record.two == "2"_expect);
          CHECK (copy.two == "something different"_expect);
          
        }
      
      
        struct Base
          {
            template<class X>
            string
            showSelfType (this X&)
              {
                return showType<X>();
              }
          };
        
        struct Derived : Base { };
      
      
      /** @test demonstrate and verify the language features
       *        used to generate arbitrary record data types.
       *      - capture self-type with C++20 _explicit object member function_
       *      - use generic-λ to generate a derived class with a property field 
       */
      void
      explain_building_blocks()
        {
          // The first building block relies on the ability to pick up at invocation time
          // the actual type that is used to call a member function, even while this function
          // was defined somewhere in a base class: C++20 introduced a special syntax to mark
          // the first "this" argument, passed automatically to each member invocation.
          // And since code from a function template is generated late only, on actual use,
          // we can define a type parameter to capture the "self type" explicitly:
          
          Derived derived;
          Base& baseref{derived};
          
          CHECK (baseref.showSelfType() == "test::PropBuilder_test::Base"_expect    );
          CHECK (derived.showSelfType() == "test::PropBuilder_test::Derived"_expect );
          
          
          // The next building block exploits the fact that C++ allows to define
          // a struct or class locally, within a function scope; and while this
          // local type can be _referred to_ only locally, within this function,
          // it is _not a hidden type_. Data elements using such a local type
          // can be returned by-value — which allows us to "drop off" such
          // a data record, and this also implicitly its type.
          
          // While this seems rather pointless in itself, it can be leveraged
          // with the help of C++20 explicit lambda template parameters: we can
          // move an instance of a base class into a function, thereby pick up
          // its type, and define a new derived class, and even move the given
          // base class instance into the embedded base class sub-object:
          
          auto dropperLambda = []<class BAS, typename VAL>(BAS&& bas, VAL&& val)
                                {
                                  using BaseLayer = std::decay_t<BAS>;
                                  using Property  = std::decay_t<VAL>;
                                    
                                    struct DataLayer_somthing
                                      : BaseLayer
                                      {
                                        Property something;
                                        
                                        DataLayer_somthing (BaseLayer b, Property v)
                                          : BaseLayer{move(b)}
                                          , something{move(v)}
                                          { }
                                        
                                        DataLayer_somthing() = default;
                                      };
                                  return DataLayer_somthing{forward<BAS>(bas)
                                                           ,forward<VAL>(val)
                                                           };
                                };
          
          struct SomeRecord
            {
              float someData;
            };
          
          ushort evil = 55;
          auto compoundRecord = dropperLambda (SomeRecord{0.13}, evil);
          
          // here SomeRecord was created transiently, moved into the dropper-λ,
          // used there as base for a derived struct DataLayer_something, which
          // features an additional property `something`, initialised with an
          // arbitrary value (and type) also passed on invocation.
          // The result is a record object, which incorporates the base, together
          // with an additional "Layer" holding the new property, stored efficiently.
          
          
          using Compound = decltype(compoundRecord);
SHOW_TYPE(decltype(compoundRecord))
SHOW_EXPR(TYPE(compoundRecord))
SHOW_EXPR(sizeof(SomeRecord))
          CHECK (sizeof(SomeRecord) == sizeof(float));
SHOW_EXPR(sizeof(Compound))
          CHECK (sizeof(Compound)   >= sizeof(float)+sizeof(ushort));
SHOW_EXPR(alignof(SomeRecord))
SHOW_EXPR(alignof(Compound))
          CHECK (alignof(Compound) == alignof(SomeRecord));
          CHECK (sizeof(Compound)  == 2 * alignof(float));

SHOW_EXPR(std::is_class_v<Compound>)
SHOW_EXPR(std::is_trivial_v<Compound>)
SHOW_EXPR(std::is_trivial_v<SomeRecord>)
SHOW_EXPR(std::is_trivially_copyable_v<SomeRecord>)
SHOW_EXPR(std::is_trivially_copyable_v<Compound>)
SHOW_EXPR(std::is_standard_layout_v<Compound>)
          CHECK (std::is_class_v<Compound>);
          CHECK (std::is_trivial_v<Compound>);
          CHECK (std::is_trivial_v<SomeRecord>);
          CHECK (std::is_trivially_copyable_v<Compound>);
          CHECK (std::is_trivially_copyable_v<SomeRecord>);
          
          // Yet obviously it is not a POD,
          // since non-static members reside at several levels
          CHECK (not std::is_standard_layout_v<Compound>);
          
SHOW_EXPR((is_Subclass_v<Compound, SomeRecord>))
          CHECK ((is_Subclass_v<Compound, SomeRecord>));
        }
      
      
      /** @test show how to use generated records to implement a policy.
       */
      void
      demonstrate_usage_scenario()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PropBuilder_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
