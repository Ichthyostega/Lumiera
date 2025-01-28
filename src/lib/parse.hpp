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
 ** 
 ** So what is provided here is _not a parser library_ — yet aims at »making
 ** simple things simple« and let you implement the complicated ones yourselves.
 ** Several decisions were taken accordingly, like only supporting std::string_view
 ** and automatically consuming any leading whitespace. And notably the focus was
 ** _not placed_ on the challenging aspects of parsing — while still allowing a
 ** pathway towards definition of arbitrarily recursive grammars, if so desired.
 */


#ifndef LIB_PARSE_H
#define LIB_PARSE_H


#include "lib/error.hpp"
#include "lib/branch-case.hpp"
#include "lib/format-string.hpp"
#include "lib/meta/variadic-rebind.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/regex.hpp"
#include "lib/test/diagnostic-output.hpp"/////////TODO

#include <optional>
#include <utility>
#include <cstddef>
#include <tuple>
#include <array>

namespace util {
  namespace parse {
    namespace err = lumiera::error;
    
    using std::move;
    using std::forward;
    using std::optional;
    using lib::meta::_Fun;
    using lib::meta::has_Sig;
    using lib::meta::NullType;
    using lib::meta::_Vari;
    using std::decay_t;
    using std::tuple;
    using std::array;
    using util::_Fmt;
    
    using StrView = std::string_view;
    
    template<class PAR>
    class Syntax;
    
    template<class CON>
    class Parser;
    
    
    
    /**
     * Parse evaluation result
     * @tparam RES model type to bind
     */
    template<class RES>
    struct Eval
      {
        using Result = RES;
        optional<RES> result;
        size_t consumed{0};
      };
    
    /**
     * Building block: parser function
     * definition and connection element.
     */
    template<class FUN>
    struct Connex
      {
        using PFun = FUN;
        PFun parse;
        
        using Result = typename _Fun<PFun>::Ret::Result;
        
        Connex (FUN pFun)
          : parse{pFun}
          { }
      };
    
    /** special setup to be pre-declared and then used recursively */
    template<class RES>
    using OpaqueConnex = Connex<std::function<Eval<RES>(StrView)>>;
    
    template<class RES>
    using ForwardConnex = Connex<std::function<Eval<RES>(StrView)>&>;
    
    
    
    
    
    /** »Null-Connex« which always successfully accepts the empty sequence */
    auto
    buildConnex(NullType)
    {
      return Connex{[](StrView) -> Eval<NullType>
                      {
                        return {NullType{}};
                      }};
    }
    using NulP = decltype(buildConnex (NullType()));
    
    
    /**
     * Foundation: build a \ref Connex to accept a _terminal symbol._
     * the actual parsing is delegated to a Regular Expression,
     * which must match against the _beginning_ of the input sequence,
     * possibly after skipping some whitespace. The defined parser
     * returns an \ref Eval context, to hold a _Result Model_ and
     * the number of characters matched by this terminal symbol.
     */
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
    
    /** build from a string with Regular-Epression spec */
    Term
    buildConnex (string const& rexDef)
    {
      return buildConnex (regex{rexDef});
    }
    
    /** copy-builder from an existing parser function */
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
    
    template<class PAR>
    auto
    buildConnex (Syntax<PAR> const& anchor)
    {
      using Con = typename Syntax<PAR>::Connex;
      return Con{anchor};
    }
    
    /** special setup to attach to a pre-defined clause for recursive syntax
     * @note works in concert with the Parser deduction guide, so that the
     *       resulting ForwardConnex holds a _reference_ to std::function,
     *       and thus gets to see the full definition reassigned later. */
    template<class RES>
    auto
    buildConnex (Syntax<Parser<OpaqueConnex<RES>>> & refClause)
    {
      OpaqueConnex<RES>& refConnex = refClause;
      return ForwardConnex<RES>{refConnex.parse};
    }
    
    
    
