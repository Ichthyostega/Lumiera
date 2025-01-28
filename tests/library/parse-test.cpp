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
  using lib::meta::typeSymbol;
  using lib::meta::is_Tuple;
  using std::decay_t;
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
          simpleUsage();
          acceptTerminal();
          acceptSequential();
          acceptAlternatives();
          acceptIterWithDelim();
          acceptOptionally();
          acceptBracketed();
          
          verify_modelBinding();
          verify_recursiveSyntax();
          verify_nestedSpecTerms();
        }
      
      
      /** @test TODO just blah. */
      void
      simpleUsage ()
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
          smatch res = *eval.result;                           // ◁——————————— the »result model« of a terminal parse is the RegExp-Matcher 
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
          
          // Going full circle: extract Parser definition from syntax
          auto parse2 = Parser{syntax2};
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
          
          
          // AltModel with homogeneous types are special
          auto hom = AltModel<int,int>::mark_right(42);
          CHECK (hom.getAny()   == 42);
          CHECK (hom.selected() == 1 );
          
          hom = AltModel<int,int>::mark_left(55);
          CHECK (hom.getAny()   == 55);
          CHECK (hom.selected() == 0 );

          
          
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
          CHECK (syntax2.getResult().N == 3);                  // Note: further branch has been folded into an extended AltModel
          CHECK (syntax2.getResult().selected() == 0);         //  ... string s2 still matched the same branch (#0)
          CHECK (syntax2.getResult().get<0>().str() == "brazen");
          
          syntax2.parse("smarmy saviour");
          CHECK (syntax2.success());
          auto altModel2 = syntax2.getResult();
          CHECK (syntax2.getResult().selected() == 2);         //  ... but another string can match the added branch #2
          CHECK (syntax2.getResult().get<2>().str() == "smarmy saviour");
          CHECK (syntax2.getResult().get<2>().str(1) == "saviour");
        }                                                      // Note: syntax for this branch #2 captured an additional word
      
      
      
      /** @test define repetitive sequence with delimiter
       *      - demonstrate how actually to accept such a flexible sequence
       *      - cover integration into the syntax clause DSL
       *      - repetition count and delimiter
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
          string s1{"seid umschlungen, Millionen"};
          string s2{"beguile, extort, profit"};
          
          auto e1 = parseSeq(s1);
          CHECK (e1.result);
          CHECK (e1.result->size() == 1);
          CHECK (e1.result->at(0).str() == "seid");
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
          auto syntax1 = accept_repeated(",", term);
          
          // Perform the same parse as demonstrated above....
          CHECK (not syntax1.hasResult());
          syntax1.parse(s1);
          CHECK (syntax1.success());
          auto res1 = syntax1.getResult();
          CHECK (res1.size() == 1);
          CHECK (res1.get(0).str() == "seid");
          
          syntax1.parse(s2);
          CHECK (syntax1.success());
          res1 = syntax1.getResult();
          CHECK (res1.size() == 3);
          CHECK (res1[0].str() == "beguile");
          CHECK (res1[1].str() == "extort" );
          CHECK (res1[2].str() == "profit" );
          
          auto syntax2 = accept_repeated(1,2,",", term);
          auto syntax3 = accept_repeated(  4,",", term);
          syntax2.parse(s2);
          syntax3.parse(s2);
          CHECK (    syntax2);
          CHECK (not syntax3);
          CHECK (syntax2.getResult().size() == 2);
          CHECK (s2.substr(syntax2.consumed()) == ", profit");
          
          auto sx = s2 + "  , \tdump";
          syntax3.parse(sx);
          CHECK (syntax3);
          CHECK (syntax3.getResult().size() == 4);
          CHECK (syntax3.getResult()[0].str() == "beguile");
          CHECK (syntax3.getResult()[1].str() == "extort" );
          CHECK (syntax3.getResult()[2].str() == "profit" );
          CHECK (syntax3.getResult()[3].str() == "dump"   );
          
          auto syntax4 = accept_repeated(term);
          syntax4.parse(s1);
          CHECK (syntax4.success());
          CHECK (syntax4.getResult().size() == 2);
          CHECK (syntax4.getResult()[0].str() == "seid");
          CHECK (syntax4.getResult()[1].str() == "umschlungen" );
          CHECK (s1.substr(syntax4.consumed()) == ", Millionen");
        }
      
      
      
      /** @test define compound syntax with optional sub-clause
       *      - use the DSL to construct a complex syntax
       *      - by default, several parts are implicitly sequenced
       *      - here we combine repeated parts with an optional clause
       *      - which in turn is again a compound syntax clause
       *      - the produced model reflects the structure of this syntax
       *      - result model of the optional clause is wrapped into `std::optional`
       *      - terminal elements produce a `std::smatch` (RegExp matcher object)
       */
      void
      acceptOptionally()
        {
          auto syntax = accept_repeated(",", "\\w+")                       // first we look for comma separated words
                          .opt(accept("and")                               // then (implicitly sequenced) an optional clause
                                .repeat("\\w+"));                          //       ... comprising "and" followed by several words
          using Model = decay_t<decltype(syntax.getResult())>;
          
          string s1{"fearmongering, scapegoating, intimidation"};
          string s2{"charisma and divine blessing"};
          
          CHECK (not syntax.hasResult());
          syntax.parse(s1);
          CHECK (syntax.success());
          
          Model  res1 = syntax.getResult();
          CHECK (typeSymbol(res1)       == "SeqModel");
          CHECK (typeSymbol(res1.get<0>()) == "IterModel");
          CHECK (typeSymbol(res1.get<1>()) == "optional");
          
          CHECK (res1.N                 == 2);                             // 2-component tuple at top
          CHECK (res1.get<0>().size()   == 3);                             // sequence in 1st component matched 3 elements
          CHECK (res1.get<0>()[0].str() == "fearmongering");               // elements in the sequence...
          CHECK (res1.get<0>()[1].str() == "scapegoating");
          CHECK (res1.get<0>()[2].str() == "intimidation");
          CHECK (res1.get<1>()          == std::nullopt);                  // the optional clause did not match
          
          syntax.parse(s2);
          CHECK (syntax.success());
          
          Model  res2 = syntax.getResult();
          CHECK (typeSymbol(res2)       == "SeqModel");                    //            Syntax                    SeqModel
          CHECK (typeSymbol(res2.get<0>()) == "IterModel");                //  repeat(word)  opt            IterModel   optional
          CHECK (typeSymbol(res2.get<1>()) ==  "optional");                //                 |                            |
          CHECK (typeSymbol(*res2.get<1>()) == "SeqModel");                //              Syntax                       SeqModel
          CHECK (typeSymbol(res2.get<1>()->get<0>()) == "match_results");  //           "and"  repeat(word)        Terminal  IterModel
          CHECK (typeSymbol(res2.get<1>()->get<1>()) == "IterModel");      //
          
          CHECK (res2.get<0>().size()   == 1);
          CHECK (res2.get<0>()[0].str() == "charisma");
          CHECK (res2.get<1>()          != std::nullopt);
          CHECK (res2.get<1>()->N       == 2);
          CHECK (res2.get<1>()->get<0>().str()    == "and");
          CHECK (res2.get<1>()->get<1>().size()   == 2      );
          CHECK (res2.get<1>()->get<1>()[0].str() == "divine" );
          CHECK (res2.get<1>()->get<1>()[1].str() == "blessing" );
          
          string s3{s1+" , "+s2};
          syntax.parse(s3);
          CHECK (syntax.success());
          
          Model  res3 = syntax.getResult();
          CHECK (typeSymbol(res3)       == "SeqModel");
          CHECK (res3.get<0>().size()   == 4);
          CHECK (res3.get<0>()[0].str() == "fearmongering");
          CHECK (res3.get<0>()[1].str() == "scapegoating");
          CHECK (res3.get<0>()[2].str() == "intimidation");
          CHECK (res3.get<0>()[3].str() == "charisma");
          CHECK (res3.get<1>()          != std::nullopt);
          CHECK (res3.get<1>()->N       == 2);
          CHECK (res3.get<1>()->get<0>().str() == "and");
          CHECK (res3.get<1>()->get<1>().size() == 2);
          CHECK (res3.get<1>()->get<1>()[0].str() == "divine");
          CHECK (res3.get<1>()->get<1>()[1].str() == "blessing");
        }
      
      
      
      /** @test define syntax with bracketed sub-expressions */
      void
      acceptBracketed()
        {
          string word{"\\w+"};
          
          CHECK (not accept(word).bracket(word)   .parse("so sad"));
          CHECK (    accept(word).bracketOpt(word).parse("so sad"));
          CHECK (    accept(word).bracketOpt(word).parse("so (sad)"));
          
          CHECK (accept_bracket(word).parse(" ( again ) ").getResult().str() == "again");
          
          CHECK (not accept_bracket(word)   .parse("(again"));
          CHECK (not accept_bracketOpt(word).parse("(again"));
          CHECK (    accept_bracketOpt(word).parse("again)"));             // just stops before the trailing ')'
          CHECK (    accept_bracketOpt(word).parse("again)").consumed() == 5);
          CHECK (    accept_bracketOpt(word).parse(" again"));             // backtracks also over the whitespace
          
          CHECK (not accept_bracket("[]",word).parse("(again)"));
          CHECK (not accept_bracket("[]",word).parse("[again)"));
          CHECK (not accept_bracket("[]",word).parse("(again]"));
          CHECK (    accept_bracket("[]",word).parse("[again]"));
          CHECK (    accept_bracket("a","n","...").parse("again"));        // arbitrary expressions for open / close
          CHECK (not accept_bracket("a","n","...").parse(" gain"));        // opening expression "a" missing
          CHECK (not accept_bracket("a","n", word).parse("again"));        // "\\w+" consumes eagerly => closing expression not found
        }
      
      
      
      /** @test attach model-transformation functions at various levels,
       *        which is the primary intended way to build results from the parse.
       */
      void
      verify_modelBinding()
        {
          auto word{"\\w+"};
          auto syntax1 = accept(word).seq(word)                            // get a tuple with two RegExp-Matchers
                                       .bind([](SeqModel<smatch,smatch> res)
                                               {
                                                 return res.get<0>().str() +"-"+ res.get<1>().str();
                                               });
          
          string s1{"ham actor"};
          CHECK (not syntax1.hasResult());
          syntax1.parse(s1);
          CHECK (syntax1.success());
          auto res1 = syntax1.getResult();
          CHECK (showType<decltype(res1)>() == "string");                  // surprise! it's a simple string (as returned from λ)
          CHECK (res1 == "ham-actor"_expect);
          
          // 💡 shortcut for RegExp match groups...
          auto syntax1b = accept("(\\w+) (\\w+)");
          CHECK (accept(syntax1b).bindMatch( ).parse(s1).getResult() == "ham actor"_expect );
          CHECK (accept(syntax1b).bindMatch(1).parse(s1).getResult() ==   "ham"_expect );
          CHECK (accept(syntax1b).bindMatch(2).parse(s1).getResult() == "actor"_expect );
          CHECK (accept(syntax1b).bindMatch(3).parse(s1).getResult() ==      ""_expect );
          
          auto wordEx = accept(word).bindMatch();
          auto syntax1c = accept(wordEx)
                            .seq(wordEx)                                   // sub-expressions did already transform to string
                                       .bind([](SeqModel<string,string> res)
                                              { return res.get<0>() +"-"+ res.get<1>(); });
          
          CHECK (syntax1c.parse("ham  actor").getResult() == "ham-actor");
          CHECK (syntax1c.parse("con artist").getResult() == "con-artist");
          
          auto syntax1d = accept(word).seq(word)
                                       .bindMatch();                       // generic shortcut: ignore model, yield accepted part of input
          CHECK (syntax1d.parse("ham  actor").getResult() == "ham  actor");
          CHECK (syntax1d.parse(" ham actor").getResult() == "ham actor");
          
            // another example to demonstrate arbitrary transformations:
           //  each sub-expr counts the letters, and the top-level binding sums those up
          auto letterCnt = accept(word).bindMatch().bind([](string s){ return s.size(); });
          auto syntax1e = accept(letterCnt)
                            .seq(letterCnt)
                                       .bind([](auto m){ auto [l1,l2] = m; return l1+l2; });
                                                                           // note this time we provide a λ-generic and use a structured binding
          CHECK (syntax1e.parse("ham  actor").getResult() == 8);
          CHECK (syntax1e.parse("con artist").getResult() == 9);
        }
      
      
      
      /** @test definition of recursive Syntax clauses
       *      - pre-declared placeholder with known result
       *      - bind a syntax clause later to that placeholder,
       *        which is possibly only with a binding to yield
       *        the expected result type; in the example here
       *        we count the optional sequenced expressions
       *      - demonstrate textbook example of nested numeric
       *        expression, including parentheses and even a
       *        square root function. Calculate golden ratio!
       */
      void
      verify_recursiveSyntax()
        {
          auto recurse = expectResult<int>();
          CHECK (not recurse.canInvoke());
          
          recurse = accept("great")
                        .opt(accept("!")
                               .seq(recurse))
                                            .bind([](auto m) -> int
                                                    {
                                                      auto& [_,r] = m;
                                                      return 1 + (r? get<1>(*r):0);
                                                    });
          CHECK (recurse.canInvoke());
          
          recurse.parse("great ! great ! great");
          CHECK (recurse.success());
          CHECK (recurse.getResult() == 3 );
          
          CHECK (not recurse.parse("  ! great"));
          CHECK (recurse.parse("great ! great   actor").getResult() == 2);
          CHECK (recurse.parse("great ! great ! actor").getResult() == 2);
          
          
           //_____________________________________________
          // Build a recursive numeric expression syntax...
          auto num  = accept("\\d+")                .bindMatch().bind([](auto num){ return std::stod(num);         });
          auto sqrt = accept("√").seq(num)                      .bind([](auto seq){ return std::sqrt(get<1>(seq)); });
          
          CHECK (sqrt.parse(" √x ").getResult() ==  0 );
          CHECK (sqrt.parse(" √2 ").getResult() == "1.4142136"_expect);
          
          //    E ::= T [ + E ]
          //    T ::= F [ / F ]
          //    F ::= ( E ) | V
          //    V ::= num   | √ num
          auto expr = expectResult<double>();
          
          auto valu = accept(num).alt(sqrt)                     .bind([](auto alt){ return alt.getAny(); });
          auto fact = accept_bracket(expr).alt(valu)            .bind([](auto alt){ return alt.getAny(); });
          auto term = accept(fact).opt(accept("/")  .seq(fact)) .bind([](auto seq){ auto [f1,f2] = seq; return f1 / (f2? get<1>(*f2) : 1.0); });
               expr = accept(term).opt(accept("\\+").seq(expr)) .bind([](auto exp){ auto [s1,s2] = exp; return s1 + (s2? get<1>(*s2) : 0.0); });
          
          CHECK (expr.canInvoke());
          CHECK (not expr.hasResult());
          
          expr.parse(" 42 forever");
          CHECK (expr.success());
          CHECK (expr.getResult() == 42 );
          
          expr.parse(" 42 + 13 =?");
          CHECK (expr.success());
          CHECK (expr.getResult() == 55 );
          
          expr.parse(" 1 + 4/3 ");
          CHECK (expr.success());
          CHECK (expr.getResult() == "2.3333333"_expect);
          
          expr.parse("(2+2)/(2+1) + 4/2");
          CHECK (expr.success());
          CHECK (expr.getResult() == "3.3333333"_expect);
          
          expr.parse("(1 + √5) / 2 ");
          CHECK (expr.success());
          CHECK (expr.getResult() == "1.618034"_expect);
        }
      
      
      
      /** @test demonstrate how to extract a nested specification term
       *      - accept anything not delimiter-like
       *      - open nested scope for parentheses and quotes
       *      - especially this allows proper handling of comma separated
       *        lists enclosed in parentheses, when the term itself is
       *        also part of a comma separated list — such a term-selection
       *        can not be achieved with regular expressions alone.
       */
      void
      verify_nestedSpecTerms()
        {
          auto content = accept(R"_([^,\\\(\)\[\]{}<>"]+)_");
          auto escape  = accept(R"_(\\.)_");
          
          auto nonQuot = accept(R"_([^"\\]+)_");
          auto quoted = accept_repeated(accept(nonQuot).alt(escape));
          auto quote = accept_bracket("\"\"", quoted);
          
          auto paren = expectResult<NullType>();
          auto nonParen = accept(R"_([^\\\(\)"]+)_");
          auto parenCont = accept_repeated(accept(nonParen)
                                             .alt(escape)
                                             .alt(quote)
                                             .alt(paren));
               paren = accept_bracket("()", parenCont).bind([](auto){ return NullType{}; });
          
          auto spec = accept_repeated(accept(content)
                                        .alt(escape)
                                        .alt(quote)
                                        .alt(paren));
          
          auto apply = [](auto& syntax)
                        { return [&](auto const& str)
                                    { return accept(syntax).bindMatch()
                                                           .parse(str)
                                                           .getResult();
                                    };
                        };
          
