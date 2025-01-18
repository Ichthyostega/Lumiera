/*
  Parse(Test)  -  verify parsing textual specifications

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file parse-test.cpp
 ** unit test \ref Parse_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/parse.hpp"
//#include "lib/iter-explorer.hpp"
//#include "lib/format-util.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////////TODO
//#include "lib/util.hpp"

//#include <vector>
//#include <memory>



namespace util {
namespace parse{
namespace test {
  
  using lib::meta::is_Tuple;
  using std::get;
//  using util::join;
//  using util::isnil;
//  using std::vector;
//  using std::shared_ptr;
//  using std::make_shared;
  
//  using LERR_(ITER_EXHAUST);
//  using LERR_(INDEX_BOUNDS);
  
  
  namespace { // test fixture
    
//    const uint NUM_ELMS = 10;
    
//    using Numz = vector<uint>;
    
  } // (END)fixture
  
  
  
  
  
  
  
  /************************************************************************//**
   * @test verify helpers and shortcuts for simple recursive descent parsing
   *       of structured data and specifications.
   *
   * @see parse.hpp
   * @see proc-node.cpp "usage example"
   */
  class Parse_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleBlah();
          acceptTerminal();
          acceptSequential();
        }
      
      
      /** @test TODO just blah. */
      void
      simpleBlah ()
        {
        }
      
      
      /** @test define a terminal symbol to match by parse. */
      void
      acceptTerminal()
        {
          // set up a parser function to accept some token as terminal
          auto parse = Parser{"hello (\\w+) world"};
          string toParse{"hello vile world of power"};
          auto eval = parse (toParse);
          CHECK (eval.result);
          auto res = *eval.result;                             // ◁——————————— the »result model« of a terminal parse is the RegExp-Matcher 
          CHECK (res.ready() and not res.empty());
          CHECK (res.size()     == "2"_expect );
          CHECK (res.position() == "0"_expect );
          CHECK (res.str()    == "hello vile world"_expect );
          CHECK (res[1]       ==      "vile"_expect );
          CHECK (res.suffix() == " of power"_expect );
          
          auto syntax = Syntax{move (parse)};                  // Build a syntax clause from the simple terminal symbol parser
          CHECK (not syntax.hasResult());
          syntax.parse (toParse);
          CHECK (syntax.success());                            // Syntax clause holds an implicit state from the last parse
          CHECK (syntax.getResult()[1] == "vile"_expect);
          
          // shorthand notation to start building a syntax
          auto syntax2 = accept ("(\\w+) world");
          CHECK (not syntax2.hasResult());
          syntax2.parse (toParse);
          CHECK (not syntax2.success());
          string bye{"cruel world"};
          syntax2.parse (bye);
          CHECK (syntax2.success());
          CHECK (syntax2.getResult()[1] == "cruel"_expect);
          
          // going full circle: extract parser def from syntax
//          using Conn = decltype(syntax2)::Connex;
//          Conn conny{syntax2};
//          auto parse2 = Parser{conny};
          auto parse2 = Parser{syntax2.getConny()};
          CHECK (eval.result->str(1) == "vile");
          eval = parse2 (toParse);
          CHECK (not eval.result);
          eval = parse2 (bye);
          CHECK (eval.result->str(1) == "cruel");
        }
      
      
      /** @test define a sequence of syntax structures to match by parse. */
      void
      acceptSequential()
        {
          // Demonstration: how sequence combinator works....
          auto term1 = buildConnex ("hello");
          auto term2 = buildConnex ("world");
          auto parseSeq = [&](StrView toParse)
                              {
                                using R1 = decltype(term1)::Result;
                                using R2 = decltype(term2)::Result;
                                using ProductResult = std::tuple<R1,R2>;
                                using ProductEval = Eval<ProductResult>;
                                auto eval1 = term1.parse (toParse);
                                if (eval1.result)
                                  {
                                    uint end1 = eval1.consumed;
                                    StrView restInput = toParse.substr(end1);
                                    auto eval2 = term2.parse (restInput);
                                    if (eval2.result)
                                      {
                                        uint consumedOverall = end1 + eval2.consumed;
                                        return ProductEval{ProductResult{move(*eval1.result)
                                                                        ,move(*eval2.result)}
                                                          ,consumedOverall
                                                          };
                                      }
                                  }
                                return ProductEval{std::nullopt};
                              };
          string s1{"hello millions"};
          string s2{"hello world"};
          string s3{" hello world trade "};
          
          auto e1 = parseSeq(s1);
          CHECK (not e1.result);                               // Syntax 'hello'>>'world' does not accept "hello millions"
          auto e2 = parseSeq(s2);
          CHECK (    e2.result);
          
          using SeqRes = std::decay_t<decltype(*e2.result)>;   // Note: the result type depends on the actual syntax construction
          CHECK (is_Tuple<SeqRes>());                          //       Result model from sequence is the tuple of terminal results
          auto& [r1,r2] = *e2.result;
          CHECK (r1.str() == "hello"_expect);
          CHECK (r2.str() == "world"_expect);
          
          CHECK (term2.parse(" world").result);                // Note: leading whitespace skipped by the basic terminal parsers
          CHECK (term2.parse("\n \t world  ").result);
          CHECK (not term2.parse(" old  ").result);
          
          
          // DSL parse clause builder: a sequence of terminals...
          auto syntax = accept("hello").seq("world");
          
          // Perform the same parse as demonstrated above....
          CHECK (not syntax.hasResult());
          syntax.parse(s1);
          CHECK (not syntax.success());
          syntax.parse(s2);
          CHECK (syntax);
          SeqRes seqModel = syntax.getResult();
          CHECK (get<0>(seqModel).str() == "hello"_expect);
          CHECK (get<1>(seqModel).str() == "world"_expect);
          
          
          // can build extended clause from existing one
          auto syntax2 = syntax.seq("trade");
          CHECK (not syntax2.hasResult());
          syntax2.parse(s2);
          CHECK (not syntax2.success());
          syntax2.parse(s3);
          CHECK (syntax2.success());
          auto seqModel2 = syntax2.getResult();                // Note: model of consecutive sequence is flattened into a single tuple
          CHECK (get<0>(seqModel2).str() == "hello"_expect);
          CHECK (get<1>(seqModel2).str() == "world"_expect);
          CHECK (get<2>(seqModel2).str() == "trade"_expect);
        }
    };
  
  LAUNCHER (Parse_test, "unit common");
  
  
}}} // namespace util::parse::test