    namespace {
      /** helper to detect return type of a possibly generic λ */
      template<typename ARG, typename FUN, typename SEL =void>
      struct _ProbeFunReturn
        {
          static_assert (!sizeof(ARG),
                         "Model binding must accept preceding model result.");
        };
      
      template<typename ARG, typename FUN>
      struct _ProbeFunReturn<ARG,FUN, std::void_t<decltype(std::declval<FUN>() (std::declval<ARG>()))>>
        {                                       // probe the λ with ARG to force template instantiation
          using Ret = decltype(std::declval<FUN>() (std::declval<ARG>()));
        };
      
      
      template<class FUN>
      inline bool
      _boundFun(FUN const& fun)
      {
        if constexpr (std::is_constructible<bool, FUN const&>())
          return bool(fun);
        else
          return std::is_invocable<FUN, StrView>();
      }
    }
    
    /**
     * Adapt by applying a result-transforming function after a successful parse.
     * @remark the purpose is to extract a custom data model immediately from the
     *         result; binding functors can be applied at any level of a Syntax,
     *         and thus the parse can be configured to produce custom result data.
     */
    template<class CON, class BIND>
    auto
    adaptConnex (CON&& connex, BIND&& modelBinding)
    {
      using RX = typename CON::Result;
      using Arg = std::add_rvalue_reference_t<RX>;
      using AdaptedRes = typename _ProbeFunReturn<Arg,BIND>::Ret;
      return Connex{[origConnex = forward<CON>(connex)
                    ,binding = forward<BIND>(modelBinding)
                    ]
                    (StrView toParse) -> Eval<AdaptedRes>
                      {
                        auto eval = origConnex.parse (toParse);
                        if (eval.result)
                          return {binding (move (*eval.result))
                                 ,eval.consumed};
                        else
                          return {std::nullopt};
                      }};
    }
    
    template<class CON>
    auto
    toStringConnex (CON&& connex, uint part)
    {
      using Result = typename CON::Result;
      return Connex([baseConnex = forward<CON>(connex)
                    ,part
                    ]
                    (StrView toParse) -> Eval<string>
                      {
                        auto eval = baseConnex.parse (toParse);
                        if (eval.result)
                          if constexpr (lib::meta::is_basically<Result,smatch>())
                              return {eval.result->str(part)
                                     ,eval.consumed
                                     };
                          else
                            { // defensive fall-back: ignore model, return accepted input part
                              size_t pre = leadingWhitespace (toParse);
                              return {string{toParse.substr (pre, eval.consumed)}
                                     ,eval.consumed
                                     };
                            }
                        else
                          return {std::nullopt};
                      });
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
        static constexpr size_t N = sizeof...(RESULTS);
        using Seq = lib::meta::TySeq<RESULTS...>;
        using Tup = std::tuple<RESULTS...>;
        
        SeqModel() = default;
        
        template<typename...XS, typename XX>
        SeqModel (SeqModel<XS...>&& seq, XX&& extraElm)
          : Tup{std::tuple_cat (seq.extractTuple()
                               ,std::make_tuple (forward<XX> (extraElm)) )}
          { }
          
        template<typename X1, typename X2>
        SeqModel (X1&& res1, X2&& res2)
          : Tup{move(res1), move(res2)}
          { }
        
        Tup&& extractTuple() { return move(*this); }
        
        template<size_t i>
        auto get() { return std::get<i> (*this); }
      };
    
    
    /**
     * Sum Model : results from a disjunction of parsing clauses,
     * which are are tested and accepted as alternatives, one at least.
     */
    template<typename...CASES>
    struct AltModel
      : lib::BranchCase<CASES...>
      {
        using Alt = lib::BranchCase<CASES...>;
        static constexpr size_t N = sizeof...(CASES);
        
        template<typename EXTRA>
        using Additionally = AltModel<CASES...,EXTRA>;
        
        template<typename EXTRA>
        Additionally<EXTRA>
        addBranch()      ///< mark-up existing model to add a further branch-case
          {
            Additionally<EXTRA>& upFaked = reinterpret_cast<Additionally<EXTRA>&> (*this);
            return {move (upFaked)};
          }           // this trick works due to similar storage layout
        
        
        /* === Builder functions to mark which side of the combinator to pick === */
        
        using SubSeq = typename _Vari<AltModel, CASES...>::Prefix;  ///< a nested sub-model to extend
        using Penult = typename _Vari<AltModel, CASES...>::Penult;  ///< plain value expected for left-branch
        using Ultima = typename _Vari<AltModel, CASES...>::Ultima;  ///< plain value expected for right-branch
        
        static AltModel
        mark_left (SubSeq&& leftCases)
          {
            return {leftCases.template addBranch<Ultima>()};
          }
        
        static AltModel
        mark_left (Penult&& leftCase)
          {
            return {Alt::TOP-1, move(leftCase)};
          }
        
        static AltModel
        mark_right (Ultima&& rightCase)
          {
            return {Alt::TOP, move(rightCase)};
          }
        
      private:
        template<typename INIT>
        AltModel (size_t branchID, INIT&& init)
          : Alt{branchID, forward<INIT> (init)}
          { }
      };
    
    
    /** Special case Product Model to represent iterative sequence */
    template<typename RES>
    struct IterModel
      : std::vector<RES>
      {
        RES& get (size_t i) { return this->at(i); }
      };
    
