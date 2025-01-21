/*
  PARSE.hpp  -  helpers for parsing textual specifications

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file parse.hpp
 ** Convenience wrappers and definitions for parsing structured definitions.
 ** Whenever a specification syntax entails nested structures, extracting contents
 ** with regular expressions alone becomes tricky. Without much sophistication, a
 ** directly implemented simple recursive descent parser is often less brittle and
 ** easier to understand and maintain. With some helper abbreviations, notably
 ** a combinator scheme to work from building blocks, a hand-written solution
 ** can benefit from taking short-cuts, especially related to result bindings.
 */


#ifndef LIB_PARSE_H
#define LIB_PARSE_H


#include "lib/branch-case.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/regex.hpp"

#include <optional>
#include <utility>
#include <cstddef>
#include <tuple>
#include <array>

namespace util {
  namespace parse {
    
    using std::move;
    using std::forward;
    using std::optional;
    using lib::meta::_Fun;
    using lib::meta::has_Sig;
    using lib::meta::NullType;
    using std::decay_t;
    using std::tuple;
    using std::array;
    
    using StrView = std::string_view;
    
    
    /**
     */
    template<class RES>
    struct Eval
      {
        using Result = RES;
        optional<RES> result;
        size_t consumed{0};
      };
    
    template<class FUN>
    struct Connex
      : util::NonAssign
      {
        using PFun = FUN;
        PFun parse;
        
        using Result = typename _Fun<PFun>::Ret::Result;
        
        Connex (FUN&& pFun)
          : parse{move(pFun)}
          { }
      };
    
    auto
    buildConnex(NullType)
    {
      return Connex{[](StrView) -> Eval<NullType>
                      {
                        return {NullType{}};
                      }};
    }
    using NulP = decltype(buildConnex (NullType()));
    
    auto
    buildConnex (regex rex)
    {
      return Connex{[regEx = move(rex)]
                    (StrView toParse) -> Eval<smatch>
                      {           // skip leading whitespace...
                        size_t pre = leadingWhitespace (toParse);
                        toParse = toParse.substr(pre);
                        auto result{matchAtStart (toParse,regEx)};
                        size_t consumed = result? pre+result->length() : 0;
                        return {move(result), consumed};
                      }};
    }
    using Term = decltype(buildConnex (std::declval<regex>()));
    
    Term
    buildConnex (string const& rexDef)
    {
      return buildConnex (regex{rexDef});
    }
    
    template<class FUN>
    auto
    buildConnex (Connex<FUN> const& anchor)
    {
      return Connex{anchor};
    }
    template<class FUN>
    auto
    buildConnex (Connex<FUN> && anchor)
    {
      return Connex{move(anchor)};
    }
    
    
    template<class CON, class BIND>
    auto
    adaptConnex (CON&& connex, BIND&& modelBinding)
    {
      using RX = typename CON::Result;
      using Arg = lib::meta::_FunArg<BIND>;
      static_assert (std::is_constructible_v<Arg,RX>,
                     "Model binding must accept preceding model result.");
      using AdaptedRes = typename _Fun<BIND>::Ret;
      return Connex{[origConnex = forward<CON>(connex)
                    ,binding = forward<BIND>(modelBinding)
                    ]
                    (StrView toParse) -> Eval<AdaptedRes>
                      {
                        auto eval = origConnex.parse (toParse);
                        if (eval.result)
                          return {binding (move (*eval.result))};
                        else
                          return {std::nullopt};
                      }};
    }
    
    
    /* ===== building structured models ===== */
    
    /**
     * Product Model : results from a conjunction of parsing clauses,
     * which are to be accepted in sequence, one after the other.
     */
    template<typename...RESULTS>
    struct SeqModel
      : tuple<RESULTS...>
      {
        static constexpr size_t SIZ = sizeof...(RESULTS);
        using Seq = lib::meta::TySeq<RESULTS...>;
        using Tup = std::tuple<RESULTS...>;
        
        SeqModel() = default;
        
        template<typename...XS, typename XX>
        SeqModel (SeqModel<XS...>&& seq, XX&& extraElm)
          : Tup{std::tuple_cat (seq.extractTuple()
                               ,make_tuple (forward<XX> (extraElm)) )}
          { }
          
        template<typename X1, typename X2>
        SeqModel (X1&& res1, X2&& res2)
          : Tup{move(res1), move(res2)}
          { }
        
        Tup&& extractTuple() { return move(*this); }
      };
    
    /**
     * Sum Model : results from a disjunction of parsing clauses,
     * which are are tested and accepted as alternatives, one at least.
     */
    template<typename...CASES>
    struct AltModel
      : lib::BranchCase<CASES...>
      {
        using _Model = lib::BranchCase<CASES...>;
        
        template<typename INIT,     typename =lib::meta::disable_if_self<AltModel,INIT>>
        AltModel (INIT&& init)
          : _Model{_Model::TOP, forward<INIT> (init)}
          { }
          
        template<typename EX>
        using Additionally = AltModel<CASES...,EX>;
        
        template<typename EX>
        Additionally<EX>
        addBranch()
          {
            Additionally<EX>& upFaked = reinterpret_cast<Additionally<EX>&> (*this);
            return {move (upFaked)};
          }
      };
    
    
    /** Special case Product Model to represent iterative sequence */
    template<typename RES>
    struct IterModel
      {
        
      };
    
    /** Marker-Tag for the result from a sub-expression, not to be joined */
    template<typename RES>
    struct SubModel
      {
        
      };
    
