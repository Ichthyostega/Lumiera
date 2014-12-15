/*
  LIST-DIFF-APPLICATION.hpp  -  language to describe differences in linearised form

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file list-diff-application.hpp
 ** Fundamental definitions for a representation of changes.
 ** 
 ** @see diff-list-application-test.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_LIST_DIFF_APPLICATION_H
#define LIB_DIFF_LIST_DIFF_APPLICATION_H


#include "lib/test/run.hpp"
#include "lib/verb-token.hpp"
#include "lib/util.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"

#include <boost/noncopyable.hpp>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <tuple>

using util::isnil;
using std::string;
using util::_Fmt;
using std::vector;
using std::move;


namespace lib {
namespace test{
  namespace error = lumiera::error;
  
  LUMIERA_ERROR_DEFINE(DIFF_CONFLICT, "Collision in diff application: contents of target not as expected.");
  
  template< class I, typename E>
  struct DiffLanguage
    {
      
      using DiffVerb = VerbToken<I, void(E)>;
      using VerbTok = std::tuple<DiffVerb, E>;
      
      struct DiffStep
        : VerbTok
        {
          DiffVerb& verb() { return std::get<0>(*this); }
          E         elm()  { return std::get<1>(*this); }
          
          DiffStep(DiffVerb verb, E e)
            : VerbTok(verb,e)
            { }
          
          operator string()  const
            {
              return string(verb()) + "("+string(elm())+")";
            }
          
          void
          applyTo (I& interpreter)
            {
              verb().applyTo (interpreter, elm());
            }
        };
    };
  
  template<class I, typename E>
  using HandlerFun = void (I::*) (E);
  
  
  template<typename SIG_HANDLER>
  struct DiffStepBuilder;
  
  /** generator to produce specific language tokens */
  template<class I, typename E>
  struct DiffStepBuilder<HandlerFun<I,E>>
    {
      using Lang = DiffLanguage<I,E>;
      using Step = typename Lang::DiffStep;
      using Verb = typename Lang::DiffVerb;
      
      HandlerFun<I,E> handler;
      Literal id;
      
      Step
      operator() (E elm)  const
        {
          return { Verb(handler,id), elm };
        }
    };
  
  
  /** set up a diff language token generator,
   *  based on the specific handler function given.
   *  This generator will produce tokens, wrapping  concrete content elements
   *  of type \c E. In the end, the purpose is to send a sequence of such tokens
   *  around, to feed them to a consumer, which implements the \em Interpreter
   *  interface of the diff language. E.g. this consumer might apply the diff.
   */
  template<class H>
  inline DiffStepBuilder<H>
  diffTokenBuilder (H handlerFun, Literal id)
  {
    return { handlerFun, id };
  }
  
/** shortcut to define tokens of the diff language.
 *  Use it to define namespace level function objects, which,
 *  when supplied with an argument value of type \c E, will generate
 *  a specific language token wrapping a copy of this element.
 * @note need a typedef \c Interpreter at usage site
 *       to refer to the actual language interpreter interface;
 *       the template parameters of the Language and the element
 *       type will be picked up from the given member function pointer.
 */