    /** Marker-Tag for the result from a sub-expression, not to be joined */
    template<typename RES>
    struct SubModel
      {
        RES model;
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
    
    
    /** accept either one of two alternative parse functions */
    template<class C1, class C2>
    auto
    branchedConnex (C1&& connex1, C2&& connex2)
    {
      using R1 = typename decay_t<C1>::Result;
      using R2 = typename decay_t<C2>::Result;
      using SumResult = typename _Join<AltModel, R1, R2>::Result;
      using SumEval = Eval<SumResult>;
      return Connex{[conL = forward<C1>(connex1)
                    ,conR = forward<C2>(connex2)
                    ]
                    (StrView toParse) -> SumEval
                      {
                        auto eval1 = conL.parse (toParse);
                        if (eval1.result)
                          {
                            uint endBranch1 = eval1.consumed;
                            return SumEval{SumResult::mark_left (move(*eval1.result))
                                          ,endBranch1
                                          };
                          }
                        auto eval2 = conR.parse (toParse);
                        if (eval2.result)
                          {
                            uint endBranch2 = eval2.consumed;
                            return SumEval{SumResult::mark_right (move(*eval2.result))
                                          ,endBranch2
                                          };
                          }
                        return SumEval{std::nullopt};
                      }};
    }
    
    
    /** repeatedly accept parse-function, optionally delimited. */
    template<class C1, class C2>
    auto
    repeatedConnex (uint min, uint max
                   ,C1&& delimConnex
                   ,C2&& bodyConnex)
    {
      using Res = typename decay_t<C2>::Result;
      using IterResult = IterModel<Res>;
      using IterEval = Eval<IterResult>;
      return Connex{[sep = forward<C1>(delimConnex)
                    ,body = forward<C2>(bodyConnex)
                    ,min,max
                    ]
                    (StrView toParse) -> IterEval
                      {
                        size_t consumed{0};
                        IterResult results;
                        do
                          {
                            uint offset{0};
                            if (not results.empty())
                              {  // look for delimiter within sequence
                                auto delim = sep.parse (toParse);
                                if (not delim.result)
                                  break;
                                offset += delim.consumed;
                              }
                            auto eval = body.parse (toParse.substr(offset));
                            if (not eval.result)
                              break;
                            offset += eval.consumed;
                            results.emplace_back (move(*eval.result));
                            toParse = toParse.substr(offset);
                            consumed += offset;
                          }
                        while (results.size() < max);
                        return results.size() >= min? IterEval{move(results), consumed}
                                                    : IterEval{std::nullopt};
                      }};
    }
    
    
    /** try to accept parse-function, backtracking if not successful. */
    template<class CNX>
    auto
    optionalConnex (CNX&& connex)
    {
      using Res = typename decay_t<CNX>::Result;
      using OptResult = optional<Res>;
      using OptEval = Eval<OptResult>;
      return Connex{[body = forward<CNX>(connex)
                    ]
                    (StrView toParse) -> OptEval
                      {
                        auto eval = body.parse (toParse);
                        size_t consumed{eval.result? eval.consumed : 0};
                        return OptEval{OptResult{eval.result? move(eval.result) : std::nullopt}
                                      ,consumed
                                      };
                      }};
    }
    
    
    /** accept some structure enclosed into a bracketing construct.
     * \param isOptional if the bracketing can be omitted */
    template<class C1, class C2, class C3>
    auto
    bracketedConnex (C1&& openingConnex
                    ,C2&& closingConnex
                    ,C3&& bodyConnex
                    ,bool isOptional)
    {
      using Res = typename decay_t<C3>::Result;
      return Connex{[opening = forward<C1>(openingConnex)
                    ,closing = forward<C2>(closingConnex)
                    ,body    = forward<C3>(bodyConnex)
                    ,isOptional
                    ]
                    (StrView toParse) -> Eval<Res>
                      {
                        auto bracket = opening.parse (toParse);
                        if (bracket.result or isOptional)
                          {
                            size_t consumed = bracket.consumed;
                            bool expectClose{bracket.result};
                            auto eval = body.parse (toParse.substr(consumed));
                            if (eval.result)
                              {
                                consumed += eval.consumed;
                                if (expectClose)
                                  bracket = closing.parse (toParse.substr(consumed));
                                if (bracket.result or not expectClose)
                                  {
                                    consumed += bracket.consumed;
                                    return Eval<Res>{move (*eval.result)
                                                    ,consumed
                                                    };
                                  }
                              }
                          }
                        return Eval<Res>{std::nullopt};
                      }};
    }
    
    
    
    
    /**
     * A Parser function to match and accept some syntax.
     * This is a typing- and interface-adapter, wrapping a Connex.
     */
    template<class CON>
    class Parser
      : public CON
      {
        using PFun = typename CON::PFun;
        static_assert (_Fun<PFun>(), "Connex must define a parse-function");
        
      public:
        using Connex = CON;
        using Result = typename CON::Result;
        
        static_assert (has_Sig<PFun, Eval<Result>(StrView)>()
                      ,"Signature of the parse-function not suitable");
        
        /**
         * Parse-Function operator: test input and yield Eval record
         */
        Eval<Result>
        operator() (StrView toParse)
          {
            REQUIRE (_boundFun (CON::parse), "unbound recursive syntax");
            return CON::parse (toParse);
          }
        
        template<typename SPEC>
        Parser (SPEC&& spec)
          : CON{buildConnex (forward<SPEC> (spec))}
          { }
      };
    
