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
  
  using lib::test::showType;
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
          acceptAlternatives();
          acceptIterWithDelim();
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
      
      
      /** @test define a sequence of syntax structures to match by parse.
       *      - first demonstrate explicitly how the consecutive parsing works
       *        and how both models are combined into a product model (tuple)
       *      - demonstrate how leading whitespace is skipped automatically
       *      - then perform the same parse with a Syntax clause build with
       *        the `seq()` builder-DSL
       *      - extend this Syntax by adding a further sequential clause.
       */
      void
      acceptSequential()
        {  //_______________________________________________
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
          
          using SeqRes = decltype(e2)::Result;                 // Note: the result type depends on the actual syntax construction
          CHECK (is_Tuple<SeqRes>());                          //       Result model from sequence is the tuple of terminal results
          auto& [r1,r2] = *e2.result;
          CHECK (r1.str() == "hello"_expect);
          CHECK (r2.str() == "world"_expect);
          
          CHECK (term2.parse(" world").result);                // Note: leading whitespace skipped by the basic terminal parsers
          CHECK (term2.parse("\n \t world  ").result);
          CHECK (not term2.parse(" old  ").result);
          
          
           //___________________________________________________
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
      
      
      
      /** @test define alternative syntax structures to match by parse.
       *      - first demonstrate how a model with alternative branches can be
       *        populated and gradually extended while searching for a match.
       *      - then show explicitly the logic to check and select branches
       *        and construct the corresponding sum-model (variant)
       */
      void
      acceptAlternatives()
        {  //_______________________________
          // Demonstrate Alt-Model mechanics
          using R1 = char;
          using R2 = string;
          using R3 = double;
          
          // build Model-Alternatives incrementally
          using A1 = AltModel<R1>;
          CHECK (showType<A1>() == "parse::AltModel<char>"_expect);
          
          using A2 = A1::Additionally<R2>;
          CHECK (showType<A2>() == "parse::AltModel<char, string>"_expect);
          
          // create instance to represent this second branch...
          A2 model2 = A2::mark_right ("seduced");
          CHECK (sizeof(A2) >= sizeof(string)+sizeof(size_t));
          CHECK (model2.SIZ == sizeof(string));
          CHECK (model2.TOP        == 1);
          CHECK (model2.selected() == 1);
          CHECK (model2.get<1>()   == "seduced");

          using A3 = A2::Additionally<R3>;
          A3 model3 = A3::mark_left (move (model2));
          CHECK (showType<A3>() == "parse::AltModel<char, string, double>"_expect);
          CHECK (sizeof(A3) == sizeof(A2));
          CHECK (model3.TOP        == 2);
          CHECK (model3.selected() == 1);
          CHECK (model3.get<1>()   == "seduced");

          auto res = move(model3);
          CHECK (showType<decltype(res)>() == "parse::AltModel<char, string, double>"_expect);
          CHECK (sizeof(res) == sizeof(A2));
          CHECK (res.selected()    == 1);
          CHECK (res.get<1>()      == "seduced");
          
          
           //_____________________________________________
          // Demonstration: how branch combinator works....
          auto term1 = buildConnex ("brazen");
          auto term2 = buildConnex ("bragging");
          auto parseAlt = [&](StrView toParse)
                              {
                                using R1 = decltype(term1)::Result;
                                using R2 = decltype(term2)::Result;
                                using SumResult = AltModel<R1,R2>;
                                using SumEval = Eval<SumResult>;
                                auto eval1 = term1.parse (toParse);
                                if (eval1.result)
                                  {
                                    uint endBranch1 = eval1.consumed;
                                    return SumEval{SumResult::mark_left (move(*eval1.result))
                                                  ,endBranch1
                                                  };
                                  }
                                auto eval2 = term2.parse (toParse);
                                if (eval2.result)
                                  {
                                    uint endBranch2 = eval2.consumed;
                                    return SumEval{SumResult::mark_right (move(*eval2.result))
                                                  ,endBranch2
                                                  };
                                  }
                                return SumEval{std::nullopt};
                              };
          string s1{"decent contender"};
          string s2{"brazen dicktator"};
          
          auto e1 = parseAlt(s1);
          CHECK (not e1.result);                               // does not compute....
          auto e2 = parseAlt(s2);                              // one hell of a match!
          CHECK (    e2.result);
          CHECK (e2.result->selected() == 0);                  // Selector-ID of the first matching branch (here #0)
          CHECK (e2.result->get<0>().str() == "brazen");       // We know that branch#0 holds a RegExp-Matcher (from term1)
          CHECK (e2.result->get<0>().suffix() == " dicktator");
          CHECK (e2.consumed == 6);
          CHECK (s2.substr(e2.consumed)  == " dicktator");
          
          
           //________________________________________________
          // DSL parse clause builder: alternative branches...
          auto syntax = accept("brazen").alt("bragging");
          
          // Perform the same parse as demonstrated above....
          CHECK (not syntax.hasResult());
          syntax.parse(s1);
          CHECK (not syntax.success());
          syntax.parse(s2);
          CHECK (syntax);
          auto altModel = syntax.getResult();
          CHECK (altModel.selected() == 0);
          CHECK (altModel.get<0>().str() == "brazen");
          
          // can build extended clause from existing one
          auto syntax2 = syntax.alt("smarmy (\\w+)");
          CHECK (not syntax2.hasResult());
          syntax2.parse(s1);
          CHECK (not syntax2.success());
          syntax2.parse(s2);
          CHECK (syntax2.success());
          CHECK (syntax2.getResult().N == 2);                  // Note: further branch has been folded into an extended AltModel
          CHECK (syntax2.getResult().selected() == 0);         //  ... string s2 still matched the same branch (#0)
          CHECK (syntax2.getResult().get<0>().str() == "brazen");
          
          syntax2.parse("smarmy saviour");
          CHECK (syntax2.success());
          auto altModel2 = syntax2.getResult();
          CHECK (syntax2.getResult().selected() == 2);         //  ... but another string can match the added branch #2
          CHECK (syntax2.getResult().get<2>().str() == "smarmy saviour");
          CHECK (syntax2.getResult().get<2>().str(1) == "saviour");
        }                                                      // Note: syntax for this branch #2 captured an additional word
      
      
      
      /** @test TODO define repetitive sequence with delimiter
       *      - demonstrate how actually to accept such a flexible sequence
       */
      void
      acceptIterWithDelim()
        {  //_______________________________________________
          // Demonstration: how repetitive sequence works....
          auto sep = buildConnex (",");
          auto term = buildConnex ("\\w+");
          auto parseSeq = [&](StrView toParse)
                              {
                                using Res = decltype(term)::Result;
                                using IterResult = std::vector<Res>;
                                using IterEval = Eval<IterResult>;
                                uint consumed{0};
                                IterResult results;
                                auto hasResults = [&]{ return not results.empty(); };
                                while (true)
                                  {
                                    uint offset{0};
                                    if (hasResults())
                                      {
                                        auto delim = sep.parse (toParse);
                                        if (not delim.result)
                                          break;
                                        offset += delim.consumed;
                                      }
                                    auto eval = term.parse (toParse.substr(offset));
                                    if (not eval.result)
                                      break;
                                    offset += eval.consumed;
                                    results.emplace_back (move(*eval.result));
                                    toParse = toParse.substr(offset);
                                    consumed += offset;
                                  }
                                return hasResults()? IterEval{move(results), consumed}
                                                   : IterEval{std::nullopt};
                              };
          string s1{"Seit umschlungen, Millionen"};
          string s2{"beguile, extort, profit"};
          
          auto e1 = parseSeq(s1);
          CHECK (e1.result);
          CHECK (e1.result->size() == 1);
          CHECK (e1.result->at(0).str() == "Seit");
          CHECK (e1.result->at(0).suffix() == " umschlungen, Millionen");
          CHECK (e1.consumed == 4);
          
          auto e2 = parseSeq(s2);
          CHECK (e2.result);
          CHECK (e2.result->size() == 3);
          CHECK (e2.result->at(0).str() == "beguile");
          CHECK (e2.result->at(1).str() == "extort" );
          CHECK (e2.result->at(2).str() == "profit" );
          CHECK (e2.result->at(0).suffix() == ", extort, profit");
          CHECK (e2.result->at(1).suffix() == ", profit");
          CHECK (e2.result->at(2).suffix() == ""        );
          CHECK (e2.consumed == s2.length());
          
          
           //______________________________________________
          // DSL parse clause builder: iterative sequence...
          auto syntax1 = accept("brazen").alt("bragging");
        }
    };
  
  LAUNCHER (Parse_test, "unit common");
  
  
}}} // namespace util::parse::test

