/*
  TEXT-TEMPLATE.hpp  -  minimalistic text substitution engine

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file text-template.hpp
 ** A minimalistic text templating engine with flexible data binding.
 ** Text template instantiation implies the interpretation of a template specification,
 ** which contains literal text with some placeholder tags. This is combined with an actual
 ** data source; the engine needs to retrieve data values as directed by key names extracted
 ** from the placeholders and render and splice them into the placeholder locations. This
 ** process is crucial for code generation, for external tool integration and is also often
 ** used for dynamic web page generation. Several external libraries are available, offering
 ** a host of extended functionality. This library implementation for internal use by the
 ** Lumiera application however attempts to remain focused on the essential functionality,
 ** with only minimal assumptions regarding the data source used for instantiation. Rather
 ** than requiring data to be given in some map, or custom JSON data type, or some special
 ** property-tree or dynamic object type, a _data binding protocol_ is stipulated; this
 ** way, any data type can be attached, given that five generic functions can be implemented
 ** to establish the binding. By default, a pre-defined binding is provided for a STL map
 ** and for Lumiera's »External Tree Description« format based on `Record<GenNode>`.
 ** 
 ** # Template syntax and features
 ** 
 ** TextTemplate is able to substitute simple placeholders by name, it can handle
 ** conditional sections and supports a data iteration construct for a nested scope.
 ** The supported functionality is best explained with an example:
 ** \code
 ** Rendered at ${date}.
 ** ${if critical}
 ** WARNING: critical!
 ** ${else}(routine report)${end if critical}
 ** 
 ** **Participants**
 ** ${for person} - ${name} ${if role}(${role})${end if role}
 ** ${else} _no participants_
 ** ${end for person}
 ** \endcode
 ** This template spec is parsed and preprocessed into an internal representation,
 ** which can then be rendered with any suitable data source.
 ** - the placeholder `${date}` is replaced by a value retrieved with the key "date"
 ** - the conditional section will appear only if a key "critical" is defined
 ** - when the data defines content under the key "person", and this content
 **   can be suitably interpreted as a sequence of sub-scopes, then the »for block«
 **   is instantiated for each entry, using the values retrieved through the keys
 **   "name" and "role". Typically these keys are defined for each sub-scope
 ** - note that `${role}` is enclosed into a conditional section, making it optional
 ** - note that both for conditional sections, and for iteration, an _else branch_
 **   can optionally be defined in the template.
 ** How data is actually accessed and what constitutes a nested scope is obviously
 ** a matter of the actual data binding, which is picked up through a template
 ** specialisation for lib::TextTemplate::DataSource
 ** 
 ** # Implementation notes
 ** 
 ** The template specification is parsed and compiled immediately when constructing
 ** the TextTemplate instance. At this point, syntactical errors, e.g. mismatched
 ** conditional opening and closing tags will be detected and raised as exceptions.
 ** The _compiled template_ is represented as a vector of action tokens, holding the
 ** constant parts as strings in heap memory and marking the positions of placeholders
 ** and block bounds.
 ** 
 ** The actual instantiation is initiated through TextTemplate::render(), which picks
 ** a suitable data binding (causing a compilation failure in case not binding can
 ** be established). This function yields an iterator, which will traverse the
 ** sequence of action tokens precompiled for this template and combine them
 ** with the retrieved data, yielding a std::string_view for each instantiated
 ** chunk of the template. The full result can thus be generated either by
 ** looping, or by invoking util::join() on the provided iterator.
 **  
 ** @todo WIP-WIP-WIP 3/2024
 ** @see TextTemplate_test
 ** @see gnuplot-gen.hpp
 ** @see SchedulerStress_test
 */


#ifndef LIB_TEXT_TEMPLATE_H
#define LIB_TEXT_TEMPLATE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/iter-index.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/regex.hpp"
#include "lib/util.hpp"

#include <optional>
#include <string>
#include <vector>
#include <stack>
#include <map>


namespace lib {
  namespace error = lumiera::error;
  
  using std::optional;
  using std::nullopt;
  using std::string;
  using StrView = std::string_view;
  
  using util::_Fmt;
  using util::isnil;
  using util::unConst;
  
  
  namespace {
    
    /** shorthand for an »iter-explorer« build from some source X */
    template<class X>
    using ExploreIter = decltype (lib::explore (std::declval<X>()));
    