    /* === Deduction guide : how to construct a Parser === */
    Parser(NullType)      -> Parser<NulP>;
    Parser(regex &&)      -> Parser<Term>;
    Parser(regex const&)  -> Parser<Term>;
    Parser(string const&) -> Parser<Term>;
    
    template<class FUN>
    Parser(Connex<FUN>) -> Parser<Connex<FUN>>;
    
    template<class PAR>
    Parser(Syntax<PAR>) -> Parser<typename PAR::Connex>;
    
    template<class RES>
    Parser(Syntax<Parser<OpaqueConnex<RES>>>) -> Parser<ForwardConnex<RES>>;
                                              // bind to recursive syntax by reference
    
    
    /** @internal meta-helper : detect if parser can be built from a given type */
    template<typename SPEC, typename SEL =void>
    struct is_usableSpec : std::false_type{ };
    
    template<typename SPEC>
    struct is_usableSpec<SPEC, std::void_t<decltype(Parser{std::declval<SPEC>()})>>
      : std::true_type
      { };
    
    template<typename SPEC>
    using if_acceptableSpec  = lib::meta::enable_if<is_usableSpec<SPEC>>;
    
    template<typename SPEC1, typename SPEC2>
    using if_acceptableSpecs = lib::meta::enable_if<is_usableSpec<SPEC1>
                                                   ,lib::meta::enable_if<is_usableSpec<SPEC2>>>;
    
    
    
    
    /***********************************************************************//**
     * A Syntax clause with a parser and result state.
     * An instance of this class embodies a (possibly complex)
     * _expected syntactical structure;_ the [parse function](\ref parse)
     * analyses a given input text for compliance with this expected structure.
     * After the parse, result state has been set
     * - indicating if the parse was successful
     * - possibly with an failure message (TODO 1/25)
     * - the number of characters covered by this match
     * - a _Result Model,_ as a structured term holding
     *   result components from each part / sub-clause
     */
    template<class PAR>
    class Syntax
      : public Eval<typename PAR::Result>
      {
        PAR parse_;
        
      public:
        using Connex = typename PAR::Connex;
        using Result = typename PAR::Result;
        
        Syntax()
          : parse_{NullType()}
          { }
        
        explicit
        Syntax (PAR&& parser)
          : parse_{move (parser)}
          { }
        
        explicit
        operator bool()          const { return success();}
        
        operator Connex&()             { return parse_; }
        operator Connex const&() const { return parse_; }
        
        bool success()           const { return bool(Syntax::result);   }
        bool hasResult()         const { return bool(Syntax::result);   }
        bool canInvoke()         const { return _boundFun(parse_.parse);}
        size_t consumed()        const { return Eval<Result>::consumed; }
        Result& getResult()            { return * Syntax::result;       }
        Result&& extractResult()       { return move(getResult());      }
        
        
        /********************************************//**
         * Core API : parse against this syntax clause
         */
        Syntax&&
        parse (StrView toParse)
          {
            eval() = parse_(toParse);
            return move(*this);
          }
        
        
        template<class PX>
        Syntax&
        operator= (Syntax<PX> refSyntax)
          {
            using ConX = typename PX::Connex;
            ConX& refConnex = refSyntax;
            parse_.parse = move(refConnex.parse);
            return *this;
          }
        
        
        /** ===== Syntax clause builder DSL ===== */
        
        template<typename SPEC>
        auto seq (SPEC&& clauseDef);
        
        template<typename SPEC>
        auto alt (SPEC&& clauseDef);
        
        template<typename SPEC>
        auto opt (SPEC&& clauseDef);
        
        template<typename SPEC1, typename SPEC2>
        auto repeat (uint min, uint max, SPEC1&& delimDef, SPEC2&& clauseDef);
        
        template<typename SPEC1, typename SPEC2>
        auto repeat (uint cnt, SPEC1&& delimDef, SPEC2&& clauseDef);
        
        template<typename SPEC1, typename SPEC2>
        auto repeat (SPEC1&& delimDef, SPEC2&& clauseDef);
        
        template<typename SPEC>
        auto repeat (SPEC&& clauseDef);
        
        template<typename SPEC1, typename SPEC2, typename SPEC3>
        auto bracket (SPEC1&& openDef, SPEC2&& closeDef, SPEC3&& bodyDef);
        
        template<typename SPEC>
        auto bracket (string bracketSpec, SPEC&& bodyDef);
        
        template<typename SPEC>
        auto bracket (SPEC&& bodyDef);
        
        template<typename SPEC>
        auto bracketOpt (string bracketSpec, SPEC&& bodyDef);
        
        template<typename SPEC>
        auto bracketOpt (SPEC&& bodyDef);
        
        template<class FUN>
        auto bind (FUN&& modelAdapt);
        
        auto bindMatch (uint group =0);
        
      private:
        Eval<Result>& eval() { return *this;}
      };
    
    
    
    
    /** ===== Syntax clause builder DSL ===== */
    
