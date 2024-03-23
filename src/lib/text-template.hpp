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
#include "lib/format-util.hpp"///////////////////OOO use format-string??
#include "lib/regex.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>
#include <stack>
#include <map>


namespace lib {
  
  using std::string;
  using StrView = std::string_view;
  
  using util::unConst;
  
  
  namespace {
    
    /** shorthand for an »iter-explorer« build from some source X */
    template<class X>
    using ExploreIter = decltype (lib::explore (std::declval<X>()));
    
    const string MATCH_SINGLE_KEY = "[A-Za-z_]+\\w*";
    const string MATCH_KEY_PATH   = MATCH_SINGLE_KEY+"(?:\\."+MATCH_SINGLE_KEY+")*";
    const string MATCH_LOGIC_TOK  = "if|for";
    const string MATCH_END_TOK    = "end\\s*";
    const string MATCH_ELSE_TOK   = "else";
    const string MATCH_SYNTAX     = "("+MATCH_ELSE_TOK+")|(?:("+MATCH_END_TOK+")?("+MATCH_LOGIC_TOK+")\\s+)?("+MATCH_KEY_PATH+")";
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
        Keyword syntaxCase{ESCAPE};
        StrView lead;
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
                          if (mat[1].matched)
                            return tag;
                          if (mat[5].matched)
                            tag.key = mat[5];
                          if (mat[4].matched)
                            { // detected a logic keyword...
                              if ("if" == mat[4])
                                tag.syntaxCase = mat[5].matched? TagSyntax::END_IF : TagSyntax::IF;
                              else
                              if ("for" == mat[4])
                                tag.syntaxCase = mat[5].matched? TagSyntax::END_FOR : TagSyntax::FOR;
                              else
                                throw error::Logic("unexpected keyword");
                            }
                          else
                          if (mat[3].matched)
                            tag.syntaxCase = TagSyntax::ELSE;
                          else
                            tag.syntaxCase = TagSyntax::KEYID;
                          return tag;
                        };
      
      return explore (util::RegexSearchIter{input, ACCEPT_MARKUP})
                .transform(classify);
    }
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
      
      struct Action
        {
          Code code{TEXT};
          string val{""};
          Idx refIDX{0};
          
          template<class SRC>
          StrView instantiate (InstanceCore<SRC>&)  const;
        };
      
      /** the text template is compiled into a sequence of Actions */
      using ActionSeq = std::vector<Action>;
      

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
          
          void instantiateNext();
          StrView getContent(string key);
        };
      
      template<class DAT>
      using InstanceIter = ExploreIter<InstanceCore<DataSource<DAT>>>;
      
    public:
      TextTemplate(string spec)
        { }
      
      template<class DAT>
      InstanceIter<DAT>
      render (DAT const& data)  const;
      
      template<class DAT>
      static string
      apply (string spec, DAT const& data);
    };
  
  
  
  
  
  /* ======= preconfigured data bindings ======= */
  
  template<class DAT, typename SEL=void>
  struct TextTemplate::DataSource
    {
      static_assert (not sizeof(DAT),
                     "unable to bind this data source "
                     "for TextTemplate instantiation");
    };
  
  using MapS = std::map<string,string>;
  
  template<>
  struct TextTemplate::DataSource<MapS>
    {
      MapS* data_;
      using Iter = std::string_view;
      
      bool
      contains (string key)
        {
          return util::contains (*data_, key);
        }
      
      string const&
      retrieveContent (string key)
        {
          return (*data_)[key];
        }
    };
  
  
  
  
  /* ======= implementation of the instantiation state ======= */
  
  template<class SRC>
  TextTemplate::InstanceCore<SRC>::InstanceCore (TextTemplate::ActionSeq const& actions, SRC s)
    : dataSrc_{s}
    , actionIter_{explore (actions)}
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
    instantiateNext();
  }
  
  template<class SRC>
  inline void
  TextTemplate::InstanceCore<SRC>::instantiateNext()
  {
    rendered_ = actionIter_? actionIter_->instantiate(*this)
                           : StrView{};
  }
  
  template<class SRC>
  inline StrView
  TextTemplate::InstanceCore<SRC>::getContent(string key)
  {
    static StrView nil{""};
    return dataSrc_.contains(key)? dataSrc_.retrieveContent(key) : nil;
  }
  
  
  
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
        return "";
      case JUMP:
        return "";
      case ITER:
        return "";
      case LOOP:
        return "";
      default:
        NOTREACHED ("uncovered Activity verb in activation function.");
      }
  }
  
  
  
  
  /** */
  template<class DAT>
  inline TextTemplate::InstanceIter<DAT>
  TextTemplate::render (DAT const& data)  const
  {
    UNIMPLEMENTED ("actually instantiate the text template");
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
