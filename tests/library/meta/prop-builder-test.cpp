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


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/meta/prop-builder.hpp"
#include "lib/format-string.hpp"

#include <string>

using std::string;
using util::_Fmt;

namespace lib  {
namespace meta {
namespace test {
  
  using ::test::showType;
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
       *      - create from the PropBuilder
       *      - add named fields with values as desired
       *      - the result is a record holding the data values
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
       *      - chain-up further layers with named fields by extending self-type
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
          
          CHECK (baseref.showSelfType() == "PropBuilder_test::Base"_expect    );
          CHECK (derived.showSelfType() == "PropBuilder_test::Derived"_expect );
          
          
          // The next building block exploits the fact that C++ allows to define
          // a struct or class locally, within a function scope; and while this
          // local type can be _referred to_ only locally, within this function,
          // it is _not a hidden type_. Data elements using such a local type
          // can be returned by-value — which allows us to "drop off" such
          // a data record, and thus also implicitly export its type.
          
          // While this seems rather pointless in itself, it can be leveraged
          // with the help of C++20 explicit lambda template parameters: we can
          // move an instance of a base class into a function, thereby pick up
          // its type, then define a new derived class, and even move the given
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
          // arbitrary value (and type), also passed directly on invocation.
          // The result is a record object, which incorporates the base, together
          // with an additional "Layer" holding the new property, stored efficiently.
          
          CHECK (compoundRecord.something == evil);
          CHECK (compoundRecord.someData == 0.13f);
          
          using Compound = decltype(compoundRecord);
          CHECK (sizeof(SomeRecord) == sizeof(float));
          CHECK (sizeof(Compound)   >= sizeof(float)+sizeof(ushort));
          CHECK (alignof(Compound) == alignof(SomeRecord));
          CHECK (sizeof(Compound)  == 2 * alignof(float));

          CHECK (std::is_class_v<Compound>);
          CHECK (std::is_trivial_v<Compound>);
          CHECK (std::is_trivial_v<SomeRecord>);
          CHECK (std::is_trivially_copyable_v<Compound>);
          CHECK (std::is_trivially_copyable_v<SomeRecord>);
          
          // Yet obviously the generated record is not a POD,
          // since non-static members reside at several levels
          CHECK (not std::is_standard_layout_v<Compound>);
          
          CHECK ((is_Subclass_v<Compound, SomeRecord>));
          
          
          // Lastly, a chain of such »property layers« can be built gradually,
          // with the help of a hook-method, that grabs the _current record_
          // and passes it to the next extension-layer-λ. As first example,
          // the dropper-λ defined above can be used here too...
          auto record1 = PropBuilder()
                          .define (dropperLambda, 0.23)
                          ;
          using Rec1 = decltype(record1);
          CHECK ((is_Subclass_v<Rec1, PropBuilder>));
          CHECK (sizeof(Rec1) == sizeof(double));
          CHECK (TYPE(record1.something) == "double"_expect);
          CHECK (record1.something == 0.23);
          
          // Note that the name of the property field, here `something`,
          // needs to be hard wired into the builder-λ, due to the nature of C++.
          // Yet this hurdle can be overcome by generating the builder-λ from a *macro*
          auto record2 = record1
                          .define (PROP_FIELD(anything)
                                  ,[&](auto val){ return val + evil; } // ◁———————————— can pass a Lambda as "value"
                                  );
          using Rec2 = decltype(record2);
          CHECK ((is_Subclass_v<Rec2, Rec1>));
          CHECK ((is_Subclass_v<Rec2, PropBuilder>));
          CHECK (record2.something == 0.23);
          CHECK (record2.anything(11) == 66);
          evil = 13;
          CHECK (record2.anything(42) == 55);
          // note the anything-λ is _generic_
          CHECK (record2.anything(7.77f) == 20.77f);
          
          // Warning: record1 has been moved-away into record2,
          // so its content is now »undefined«
        }
      
      
        /** Example for demonstration of possible use-case */
        template<class POL>
        class Producer
          : POL
          {
          public:
            Producer (POL policy)
              : POL{move(policy)}
              { }
            
            decltype(auto)
            doIt()
              {
                return POL::wrap (POL::feed());
              }
          };
      
      /** @test show a didactical example how generated records
       *        can be used to support policy-based design.
       *      - the Producer template defined above can be configured
       *        with appropriate actions and operations that fit together
       *      - the test features a special context-bound producer.
       */
      void
      demonstrate_usage_scenario()
        {
          uint current{21};
          _Fmt putIt{"%s tralala"};
          
          auto doer = Producer{PropBuilder()
                                .define (PROP_FIELD(feed), [&]{ return current++; })
                                .define (PROP_FIELD(wrap), [&](auto it)->string { return putIt % it; })
                              };
          
          CHECK (doer.doIt() == "21 tralala"_expect);
          CHECK (22 == current);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PropBuilder_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