    /** build a Syntax clause from anything usable as parser-spec. */
    template<typename SPEC>
    auto
    accept (SPEC&& clauseDef)
    {
      return Syntax{Parser{forward<SPEC> (clauseDef)}};
    }
    
    /** empty Syntax clause to start further definition */
    auto accept() { return Syntax<Parser<NulP>>{}; }
    
    
    /** start Syntax clause with an optional syntax part */ 
    template<typename SPEC>
    auto
    accept_opt (SPEC&& clauseDef)
    {
      return accept(
              optionalConnex (Parser{forward<SPEC> (clauseDef)}));
    }
    
    
    /**
     * Start Syntax clause with a repeated sub-clause,
     * with separator and repetition limit; repetitions ∊ [min..max]
     * The separator will be expected _between_ instances of the repeated sub-clause
     * and will by itself produce no model. The result model is an instance of \ref IterModel,
     * which implies it is a vector (uses heap storage); if min ≡ 0, the model can be empty. 
     */ 
    template<typename SPEC1, typename SPEC2>
    auto
    accept_repeated (uint min, uint max, SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        if (max<min)
          throw err::Invalid{_Fmt{"Invalid repeated syntax-spec: min:%d > max:%d"}
                                                               % min    % max    };
        if (max == 0)
          throw err::Invalid{"Invalid repeat with max ≡ 0 repetitions"};
        
        return accept(
                repeatedConnex (min,max
                               ,Parser{forward<SPEC1> (delimDef)}
                               ,Parser{forward<SPEC2> (clauseDef)}));
      }