#define DiffStep_CTOR(_ID_) \
  const auto _ID_ = diffTokenBuilder (&Interpreter::_ID_, STRINGIFY(_ID_));
  
  
  template<typename E>
  class ListDiffInterpreter
    {
    public:
      virtual ~ListDiffInterpreter() { } ///< this is an interface
      
      virtual void ins(E e)    =0;
      virtual void del(E e)    =0;
      virtual void pick(E e)   =0;
      virtual void push(E e)   =0;
    };
  
  template<typename E>
  using ListDiffLanguage = DiffLanguage<ListDiffInterpreter<E>, E>;
  
  
  
  
  
  template<class CON>
  class DiffApplicationStrategy;
  
  /**
   * concrete strategy to apply a list diff to a target sequence given as vector.
   * The implementation swaps aside the existing content of the target sequence
   * and then consumes it step by step, while building up the altered content
   * within the previously emptied target vector. Whenever possible, elements
   * are moved directly to the target location.
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @warning behaves only EX_SANE in case of diff application errors,
   *          i.e. only partially modified / rebuilt sequence might be
   *          in the target when diff application is aborted
   */
  template<typename E, typename...ARGS>
  class DiffApplicationStrategy<vector<E,ARGS...>>
    : public ListDiffInterpreter<E>
    {
      using Vec = vector<E,ARGS...>;
      using Iter = typename Vec::iterator;
      
      Vec orig_;
      Vec& seq_;
      Iter pos_;
      
      bool
      end_of_target()
        {
          return pos_ == orig_.end();
        }
      
      void
      __expect_in_target (E const& elm, Literal oper)
        {
          if (end_of_target())
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "no (further) elements in target sequence") % oper % elm
                              , LUMIERA_ERROR_DIFF_CONFLICT);
          if (*pos_ != elm)
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "found element %s on current target position instead")
                                    % oper % elm % *pos_
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_further_elements()
        {
          if (end_of_target())
            throw error::State("Premature end of target sequence; unable to apply diff further."
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_found (E const& elm, Iter const& targetPos)
        {
          if (targetPos == orig_.end())
            throw error::State(_Fmt("Premature end of sequence; unable to locate "
                                    "element %s as reference point in target.") % elm
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      
      /* == Implementation of the diff application primitives == */
      
      void
      ins (E elm)
        {
          seq_.push_back(elm);
        }
      
      void
      del (E elm)
        {
          __expect_in_target(elm, "remove");
          ++pos_;
        }
      
      void
      pick (E elm)
        {
          __expect_in_target(elm, "pick");
          seq_.push_back (move(*pos_));
          ++pos_;
        }
      
      void
      push (E anchor)
        {
          __expect_further_elements();
          E elm(move(*pos_)); // consume current source element
          ++pos_;
          
          // locate the insert position behind the given reference anchor
          Iter insertPos = std::find(pos_, orig_.end(), anchor);
          __expect_found (anchor, insertPos);
          
          // inserting the "pushed back" element behind the found position
          // this might lead to reallocation and thus invalidate the iterators
          auto currIdx = pos_ - orig_.begin();
          orig_.insert (++insertPos, move(elm));
          pos_ = orig_.begin() + currIdx;
        }
      
      
    public:
      explicit
      DiffApplicationStrategy(vector<E>& targetVector)
        : seq_(targetVector)
        , pos_(seq_.begin())
        {
          swap (seq_, orig_);  // pos_ still refers to original input sequence, which has been moved to orig_
          seq_.reserve (targetVector.size() * 120 / 100);    // heuristics for storage pre-allocation
        }
    };
  
  
  /**
   * generic builder to apply a list diff to a given target sequence.
   * The usage pattern is as follows
   * #. construct a DiffApplicator instance, wrapping the target sequence
   * #. feed the list diff (sequence of diff verbs) to the #consume function
   * #. the wrapped target sequence has been altered, to conform to the given diff 
   * @note a suitable DiffApplicationStrategy will be picked, based on the type
   *       of the concrete target sequence given at construction. (Effectively
   *       this means you need a suitable DiffApplicationStrategy specialisation,
   *       e.g. for a target sequence within a vector)
   */
  template<class SEQ>
  class DiffApplicator
    : boost::noncopyable
    {
      using Receiver = DiffApplicationStrategy<SEQ>;
      
      Receiver target_;
      
    public:
      explicit
      DiffApplicator(SEQ& targetSeq)
        : target_(targetSeq)
        { }
      
      template<class DIFF>
      void
      consume (DIFF&& diff)
        {
          for ( ; diff; ++diff )
            diff->applyTo(target_);
        }
    };
  
  namespace {
    template<class CON>
    using ContentSnapshot = iter_stl::IterSnapshot<typename CON::value_type>;
  }
  
  template<class CON>
  inline ContentSnapshot<CON>
  snapshot(CON const& con)
    {
      return ContentSnapshot<CON>(begin(con), end(con));
    }
  
  template<class VAL>
  inline iter_stl::IterSnapshot<VAL>
  snapshot(std::initializer_list<VAL> const&& ili)
    {
      using OnceIter = iter_stl::IterSnapshot<VAL>;
      return OnceIter(begin(ili), end(ili));
    }
  
  namespace {//Test fixture....
    
    using DataSeq = vector<string>;
    
    #define TOK(id) id(STRINGIFY(id))
    
    string TOK(a1), TOK(a2), TOK(a3), TOK(a4), TOK(a5);
    string TOK(b1), TOK(b2), TOK(b3), TOK(b4);
    
    using Interpreter = ListDiffInterpreter<string>;
    using DiffStep = ListDiffLanguage<string>::DiffStep;
    using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
    
    DiffStep_CTOR(ins);
    DiffStep_CTOR(del);
    DiffStep_CTOR(pick);
    DiffStep_CTOR(push);
    
    
    inline DiffSeq
    generateTestDiff()
    {
      return snapshot({del(a1)
                     , del(a2)
                     , ins(b1)
                     , pick(a3)
                     , push(a5)
                     , pick(a5)
                     , ins(b2)
                     , ins(b3)
                     , pick(a4)
                     , ins(b4)
                     });
      
    }
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: a description language for list differences.
   *       The representation is given as a linearised sequence of verb tokens.
   *       This test demonstrates the application of such a diff representation
   *       to a given source list, transforming this list to hold the intended
   *       target list contents.
   *       
   * @see session-structure-mapping-test.cpp
   */
  class DiffListApplication_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          DataSeq src({a1,a2,a3,a4,a5});
          auto diff = generateTestDiff();
          CHECK (!isnil (diff));
          
          DataSeq target = src;
          DiffApplicator<DataSeq> application(target);
          application.consume(diff);
          
          CHECK (isnil (diff));
          CHECK (!isnil (target));
          CHECK (src != target);
          CHECK (target == DataSeq({b1,a3,a5,b2,b3,a4,b4}));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffListApplication_test, "unit common");
  
  
  
}} // namespace lib::test