SHOW_EXPR(apply(content)("prey .. haul .. loot"))
SHOW_EXPR(apply(content)("prey .. haul ,. loot"))
SHOW_EXPR(apply(content)("prey .( haul ,. loot"))
          
SHOW_EXPR(apply(quote)("\"prey .( haul ,\"loot"))
SHOW_EXPR(apply(quote)("\"prey \\ haul ,\"loot"))
SHOW_EXPR(apply(quote)("\"prey\\\"haul ,\"loot"))
          
SHOW_EXPR(apply(paren)("(prey) .. haul .. loot"))
SHOW_EXPR(apply(paren)("(prey .. haul .. loot)"))
SHOW_EXPR(apply(paren)("(prey(..(haul)..)loot)"))
SHOW_EXPR(apply(paren)("(prey \" haul)\" loot)"))
SHOW_EXPR(apply(paren)("(prey\\( haul)\" loot)"))
          
SHOW_EXPR(apply(spec)("\"prey .( haul ,\"loot!"))
SHOW_EXPR(apply(spec)("\"prey .( haul \",loot!"))
SHOW_EXPR(apply(spec)("  prey .( haul \",loot!"))
SHOW_EXPR(apply(spec)("  prey .( haul )\"loot!"))
SHOW_EXPR(apply(spec)(" (prey\\( haul }, loot)"))
        }
    };
  
  LAUNCHER (Parse_test, "unit common");
  
  
}}} // namespace util::parse::test