    /** \param cnt exact number of repetitions expected */
    template<typename SPEC1, typename SPEC2,                                       typename =if_acceptableSpecs<SPEC1,SPEC2>>
    auto
    accept_repeated (uint cnt, SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        return accept_repeated (cnt,cnt, forward<SPEC1>(delimDef), forward<SPEC2>(clauseDef));
      }
    
    /** start Syntax with an arbitrarily repeated sub-clause, with separator */
    template<typename SPEC1, typename SPEC2,                                       typename =if_acceptableSpecs<SPEC1,SPEC2>>
    auto
    accept_repeated (SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        return accept_repeated (1,uint(-1), forward<SPEC1>(delimDef), forward<SPEC2>(clauseDef));
      }
    
    template<typename SPEC>
    auto
    accept_repeated (uint min, uint max, SPEC&& clauseDef)
      {
        return accept_repeated (min, max, NullType{}, forward<SPEC>(clauseDef));
      }
    
    template<typename SPEC>
    auto
    accept_repeated (uint cnt, SPEC&& clauseDef)
      {
        return accept_repeated (cnt, NullType{}, forward<SPEC>(clauseDef));
      }
    
    template<typename SPEC>
    auto
    accept_repeated (SPEC&& clauseDef)
      {
        return accept_repeated (NullType{}, forward<SPEC>(clauseDef));
      }
    
    /**
     * Start Syntax with a sub-clause enclosed into a _bracketing construct._
     * The »bracket« is defined as syntax for the _open marker_ and _close marker._
     * These are consumed without generating model elements. The parse fails unless
     * the full sequence `open body close` can be matched.
     */
    template<typename SPEC1, typename SPEC2, typename SPEC3>
    auto
    accept_bracket (SPEC1&& openDef, SPEC2&& closeDef, SPEC3&& bodyDef)
      {
        return accept(
                 bracketedConnex (Parser{forward<SPEC1>(openDef) }
                                 ,Parser{forward<SPEC2>(closeDef)}
                                 ,Parser{forward<SPEC3>(bodyDef) }
                                 ,false // bracket mandatory, not optional
                                 ));
      }
    
    /**
     * Start Syntax with a bracketed sub-clause, with given single-char delimiters.
     * \param bracketSpec a 2-char string, e.g. "{}" to expect curly braces.
     */
    template<typename SPEC>
    auto
    accept_bracket (string bracketSpec, SPEC&& bodyDef)
      {
        if (bracketSpec.size() != 2)
          throw err::Invalid{"Bracket spec with opening and closing character expected"};
        return accept(
                 bracketedConnex (Parser{"\\"+bracketSpec.substr(0,1)}
                                 ,Parser{"\\"+bracketSpec.substr(1,1)}
                                 ,Parser{forward<SPEC>(bodyDef) }
                                 ,false // bracket mandatory, not optional
                                 ));
      }
    