    //-----------Syntax-for-iteration-control-in-map------
    const string MATCH_DATA_TOKEN { R"~(([^,;"\s]*)\s*)~"};
    const string MATCH_DELIMITER  { R"~((?:^|,|;)\s*)~" };
    const regex ACCEPT_DATA_ELM   {MATCH_DELIMITER + MATCH_DATA_TOKEN};
    
    inline auto
    iterNestedKeys (string key, string const& iterDef)
    {
      return explore (util::RegexSearchIter{iterDef, ACCEPT_DATA_ELM})
                .transform ([&](smatch mat){ return key+"."+string{mat[1]}+"."; });
    }

    
    //-----------Syntax-for-TextTemplate-tags--------
    const string MATCH_SINGLE_KEY = "[A-Za-z_]+\\w*";
    const string MATCH_KEY_PATH   = MATCH_SINGLE_KEY+"(?:\\."+MATCH_SINGLE_KEY+")*";
    const string MATCH_LOGIC_TOK  = "if|for";
    const string MATCH_END_TOK    = "end\\s*";
    const string MATCH_ELSE_TOK   = "else";
    const string MATCH_SYNTAX     = "("+MATCH_ELSE_TOK+")|(?:("+MATCH_END_TOK+")?("+MATCH_LOGIC_TOK+")\\s*)?("+MATCH_KEY_PATH+")?";
    const string MATCH_FIELD      = "\\$\\{\\s*(?:"+MATCH_SYNTAX+")\\s*\\}";
    const string MATCH_ESCAPE     = R"~((\\\$))~";
    
    const regex ACCEPT_MARKUP { MATCH_ESCAPE+"|"+MATCH_FIELD
                              , regex::ECMAScript|regex::optimize
                              };
                             // Sub-Matches: 1 = ESCAPE; 2 = ELSE; 3 = END; 4 = LOGIC; 5 = KEY;
    
    struct TagSyntax
      {
        enum Keyword{ ESCAPE
                    , KEYID
                    , IF
                    , END_IF
                    , FOR
                    , END_FOR
                    , ELSE
                    };
        Keyword syntax{ESCAPE};
        StrView lead;
        StrView tail;
        string key;
      };
    
    inline auto
    parse (string const& input)
    {
      auto classify = [rest=StrView(input)]
                      (smatch mat) mutable -> TagSyntax
                        {
                          REQUIRE (not mat.empty());
                          TagSyntax tag;
                          auto restAhead = mat.length() + mat.suffix().length();
                          auto pre = rest.length() - restAhead;
                          tag.lead = rest.substr(0, pre);
                          rest = rest.substr(tag.lead.length());
                          if (mat[5].matched)
                            tag.key = mat[5];
                          if (mat[1].matched)
                            rest = rest.substr(1);  // strip escape
                          else
                            { // not escaped but indeed active field
                              rest = rest.substr(mat.length());
                              if (mat[4].matched)
                                { // detected a logic keyword...
                                  if ("if" == mat[4])
                                    tag.syntax = mat[3].matched? TagSyntax::END_IF : TagSyntax::IF;
                                  else
                                  if ("for" == mat[4])
                                    tag.syntax = mat[3].matched? TagSyntax::END_FOR : TagSyntax::FOR;
                                  else
                                    throw error::Logic{_Fmt{"unexpected keyword \"%s\""} % mat[4]};
                                }
                              else
                              if (mat[2].matched)
                                tag.syntax = TagSyntax::ELSE;
                              else
                              if ("end" == mat[5])
                                throw error::Invalid{_Fmt{"unqualified \"end\" without logic-keyword:"
                                                          " ...%s${end |↯|}"} % tag.lead};
                              else
                                tag.syntax = TagSyntax::KEYID;
                            }
                          tag.tail = rest;
                          return tag;
                        };
      
      return explore (util::RegexSearchIter{input, ACCEPT_MARKUP})
               .transform (classify);
    }
  }
  namespace test {  // declared friend for test access
    class TextTemplate_test;
  }
  
  
  
  
  
  /*****************************************//**
   * Text template substitution engine
   */
  class TextTemplate
    : util::MoveOnly
    {
      enum Clause {
          IF, FOR
        };
      enum Code {
          TEXT, KEY, COND, JUMP, ITER, LOOP
        };
      
      /** cross-references by index number */
      using Idx = size_t;
      
      template<class SRC>
      class InstanceCore;
      
      struct ParseCtx
        {
          Clause clause;
          Idx begin{0};
          Idx after{0};
        };
      using ScopeStack = std::stack<ParseCtx, std::vector<ParseCtx>>;
      
      struct Action
        {
          Code code{TEXT};
          string val{};
          Idx refIDX{0};
          
          template<class SRC>
          StrView instantiate (InstanceCore<SRC>&)  const;
        };
      
      /** the text template is compiled into a sequence of Actions */
      using ActionSeq = std::vector<Action>;
      
      
      /** processor in a parse pipeline — yields sequence of Actions */
      class ActionCompiler;
      
      /** Binding to a specific data source.
       * @note requires partial specialisation */
      template<class DAT, typename SEL=void>
      class DataSource;
      
      template<class SRC>
      class InstanceCore
        {
          using ActionIter = IterIndex<const ActionSeq>;
          using DataCtxIter = typename SRC::Iter;
          using NestedCtx = std::pair<DataCtxIter, SRC>;
          using CtxStack = std::stack<NestedCtx, std::vector<NestedCtx>>;
          
          SRC        dataSrc_;
          ActionIter actionIter_;
          CtxStack   ctxStack_;
          StrView    rendered_;
          
        public:
          InstanceCore (ActionSeq const& actions, SRC);
          
          bool checkPoint() const;
          StrView& yield()  const;
          void iterNext();
          
          StrView instantiateNext();
          StrView reInstatiate (Idx =Idx(-1));
          StrView getContent(string key);
          bool conditional (string key);
          bool openIteration (string key);
          bool loopFurther();
        };
      
      template<class DAT>
      using InstanceIter = ExploreIter<InstanceCore<DataSource<DAT>>>;
      
      ActionSeq actions_;
      
    public:
      TextTemplate(string spec)
        : actions_{compile (spec)}
        { }
      
      template<class DAT>
      InstanceIter<DAT>
      render (DAT const& data)  const;
      
      template<class DAT>
      static string
      apply (string spec, DAT const& data);
      
      static ActionSeq compile (string const&);
      friend class test::TextTemplate_test;
    };
  
  
  
  
  /* ======= Parser / Compiler pipeline ======= */
  
  /**
   * @remarks this is a »custom processing layer«
   *   to be used in an [Iter-Explorer](\ref iter-explorer.hpp)-pipeline.
   *   The source layer (which is assumed to comply to the »State Core« concept),
   *   yields TagSyntax records, one for each match of the ACCEPT_MARKUP reg-exp.
   *   The actual compilation step, which is implemented as pull-processing here,
   *   will emit one or several Action tokens on each match, thereby embedding the
   *   extracted keys and possibly static fill strings. Since the _performance_ allows
   *   for conditionals and iteration, some cross-linking is necessary, based on index
   *   numbers for the actions emitted and coordinated by a stack of bracketing constructs.
   */
  class TextTemplate::ActionCompiler
    {
      ScopeStack scope_{};
      
    public:
      template<class PAR>
      ActionSeq
      buildActions (PAR&& parseIter)
        {
          ActionSeq actions;
          while (parseIter)
              compile (parseIter, actions);
          return actions;
        }
      
    private:
      template<class PAR>
      void
      compile (PAR& parseIter, ActionSeq& actions)
        {
          auto currIDX   = [&]{ return actions.size(); };
          auto valid     = [&](Idx i){ return 0 < i and i < actions.size(); };
          auto clause    = [](Clause c)-> string { return c==IF? "if" : "for"; };
          auto scopeClause = [&]{ return scope_.empty()? "??" : clause(scope_.top().clause); };
          
           //  Support for bracketing constructs (if / for)
          auto beginIdx    = [&]{ return scope_.empty()? 0 : scope_.top().begin; };                          // Index of action where scope was opened
          auto scopeKey    = [&]{ return valid(beginIdx())? actions[beginIdx()].val : "";};                  // Key controlling the if-/for-Scope
          auto keyMatch    = [&]{ return isnil(parseIter->key) or parseIter->key == scopeKey(); };           // Key matches in opening and closing tag
          auto clauseMatch = [&](Clause c){ return not scope_.empty() and scope_.top().clause == c; };       // Kind of closing tag matches innermost scope
          auto scopeMatch  = [&](Clause c){ return clauseMatch(c) and keyMatch(); };
          
          auto lead        = [&]{ return parseIter->lead; };
          auto clashLead   = [&]{ return actions[scope_.top().after - 1].val; };                             // (for diagnostics: lead before a conflicting other "else")
          auto abbrev      = [&](auto s){ return s.length()<16? s : s.substr(s.length()-15); };              // (shorten lead display to 15 chars)
          
           //  Syntax / consistency checks...
          auto __requireKey    = [&](string descr)
                                      {
                                        if (isnil (parseIter->key))
                                          throw error::Invalid{_Fmt{"Tag without key: ...%s${%s |↯|}"}
                                                                   % abbrev(lead()) % descr
                                                              };
                                      };
          auto __checkBalanced = [&](Clause c)
                                      {
                                        if (not scopeMatch(c))
                                          throw error::Invalid{_Fmt{"Unbalanced Logic: expect ${end %s %s}"
                                                                    " -- found ...%s${end |↯|%s %s}"}
                                                                   % scopeClause() % scopeKey()
                                                                   % abbrev(lead())
                                                                   % clause(c) % parseIter->key
                                                              };
                                      };
          auto __checkInScope  = [&]  {
                                        if (scope_.empty())
                                          throw error::Invalid{_Fmt{"Misplaced ...%s|↯|${else}"}
                                                                   % abbrev(lead())};
                                      };
          auto __checkNoDup    = [&]  {
                                        if (scope_.top().after != 0)
                                          throw error::Invalid{_Fmt{"Conflicting ...%s${else} ⟷ ...%s|↯|${else}"}
                                                                   % abbrev(clashLead()) % abbrev(lead())};
                                      };
          
           //  Primitives used for code generation....
          auto add             = [&](Code c, string v){ actions.push_back (Action{c,v});};
          auto addCode         = [&](Code c)  { add (   c, parseIter->key);             };                   // add code token and transfer key picked up by parser
          auto addLead         = [&]          { add (TEXT, string{parseIter->lead});    };                   // add TEXT token to represent the static part before this tag
          auto openScope       = [&](Clause c){ scope_.push (ParseCtx{c, currIDX()});   };                   // start nested scope for bracketing construct (if / for)
          auto closeScope      = [&]          { scope_.pop();                           };                   // close innermost nested scope
          
          auto linkElseToStart = [&]{ actions[beginIdx()].refIDX = currIDX();           };                   // link the start position of the else-branch into opening logic code
          auto markJumpInScope = [&]{ scope_.top().after = currIDX();                   };                   // memorise jump before else-branch for later linkage
          auto linkLoopBack    = [&]{ actions.back().refIDX = scope_.top().begin;       };                   // fill in the back-jump position at loop end
          auto linkJumpToNext  = [&]{ actions[scope_.top().after].refIDX = currIDX();   };                   // link jump to the position after the end of the logic bracket
          
          auto hasElse         = [&]{ return scope_.top().after != 0; };                                     // a jump code to link was only marked if there was an else tag
          
          
          /* === Code Generation === */
          switch (parseIter->syntax) {
            case TagSyntax::ESCAPE:
              addLead();
              break;
            case TagSyntax::KEYID:
              __requireKey("<placeholder>");
              addLead();
              addCode(KEY);
              break;
            case TagSyntax::IF:
              __requireKey("if <conditional>");
              addLead();
              openScope(IF);
              addCode(COND);
              break;
            case TagSyntax::END_IF:
              addLead();
              __checkBalanced(IF);
              if (hasElse())
                linkJumpToNext();
              else
                linkElseToStart();
              closeScope();
              break;
            case TagSyntax::FOR:
              __requireKey("for <data-id>");
              addLead();
              openScope(FOR);
              addCode(ITER);
              break;
            case TagSyntax::END_FOR:
              addLead();
              __checkBalanced(FOR);
              if (hasElse())
                linkJumpToNext();
              else
                { // no else-branch; end active loop here
                  addCode(LOOP);
                  linkLoopBack();
                  linkElseToStart();  // jump behind when iteration turns out empty
                }
              closeScope();
              break;
            case TagSyntax::ELSE:
              addLead();
              __checkInScope();
              __checkNoDup();
              if (IF == scope_.top().clause)
                {
                  markJumpInScope();
                  addCode(JUMP);
                  linkElseToStart();
                }
              else
                {
                  addCode(LOOP);
                  linkLoopBack();
                  markJumpInScope();
                  addCode(JUMP);
                  linkElseToStart();  // jump to else-block when iteration turns out empty
                }
              break;
            default:
              NOTREACHED ("uncovered TagSyntax keyword while compiling a TextTemplate.");
            }
          
          StrView tail = parseIter->tail;
          ++parseIter;
          if (not parseIter)
            add (TEXT, string{tail});
        }     // add final action to supply text after last active tag
    };
  
  inline TextTemplate::ActionSeq
  TextTemplate::compile (string const& spec)
  {
    ActionSeq code = ActionCompiler().buildActions (parse (spec));
    if (isnil (code))
      throw error::Invalid ("TextTemplate spec without active placeholders.");
    return code;
  }
  
  
  
  
  /* ======= preconfigured data bindings ======= */
  
  template<class DAT, typename SEL=void>
  struct TextTemplate::DataSource
    {
      static_assert (not sizeof(DAT),
                     "unable to bind this data source "
                     "for TextTemplate instantiation");
    };
  
  using MapS = std::map<string,string>;
  
  /**
   * Data-binding for a Map-of-strings.
   * Simple keys are retrieved by direct lookup.
   * For the representation of nested data sequences,
   * the following conventions apply
   * - the data sequence itself is represented by an index-key
   * - the value associated to this index-key is a CSV sequence
   * - each element in this sequence defines a key prefix
   * - nested keys are then defined as `<index-key>.<elm-key>.<key>`
   * - when key decoration is enabled for a nested data source, each
   *   lookup for a given key is first tried with the prefix, then as-is.
   * Consequently, all data in the sequence must be present in the original
   * map, stored under the decorated keys.
   * @note multiply nested sequences are _not supported._
   *       While it _is_ possible to have nested loops, the resulting sets
   *       of keys must be disjoint and data must be present in the base map.
   * @see TextTemplate_test::verify_Map_binding()
   */
  template<>
  struct TextTemplate::DataSource<MapS>
    {
      MapS const * data_;
      string keyPrefix_{};
      
      bool isNested() { return not isnil (keyPrefix_); }
      
      
      bool
      contains (string key)
        {
          return (isNested() and util::contains (*data_, keyPrefix_+key))
              or util::contains (*data_, key);
        }
      
      string const&
      retrieveContent (string key)
        {
          MapS::const_iterator elm;
          if (isNested())
            {
              elm = data_->find (keyPrefix_+key);
              if (elm == data_->end())
                elm = data_->find (key);
            }
          else
            elm = data_->find (key);
          ENSURE (elm != data_->end());
          return elm->second;
        }
      
      
      using Iter = decltype(iterNestedKeys("",""));
      
      Iter
      getSequence (string key)
        {
          if (not contains(key))
            return Iter{};
          else
            return iterNestedKeys (key, retrieveContent(key));
        }
      
      DataSource
      openContext (Iter& iter)
        {
          REQUIRE (iter);
          DataSource nested{*this};
          nested.keyPrefix_ = *iter;
          return nested;
        }
    };
  
  
  
  
  /* ======= implementation of the instantiation state ======= */
  
  /**
   * Interpret an action token from the compiled text template
   * based on the given data binding and iteration state to yield a rendering
   * @param instanceIter the wrapped InstanceCore with the actual data binding
   * @return a string-view pointing to the effective rendered chunk corresponding to this action
   */
  template<class SRC>
  inline StrView
  TextTemplate::Action::instantiate (InstanceCore<SRC>& core)  const
  {
    switch (code) {
      case TEXT:
        return val;
      case KEY:
        return core.getContent (val);
      case COND:
        return core.conditional (val)? core.reInstatiate()                  // next is the conditional content
                                     : core.reInstatiate(refIDX);           // points to start of else-block (or after)
      case JUMP:
        return core.reInstatiate(refIDX);
      case ITER:
        return core.openIteration(val)? core.reInstatiate()                 // looping initiated => continue with next
                                      : core.reInstatiate(refIDX);          // points to start of else-block (or after)
      case LOOP:
        return core.loopFurther()     ? core.reInstatiate(refIDX+1)         // start with one after the loop opening
                                      : core.reInstatiate();                // continue with next -> jump over else-block
      default:
        NOTREACHED ("uncovered Activity verb in activation function.");
      }
  }
  
  
  
  template<class SRC>
  TextTemplate::InstanceCore<SRC>::InstanceCore (TextTemplate::ActionSeq const& actions, SRC s)
    : dataSrc_{s}
    , actionIter_{actions}
    , ctxStack_{}
    , rendered_{}
    {
      instantiateNext();
    }
  
  /**
   * TextTemplate instantiation: check point on rendered Action.
   * In active operation, there is a further Action, and this action
   * can be (or has already been) rendered successfully.
   */
  template<class SRC>
  inline bool
  TextTemplate::InstanceCore<SRC>::checkPoint()  const
  {
    return bool(actionIter_);
  }
  
  template<class SRC>
  inline StrView&
  TextTemplate::InstanceCore<SRC>::yield()  const
  {
    return unConst(this)->rendered_;
  }
  
  template<class SRC>
  inline void
  TextTemplate::InstanceCore<SRC>::iterNext()
  {
    ++actionIter_;
    rendered_ = instantiateNext();
  }
  
  
  /** Instantiate next Action token and expose its rendering */
  template<class SRC>
  inline StrView
  TextTemplate::InstanceCore<SRC>::instantiateNext()
  {
    return actionIter_? actionIter_->instantiate(*this)
                      : StrView{};
  }
  
  /**
   * relocate to another Action token and continue instantiation there
   * @param nextCode index number of the next token;
   *        when not given, then iterate one step ahead
   * @return the rendering produced by the selected next Action token
   */
  template<class SRC>
  inline StrView
  TextTemplate::InstanceCore<SRC>::reInstatiate (Idx nextCode)
  {
    if (nextCode == Idx(-1))
      ++actionIter_;
    else
      actionIter_.setIDX (nextCode);
    return instantiateNext();
  }
  
  /** retrieve a data value from the data source for the indiated key */
  template<class SRC>
  inline StrView
  TextTemplate::InstanceCore<SRC>::getContent (string key)
  {
    static StrView nil{""};
    return dataSrc_.contains(key)? dataSrc_.retrieveContent(key) : nil;
  }
  
  /** retrieve a data value for the key and interpret it as boolean expression */
  template<class SRC>
  inline bool
  TextTemplate::InstanceCore<SRC>::conditional (string key)
  {
    return not util::isNo (string{getContent (key)});
  }
  
  /**
   * Attempt to open data sequence by evaluating the entrance key.
   * Data is retrieved for the key and evaluated to produce a collection of
   * data entities to be iterated; each of these will be handled as a data scope
   * nested into the current data scope. This implies to push a #NestedCtx into the
   * #ctxStack_, store aside the current data source and replace it with the new
   * data source for the nested scope. If iteration can not be initiated, all of
   * the initialisation is reverted and the previous scope is reinstated.
   * @return `true` if the nested context exists and the first element is available,
   *         `false` if iteration is not possible and the original context was restored
   */
  template<class SRC>
  inline bool
  TextTemplate::InstanceCore<SRC>::openIteration (string key)
  {
    if (dataSrc_.contains(key))
      if (DataCtxIter dataIter = dataSrc_.getSequence(key))
        {
          ctxStack_.push (NestedCtx{move (dataIter)
                                   ,dataSrc_});
          dataSrc_ = dataSrc_.openContext (ctxStack_.top().first);
          return true;
        }
    return false;
  }
  
  /**
   * Possibly continue the iteration within an already established nested scope.
   * If iteration to the next element is possible, it is expanded into the nested scope,
   * else, when reaching iteration end, the enclosing scope is reinstated
   * @return `true` if the next iterated element is available, `false` after iteration end
   */
  template<class SRC>
  inline bool
  TextTemplate::InstanceCore<SRC>::loopFurther()
  {
    DataCtxIter& dataIter = ctxStack_.top().first;
    ++dataIter;
    if (dataIter)
      {             // open next nested context *from enclosing context*
        dataSrc_ = ctxStack_.top().second.openContext (dataIter);
        return true;
      }
    else
      {         // restore original data context
        std::swap (dataSrc_, ctxStack_.top().second);
        ctxStack_.pop();
        return false;
      }
  }
  
  
  
  
  
  /**
   * Instantiate this (pre-compiled) TextTemplate using the given data binding.
   */
  template<class DAT>
  inline TextTemplate::InstanceIter<DAT>
  TextTemplate::render (DAT const& data)  const
  {
    return explore (InstanceCore{actions_, DataSource<DAT>{&data}});
  }
  
  /** */
  template<class DAT>
  inline string
  TextTemplate::apply (string spec, DAT const& data)
  {
    return util::join (TextTemplate(spec).render (data)
                      ,"");
  }
  
  
  
}// namespace lib
#endif /*LIB_TEXT_TEMPLATE_H*/