    /** Standard case : combinator of two model branches */
    template<template<class...> class TAG, class R1, class R2 =void>
    struct _Join
      {
        using Result = TAG<R1,R2>;
      };
    
    /** Generic case : extend a structured model by further branch */ 
    template<template<class...> class TAG, class...RS, class R2>
    struct _Join<TAG,TAG<RS...>,R2>
      {
        using Result = TAG<RS...,R2>;
      };
    
    /** Special Case : absorb sub-expression without flattening */
    template<template<class...> class TAG, class R1, class R2>
    struct _Join<TAG,SubModel<R1>,R2>
      {
        using Result = TAG<R1,R2>;
      };
    template<template<class...> class TAG, class R1, class R2>
    struct _Join<TAG,R1, SubModel<R2>>
      {
        using Result = TAG<R1,R2>;
      };
    template<template<class...> class TAG, class R1, class R2>
    struct _Join<TAG,SubModel<R1>,SubModel<R2>>
      {
        using Result = TAG<R1,R2>;
      };
    
    /** Special Case : absorb further similar elements into IterModel */
    template<class RES>
    struct _Join<IterModel, IterModel<RES>, RES>
      {
        using Result = IterModel<RES>;
      };
    
    
    /** accept sequence of two parse functions */
    template<class C1, class C2>
    auto
    sequenceConnex (C1&& connex1, C2&& connex2)
    {
      using R1 = typename decay_t<C1>::Result;
      using R2 = typename decay_t<C2>::Result;
      using ProductResult = typename _Join<SeqModel, R1, R2>::Result;
      using ProductEval = Eval<ProductResult>;
      return Connex{[conL = forward<C1>(connex1)
                    ,conR = forward<C2>(connex2)
                    ]
                    (StrView toParse) -> ProductEval
                      {
                        auto eval1 = conL.parse (toParse);
                        if (eval1.result)
                          {
                            size_t posAfter1 = eval1.consumed;
                            StrView restInput = toParse.substr(posAfter1);
                            auto eval2 = conR.parse (restInput);
                            if (eval2.result)
                              {
                                uint consumedOverall = posAfter1 + eval2.consumed;
                                return ProductEval{ProductResult{move(*eval1.result)
                                                                ,move(*eval2.result)}
                                                  ,consumedOverall
                                                  };
                              }
                          }
                        return ProductEval{std::nullopt};
                      }};
    }
    
    
    template<class PAR>
    class Syntax;
    
    
    template<class CON>
    class Parser
      : public CON
      {
        using PFun = typename CON::PFun;
        static_assert (_Fun<PFun>(), "Connex must define a parse-function");
        
      public:
        using Connex = CON;
        using Result = typename CON::Result;
        
using Sigi = typename _Fun<PFun>::Sig;
//lib::test::TypeDebugger<Sigi> buggi;
//lib::test::TypeDebugger<Result> guggi;
  
        static_assert (has_Sig<PFun, Eval<Result>(StrView)>()
                      ,"Signature of the parse-function not suitable");
        
        Eval<Result>
        operator() (StrView toParse)
          {
            return CON::parse (toParse);
          }
        
        template<typename SPEC>
        Parser (SPEC&& spec)
          : CON{buildConnex (forward<SPEC> (spec))}
          { }
        
//      template<class PAR>
//      Parser (Syntax<PAR> const& anchor)
//        : CON{anchor}
//        { }
//      template<class PAR>
//      Parser (CON const& anchor)
//        : CON{anchor}
//        { }
      };
    
    Parser(NullType)      -> Parser<NulP>;
    Parser(regex &&)      -> Parser<Term>;
    Parser(regex const&)  -> Parser<Term>;
    Parser(string const&) -> Parser<Term>;
    
    template<class FUN>
    Parser(Connex<FUN> const&) -> Parser<Connex<FUN>>;
//  
//  template<class PAR>
//  Parser(Syntax<PAR> const&) -> Parser<typename PAR::Connex>;
    
    
    template<class PAR>
    class Syntax
      : public Eval<typename PAR::Result>
      {
        PAR parse_;
        
      public:
        using Connex = typename PAR::Connex;
        using Result = typename PAR::Result;
        
        bool success()    const { return bool(Syntax::result); }
        bool hasResult()  const { return bool(Syntax::result); }
        Result& getResult()     { return * Syntax::result;     }
        Result&& extractResult(){ return move(getResult());    }
        
        Syntax()
          : parse_{NullType()}
          { }
        
        explicit
        Syntax (PAR&& parser)
          : parse_{move (parser)}
          { }
        
        explicit
        operator bool()  const
          {
            return success();
          }
        
        Syntax&&
        parse (StrView toParse)
          {
            eval() = parse_(toParse);
            return move(*this);
          }
        
        Connex const&
        getConny()  const
          {
            return parse_;
          }
        
        template<typename SPEC>
        auto
        seq (SPEC&& clauseDef)
          {
            return accept(
                    sequenceConnex (move(parse_)
                                   ,Parser{forward<SPEC> (clauseDef)}));
          }
        
      private:
        Eval<Result>&
        eval()
          {
            return *this;
          }
      };
  
    template<typename SPEC>
    auto
    accept (SPEC&& clauseDef)
    {
      return Syntax{Parser{forward<SPEC> (clauseDef)}};
    }
    
  //  template<class PAR>
  //  Parser(Syntax<PAR> const&) -> Parser<typename PAR::Connex>;
    
  }// namespace parse
  
  using parse::accept;
  
}// namespace util

namespace lib {
}// namespace lib
#endif/*LIB_PARSE_H*/