    /** Start Syntax with a sub-clause enclosed in parentheses */
    template<typename SPEC>
    auto
    accept_bracket (SPEC&& bodyDef)
      {
        return accept_bracket ("()", forward<SPEC>(bodyDef));
      }
    
    /** Start Syntax with a sub-clause, _optionally_ enclosed into brackets. */
    template<typename SPEC>
    auto
    accept_bracketOpt (string bracketSpec, SPEC&& bodyDef)
      {
        if (bracketSpec.size() != 2)
          throw err::Invalid{"Bracket spec with opening and closing character expected"};
        return accept(
                 bracketedConnex (Parser{"\\"+bracketSpec.substr(0,1)}
                                 ,Parser{"\\"+bracketSpec.substr(1,1)}
                                 ,Parser{forward<SPEC>(bodyDef) }
                                 ,true // bracket optional, can be omitted
                                 ));
      }
    
    template<typename SPEC>
    auto
    accept_bracketOpt (SPEC&& bodyDef)
      {
        return accept_bracketOpt ("()", forward<SPEC>(bodyDef));
      }
    
    
    /**
     * Setup an assignable, recursive Syntax clause, initially empty.
     * @remark this provides the foundation for recursive syntax clauses;
     *         initially, an empty std::function with the pre-declared return
     *         type is embedded. Together with a special Parser deduction guide,
     *         later on a full syntax clause can be built, taking a _reference_
     *         to this function; finally the definition prepared here should be
     *         _re-assigned_ with the fully defined syntax, which is handled
     *         by the assignment operator in class Syntax to re-assign a
     *         working parser function into the std::function holder.
     * @tparam RES the result model type to be expected; it is necessary
     *         to augment the full definition explicitly by a model-binding
     *         to produce this type — which typically also involves writing
     *         actual code to deal with the possibly open structure enable
     *         through a recursive syntax definition
     * @see Parse_test::verify_recursiveSyntax()
     */
    template<typename RES>
    auto
    expectResult()
      {
        return accept (Connex{std::function<Eval<RES>(StrView)>{}});
      }
    
    
    
    /**
     * Combinator: extend this Syntax clause by expecting a further sub-clause
     * behind the part of the input matched by the already defined part of this Syntax.
     * The result model will be a \SeqModel, which essentially is a tuple of the
     * result models of all sequenced parts.
     * @return Syntax clause instance accepting the extended structure.
     * @warning the old syntax is invalidated by moving the parse-function out.
     */
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::seq (SPEC&& clauseDef)
      {
        return accept(
                sequenceConnex (move(parse_)
                               ,Parser{forward<SPEC> (clauseDef)}));
      }
    
    /**
     * Combinator: extend this Syntax by adding an _alternative branch_.
     * So either the already defined part of this Syntax matches the input,
     * or the alternative clause is probed from the start of the input. At least
     * one branch must match for the parse to be successful; however, further
     * branches are not tested after finding a matching branch (short-circuit).
     * The result model is a _Sum Type,_ implemented as a custom variant record
     * of type \ref SubModel. It provides a branch selector field to detect which
     * branch of the syntax did match. And it allows to retrieve the result model
     * of this successful branch — which however requires that the invoking code
     * precisely knows the model type to expect.
     */
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::alt (SPEC&& clauseDef)
      {
        return accept(
                branchedConnex (move(parse_)
                               ,Parser{forward<SPEC> (clauseDef)}));
      }
    
    /**
     * Combinator: extend this Syntax with a further sequenced sub-clause,
     * which however is _only optional_ and the match succeed without it.
     * The result model is (as always for \ref seq ) a tuple; the result
     * from the optional part is packaged into a std::optional.
     */
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::opt (SPEC&& clauseDef)
      {
        return seq (accept_opt (forward<SPEC> (clauseDef)));
      }
    
