/*
  HeteroData(Test)  -  verify maintaining chained heterogeneous data in local storage

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file del-stash-test.cpp
 ** unit test \ref HeteroData_test
 */


#include "lib/test/run.hpp"
#include "lib/hetero-data.hpp"
#include "lib/meta/trait.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace test{
  
  using std::string;
  using meta::is_Subclass;
  using util::isSameObject;
  using util::isSameAdr;
  using util::getAdr;
  
  
  
  
  /******************************************************************//**
   * @test maintain a sequence of data tuples in local storage,
   *       providing pre-configured type-safe data access.
   *     - the initial block is just a tuple of data in local storage
   *     - but further extension segments can be created _elsewhere_
   *       and attached to an existing chain
   *     - a compile-time »overlay« of constructor- and accessor-functors
   *       is provided as _guard rails_ to prevent out-of bounds access.
   * @see lib::HeteroData
   * @see NodeBase_test::verify_TurnoutSystem()
   */
  class HeteroData_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_FrontBlock();
          verify_ChainBlock();
          verify_Accessors();
        }
      
      
      void
      simpleUsage()
        {
          using F = lib::HeteroData<uint,double>;                      // define type of the front-end segment
          auto h1 = F::build (1,2.3);                                  // build the front-end, including first data tuple
          using C = F::Chain<bool,string>;                             // define a constructor type for a follow-up segment
          auto b2 = C::build (true, "Ψ");                              // build this follow-up segment free-standing
          b2.linkInto(h1);                                             // link it as second segment into the chain
          C::AccessorFor<string> get4;                                 // get an accessor functor (picked by value type)
          CHECK (get4(h1) == "Ψ");                                     // use accessor on front-type (involves force-cast)
        }
      
      
      /** @test build a free standing data tuple block to start a chain */
      void
      verify_FrontBlock()
        {
          using Block1 = HeteroData<uint,double>;
          CHECK ((is_Subclass<Block1::NewFrame, std::tuple<uint,double>>()));
          
          auto b1 = Block1::build (42, 1.61803);
          CHECK (1.61803 == b1.get<1>());
          CHECK (42      == b1.get<0>());
          CHECK (showType<Block1::Elm_t<0>>() == "uint"_expect);
          CHECK (showType<Block1::Elm_t<1>>() == "double"_expect);
          
          Block1 b2;
          CHECK (0.0 == b2.get<1>());
          b2.get<1>() = 3.14;
          CHECK (3.14 == b2.get<1>());
          
          CHECK (2 == std::tuple_size_v<Block1::NewFrame::Tuple>);     // referring to the embedded tuple type
          CHECK (2 == std::tuple_size_v<Block1::NewFrame>);            // StorageFrame itself complies to the C++ tuple protocol
          CHECK (2 == std::tuple_size_v<Block1>);                      // likewise for the complete HeteroData Chain
          
          auto& [_,p] = b2;                                            // can use structured bindings...
          CHECK (p == 3.14);
          p = 3.14159;
          CHECK (3.14159 == b2.get<1>());
        }
      
      
      /** @test construct a follow-up data tuple block and hook it into the chain */
      void
      verify_ChainBlock()
        {
          using Block1 = HeteroData<uint>;
          CHECK ((is_Subclass<Block1::NewFrame, std::tuple<uint>>()));
          
          using Constructor = Block1::Chain<double,string>;
          using Block2 = Constructor::NewFrame;
          CHECK ((is_Subclass<Block2, std::tuple<double, string>>()));
          
          auto b1 = Block1::build (41);
          auto b2 = Constructor::build (1.61, "Φ");
          b2.linkInto(b1);
          
          using Chain2 = Constructor::ChainType;
          Chain2& chain2 = Constructor::recast (b1);
          CHECK (b1.size()     == 1);
          CHECK (chain2.size() == 3);
          
          CHECK (41   == chain2.get<0>());
          CHECK (1.61 == chain2.get<1>());
          CHECK ("Φ"  == chain2.get<2>());
          
          chain2.get<0>()++;
          chain2.get<1>() = (1 + sqrt(5)) / 2;
          
          CHECK (b1.get<0>()      == 42);
          CHECK (chain2.get<0>()  == 42);
          CHECK (std::get<0> (b2) == "1.618034"_expect);
          
          CHECK (isSameObject (chain2.get<0>() ,b1.get<0>()));
          CHECK (isSameObject (chain2.get<2>() ,std::get<1>(b2)));
          
          CHECK (1 == std::tuple_size_v<Block1::NewFrame::Tuple>);     // referring to the embedded tuple type
          CHECK (1 == std::tuple_size_v<Block1::NewFrame>);
          CHECK (1 == std::tuple_size_v<Block1>);
          
          CHECK (2 == std::tuple_size_v<Block2::Tuple>);               // referring to the embedded tuple type
          CHECK (2 == std::tuple_size_v<Block2>);
          
          CHECK (3 == std::tuple_size_v<Chain2>);
          CHECK ((showType<std::tuple_element_t<0, Chain2>>() == "uint"_expect));
          CHECK ((showType<std::tuple_element_t<1, Chain2>>() == "double"_expect));
          CHECK ((showType<std::tuple_element_t<2, Chain2>>() == "string"_expect));
          
          CHECK ((showType<std::tuple_element_t<0, Block2>>() == "double"_expect));
          CHECK ((showType<std::tuple_element_t<1, Block2>>() == "string"_expect));
          
//        CHECK (std::get<0> (chain2) == "42"_expect);                 // std::tuple is inaccessible base of HeteroData
          CHECK (std::get<0> (b2)     == "1.618034"_expect);
//        CHECK (std::get<1> (chain2) == "1.618034"_expect);           // does not compile due to range restriction for the base tuple
                                                                       // (as such this is correct — yet prevents definition of a custom get-function)
          auto& [u0] = b1;
          CHECK (u0 == "42"_expect);
          
          auto& [v0,v1] = b2;                                          // b2 is typed as StorageFrame and thus the tuple base is accessible
          CHECK (v0 == "1.618034"_expect);
          CHECK (v1 == "Φ"_expect);
          
          auto& [x0,x1,x2] = chain2;                                   // Note: structured binding on the fully typed chain uses the get<i>-Member
          CHECK (x0 == "42"_expect);
          CHECK (x1 == "1.618034"_expect);
          CHECK (x2 == "Φ"_expect);
          
//        auto& [z0,z1,z2,z3] = chain2;                                // Error: 4 names provided for structured binding, while HeteroData... decomposes into 3 elements
//        auto& [z0,z1,z2] = b1;                                       // Error: HeteroData<Node<StorageFrame<0, uint>, NullType> >' decomposes into 1 element
        }
      
      
      
      /** @test demonstrate elaborate storage layout with several chain frames
       *      - follow-up frames shall be built using constructor types
       *      - these can be defined prior to any data allocation
       *      - individual data elements can be accessed through type-safe accessor functors
       * @warning as demonstrated, this is a dangerous bare-bone memory layout without runtime checks!
       */
      void
      verify_Accessors()
        {
          using Front = lib::HeteroData<uint,double>;
          using Cons2 = Front::Chain<bool,string>;
          using Data2 = Cons2::NewFrame;
          using List2 = Cons2::ChainType;
          using Acc4  = Cons2::AccessorFor<string>;
          using Acc3  = Cons2::AccessorFor<bool>;
          using Acc2  = Front::Accessor<1>;
          using Acc1  = Front::Accessor<0>;
          using Cons3 = Cons2::ChainExtent<CStr,string>;
          using Data3 = Cons3::NewFrame;
          using List3 = Cons3::ChainType;
          using Acc5  = Cons3::AccessorFor<CStr>;
          using Acc6  = Cons3::AccessorFor<string>;
          CHECK (2 == Front::size());
          CHECK (4 == List2::size());
          CHECK (6 == List3::size());
          //
          // Note: up to now, not a single actual data element has been created
          // Moreover, individual blocks can be created in any order...
          Data2 d2;
          d2.get<1>() = "Ψ";
          Front front;
          CHECK (front.get<1>() == 0.0);
          front.get<1>() = 2.3;
          
          // Note the pitfall: Chain has not been connected yet,
          //                   but the Accessors would assume otherwise
          CHECK (Acc2::get(front) == 2.3);
//        Acc3::get(front);                                            // would cause NPE (or assertion failure on debug build)
          
          Acc4 get4;                                                   // could even instantiate the accessors...
          CHECK (sizeof(get4) == 1);                                   // (empty marker object with static methods)
//        get4(front);                                                 // likewise NPE or assertion fail
          
          // Now link the second data element in properly
          d2.linkInto(front);
          CHECK (Acc1::get(front) == 0);
          CHECK (Acc2::get(front) == 2.3);
          CHECK (Acc3::get(front) == false);
          CHECK (get4(front)      == "Ψ");
          
          // further allocations can even be »elsewhere«
          const void* loc;
          {
            Acc6 get6;
            auto magic = Cons3::build("magic","cloud");
            loc = getAdr(magic);
            CHECK (magic.get<0>() == "magic"_expect);
            CHECK (magic.get<1>() == "cloud"_expect);
            // link into the cloud...
            magic.linkInto(front);
            CHECK (get6(front) == "cloud");
          }// aaand...
          //  it's gone
          
          // Evil, evil...
          lib::UninitialisedStorage<Data3, 1> evilSpace;
          Data3& d3 = evilSpace[0];                                    // note: working with left-over data from expired stack frame
          CHECK (isSameAdr (d3, loc));
          CHECK (d3.get<0>() == "magic"_expect);                       // const char* points into static data, so the chars are still there
          new(&d3.get<1>()) string{"mushrooms"};                       // the "cloud"-string was destroyed by magic's destructor
          
          auto& [v1,v2,v3,v4,v5,v6] = Cons3::recast(front);            // using connectivity from the linked list connecting the segments
          CHECK (v1 == "0"_expect);
          CHECK (v2 == "2.3"_expect);
          CHECK (v3 == "false"_expect);
          CHECK (v4 == "Ψ"_expect);
          CHECK (v5 == "magic"_expect);
          CHECK (v6 == "mushrooms"_expect);
          
          v1 = 42;
          v2 = 5.5;
          v3 = true;
          CHECK (front.get<0>() == 42);
          CHECK (front.get<1>() == 5.5);
          CHECK (d2.get<0>()   == true);
          CHECK (d2.get<1>()   ==  "Ψ");
          
          CHECK (isSameAdr (Acc1::get(front), v1));
          CHECK (isSameAdr (Acc2::get(front), v2));
          CHECK (isSameAdr (Acc3::get(front), v3));
          CHECK (isSameAdr (Acc4::get(front), v4));
          CHECK (isSameAdr (Acc5::get(front), v5));
          CHECK (isSameAdr (Acc6::get(front), v6));
          
          CHECK (not isSameAdr (front, v1));
          CHECK (not isSameAdr (d2,    v3));
          CHECK (not isSameAdr (d3,    v5));
          // we can directly re-cast into another typed front-end
          List3& fullChain = Cons3::recast(front);
          CHECK (isSameAdr (fullChain.get<2>(), std::get<0>(d2)));
          CHECK (isSameAdr (fullChain.get<3>(), std::get<1>(d2)));
          CHECK (isSameAdr (fullChain.get<4>(), std::get<0>(d3)));
          CHECK (isSameAdr (fullChain.get<5>(), std::get<1>(d3)));
          CHECK (isSameAdr (fullChain.get<0>(), v1));
          CHECK (isSameAdr (fullChain.get<1>(), v2));
          CHECK (isSameAdr (fullChain.get<2>(), v3));
          CHECK (isSameAdr (fullChain.get<3>(), v4));
          CHECK (isSameAdr (fullChain.get<4>(), v5));
          CHECK (isSameAdr (fullChain.get<5>(), v6));
          // we can even use partially specified chains
          List2& partChain = Cons2::recast(fullChain);
          CHECK (isSameAdr (partChain.get<0>(), v1));
          CHECK (isSameAdr (partChain.get<1>(), v2));
          CHECK (isSameAdr (partChain.get<2>(), v3));
          CHECK (isSameAdr (partChain.get<3>(), v4));
          
          // Note: basically we are still using stale memory,
          //       previously allocated to the "magic" block,
          //       and now covered by the UninitialisedStorage
          CHECK (loc == & d3);
          CHECK (loc  < & v5);
          CHECK (loc  < & v6);
          
          // structural binding on partial chains is limited
          CHECK (partChain.size() == 4);
          auto& [w1,w2,w3,w4] = partChain;
          CHECK (isSameObject (v1, w1));
          CHECK (isSameObject (v2, w2));
          CHECK (isSameObject (v3, w3));
          CHECK (isSameObject (v4, w4));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HeteroData_test, "unit common");
  
  
}} // namespace lib::test
