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
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace test{
  
  using std::string;
  using meta::is_Subclass;
  using util::isSameObject;
  
  
  
  
  
  /*************************************************************//**
   * @test maintain a sequence of data tuples in local storage,
   *       providing pre-configured type-safe data access.
   * 
   * @see lib::HeteroData
   * @see NodeBase_test::verify_TurnoutSystem()
   */
  class HeteroData_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//          seedRand();
//          checksum = 0;
          
          verify_FrontBlock();
          verify_ChainBlock();
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
          Chain2& chain2 = reinterpret_cast<Chain2&> (b1);
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
          
          CHECK (std::get<0> (chain2) == 42);
//        CHECK (std::get<1> (chain2) == "1.618034"_expect);       ////////////////////////////TODO somehow the overload for std::tuple takes precedence here
//        CHECK (std::get<2> (chain2) == "Φ"_expect);
          
          CHECK (std::get<0> (b2) == "1.618034"_expect);
          CHECK (std::get<1> (b2) == "Φ"_expect);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HeteroData_test, "unit common");
  
  
}} // namespace lib::test