    /**
     * Combinator: extend this Syntax with a further sequenced sub-clause,
     * which in this case accepts a repeated sequence, with delimiter.
     * @see accept_sequenced()
     */
    template<class PAR>
    template<typename SPEC1, typename SPEC2>
    auto
    Syntax<PAR>::repeat (uint min, uint max, SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        return seq (accept_repeated (min,max
                                    ,forward<SPEC1>(clauseDef)
                                    ,forward<SPEC2>(clauseDef)));
      }
    
    template<class PAR>
    template<typename SPEC1, typename SPEC2>
    auto
    Syntax<PAR>::repeat (uint cnt, SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        return seq (accept_repeated (cnt
                                    ,forward<SPEC1>(clauseDef)
                                    ,forward<SPEC2>(clauseDef)));
      }
    
    template<class PAR>
    template<typename SPEC1, typename SPEC2>
    auto
    Syntax<PAR>::repeat (SPEC1&& delimDef, SPEC2&& clauseDef)
      {
        return seq (accept_repeated (forward<SPEC1>(clauseDef)
                                    ,forward<SPEC2>(clauseDef)));
      }
    
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::repeat (SPEC&& clauseDef)
      {
        return seq (accept_repeated (forward<SPEC>(clauseDef)));
      }
    
    /**
     * Combinator: extend this Syntax with a further sequenced sub-clause in brackets.
     * @see accept_bracket()
     */
    template<class PAR>
    template<typename SPEC1, typename SPEC2, typename SPEC3>
    auto
    Syntax<PAR>::bracket (SPEC1&& openDef, SPEC2&& closeDef, SPEC3&& bodyDef)
      {
        return seq (accept_bracket (forward<SPEC1>(openDef)
                                   ,forward<SPEC2>(closeDef)
                                   ,forward<SPEC3>(bodyDef)));
      }
    
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::bracket (string bracketSpec, SPEC&& bodyDef)
      {
        return seq (accept_bracket (move (bracketSpec)
                                   ,forward<SPEC>(bodyDef)));
      }
    
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::bracket (SPEC&& bodyDef)
      {
        return seq (accept_bracket (forward<SPEC>(bodyDef)));
      }
    
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::bracketOpt (string bracketSpec, SPEC&& bodyDef)
      {
        return seq (accept_bracketOpt (move (bracketSpec)
                                      ,forward<SPEC>(bodyDef)));
      }
    
    template<class PAR>
    template<typename SPEC>
    auto
    Syntax<PAR>::bracketOpt (SPEC&& bodyDef)
      {
        return seq (accept_bracketOpt (forward<SPEC>(bodyDef)));
      }
    
    template<class PAR>
    template<class FUN>
    auto
    Syntax<PAR>::bind (FUN&& modelAdapt)
      {
        return accept(
                 adaptConnex (move(parse_)
                             ,forward<FUN>(modelAdapt)));
      }
    
    template<class PAR>
    auto
    Syntax<PAR>::bindMatch (uint group)
      {
        return accept(
                 toStringConnex (move(parse_), group));
      }
    
  }// namespace parse
  
  
  using parse::accept;
  using parse::accept_opt;
  using parse::accept_repeated;
  
}// namespace util


namespace std { // Specialisation to support C++ »Tuple Protocol« and structured bindings.
  
  /** determine compile-time fixed size of a SeqModel */
  template<typename...ELMS>
  struct tuple_size<util::parse::SeqModel<ELMS...> >
    : tuple_size<typename util::parse::SeqModel<ELMS...>::Tup >
    { };

  /** type of the I-th element of a SeqModel -> based on tuple type */
  template<size_t I, typename...ELMS>
  struct tuple_element<I, util::parse::SeqModel<ELMS...> >
    : tuple_element<I, typename util::parse::SeqModel<ELMS...>::Tup >
    { };
}
#endif/*LIB_PARSE_H*/
