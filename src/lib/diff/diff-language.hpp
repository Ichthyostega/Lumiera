/*
  DIFF-LANGUAGE.hpp  -  language to describe differences in linearised form

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


/** @file diff-language.hpp
 ** Fundamental definitions for a representation of changes.
 ** We describe differences in data structures or changes to be applied
 ** in the form of a "linearised diff language". Such a diff can be represented
 ** as a sequence of tokens of constant size. Using a linearised constant size
 ** representation allows to process diff generation and diff application in
 ** a pipeline, enabling maximum decoupling of sender and receiver.
 ** Changes sent as diff message serve as a generic meta-representation to keep
 ** separate and different representations of the same logical structure in sync.
 ** Such an architecture allows for tight cooperation between strictly separated
 ** components, without the need of a fixed, predefined and shared data structure.
 ** 
 ** # Basic Assumptions
 ** 
 ** While the \em linearisation folds knowledge about the underlying data structure
 ** down into the actual diff, we deliberately assume that the data to be diffed is
 ** \em structured data. Moreover, we'll assume implicitly that this data is \em typed,
 ** and we'll assume explicitly that the atomic elements in the data structure have a
 ** well-defined identity and can be compared with the \c == operator. We treat those
 ** elements as values, which can be copied and moved cheaply. We include a copy of
 ** all content elements right within the tokens of the diff language, either to
 ** send the actual content data this way, or to serve as redundancy to verify
 ** proper application of the changes at the diff receiver downstream.
 ** 
 ** # Solution Pattern
 ** 
 ** The representation of this linearised diff language relies on a specialised form
 ** of the <b>visitor pattern</b>: We assume the vocabulary of the diff language to be
 ** relatively fixed, while the actual effect when consuming the stream of diff tokens
 ** is provided as a private detail of the receiver, implemented as a concrete "Interpreter"
 ** (visitor) of the specific diff language flavour in use. Thus, our implementation relies
 ** on \em double-dispatch, based both on the type of the individual diff tokens and on the
 ** concrete implementation of the Interpreter. Typical usage will employ a DiffApplicator,
 ** so the "interpretation" of the language means to apply it to a target data structure in
 ** this standard case.
 ** 
 ** Due to the nature of double-dispatch, the interpretation of each token requires two
 ** indirections. The first indirection forwards to a handler function corresponding to the
 ** token, while the second indirection uses the VTable of the concrete Interpreter to pick
 ** the actual implementation of this handler function for this specific case. Basically
 ** the individual token ("verb") in the language is characterised by the handler function
 ** it corresponds to (thus the meaning of a \em verb, an operation). To support diagnostics,
 ** each token also bears a string id. And in addition, each token carries a single data
 ** content element as argument. The idea is, that the "verbs", the handler functions and
 ** the symbolic IDs are named alike (use the macro DiffStep_CTOR to define the tokens
 ** in accordance to that rule). Such a combination of verb and data argument is called
 ** a DiffStep, since it represents a single step in the process of describing changes
 ** or transforming a data structure. For example, a list diff language can be built
 ** using the following four verbs:
 ** - pick-next
 ** - insert-new
 ** - delete-next
 ** - find reordered element
 ** 
 ** @see list-diff-application.hpp
 ** @see diff-list-application-test.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_DIFF_LANGUAGE_H
#define LIB_DIFF_DIFF_LANGUAGE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/verb-token.hpp"
#include "lib/util.hpp"

#include <tuple>


namespace lumiera {
namespace error {
  LUMIERA_ERROR_DECLARE(DIFF_STRUCTURE); ///< Invalid diff structure: implicit rules and assumptions violated.
  LUMIERA_ERROR_DECLARE(DIFF_CONFLICT); ///<  Collision in diff application: contents of target not as expected.
}}

namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  
  template<class I, typename E>
  using HandlerFun = void (I::*) (E const&); // NOTE: element value taken by const&
  
  
  /** @internal type rebinding helper */
  template<class I>
  struct InterpreterScheme               ///< base case is to expect typedef I::Val
    {
      using Interpreter = I;
      using Val = typename I::Val;
      using Handler = HandlerFun<I,Val>;
    };
  
  template<template<typename> class IP, typename E>
  struct InterpreterScheme<IP<E>>        ///< alternatively, the interpreter value type can be templated
    {
      using Val = E;
      using Interpreter = IP<E>;
      using Handler = HandlerFun<Interpreter,Val>;
    };
  
  template<class I, typename E>          ///< alternatively derive value and interpreter from a Handler binding
  struct InterpreterScheme<HandlerFun<I,E>>
    {
      using Val = E;
      using Interpreter = I;
      using Handler = HandlerFun<I,E>;
    };
  
  
  
  /**
   * Definition frame for a language to describe differences in data structures.
   * We use a \em linearised representation as a sequence of DiffStep messages
   * of constant size. The actual verbs of the diff language in use are defined
   * through the operations of the \em Interpreter; each #VerbToken corresponds
   * to a handler function on the Interpreter interface. In addition to the verb,
   * each DiffStep also carries a content data element as argument,
   * like e.g. "insert elm at next position".
   * @param I Interpreter interface of the actual language to use
   * @param E type of the elementary data elements.
   * @remarks recommendation is to set up a builder function for each distinct
   *          kind of verb to be used in the actual language: this #diffTokenBuilder
   *          takes the data element as argument and wraps a copy in the created
   *          DiffStep of the specific kind it is configured for.
   */
  template< class I, typename E>
  struct DiffLanguage
    {
      
      using DiffVerb = VerbToken<I, void(E const&)>; // NOTE: element value taken by const&
      using DiffToken = std::tuple<DiffVerb, E>;
      using Interpreter = I;
      
      struct DiffStep
        : DiffToken
        {
          DiffVerb& verb() { return std::get<0>(*this); }
          E const&  elm()  { return std::get<1>(*this); }
          
          DiffStep(DiffVerb verb, E e)
            : DiffToken(verb,e)
            { }
          
          operator string()  const
            {
              return string(unConst(this)->verb())
               + "("+string(unConst(this)->elm())+")";
            }
          
          void
          applyTo (Interpreter& interpreter)
            {
              TRACE (diff, "verb %4s(%s)", cStr(verb()), cStr(elm()) );
              verb().applyTo (interpreter, elm());
            }
        };
      
      
      static const DiffStep NIL;
    };
  
  
  
  
  
  /** generator to produce specific language tokens */
  template<class I>
  struct DiffStepBuilder
    {
      using Scheme  = InterpreterScheme<I>;
      using Handler = typename Scheme::Handler;
      using Val     = typename Scheme::Val;
      
      using Lang = DiffLanguage<I,Val>;
      using Step = typename Lang::DiffStep;
      using Verb = typename Lang::DiffVerb;
      
      Handler handler;
      Literal id;
      
      Step
      operator() (Val elm)  const
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
  inline DiffStepBuilder<typename InterpreterScheme<H>::Interpreter>
  diffTokenBuilder (H handlerFun, Literal id)
  {
    return { handlerFun, id };
  }
  
  /** shortcut to define tokens of the diff language.
   *  Use it to define namespace or class level function objects, which,
   *  when supplied with an argument value of type \c E, will generate
   *  a specific language token wrapping a copy of this element.
   * @see ListDiffLanguage usage example
   * @note need a typedef \c Interpreter at usage site
   *       to refer to the actual language interpreter interface;
   *       the template parameters of the Language and the element
   *       type will be picked up from the given member function pointer.
   */
#define DiffStep_CTOR(_ID_) \
  const DiffStepBuilder<Interpreter> _ID_ = diffTokenBuilder (&Interpreter::_ID_, STRINGIFY(_ID_));
  
  
  
  /** fixed "invalid" marker token
   * @warning use for internal state marking only --
   *          invoking this token produces undefined behaviour */
  template<class I, typename E>
  const typename DiffLanguage<I,E>::DiffStep DiffLanguage<I,E>::NIL = DiffStep(DiffVerb(), E());
  
  
  
  
  
  
  
  
  
  /* ==== Implementation Pattern for Diff Application ==== */
  
  /**
   * Extension point: define how a specific diff language
   * can be applied to elements in a concrete container
   * @remarks the actual diff fed to the DiffApplicator
   *          assumes that this DiffApplicationStrategy is
   *          an Interpreter for the given diff language.
   * @remarks the second template parameter allows for
   *          `std::enable_if` based on the concrete
   *          target type `TAR` (1st template arg)
   * @warning the actual language remains unspecified;
   *          it is picked from the visible context.
   * @see tree-diff-application.hpp
   * @see list-diff-application.hpp
   */
  template<class TAR, typename SEL =void>
  class DiffApplicationStrategy;
  
  
  /**
   * generic builder to apply a diff description to a given target data structure.
   * The usage pattern is as follows
   * #. construct a DiffApplicator instance, wrapping the target data
   * #. feed the diff (sequence of diff verbs) to the #consume function
   * #. the wrapped target data has been altered, to conform to the given diff
   * @note a suitable DiffApplicationStrategy will be picked, based on the type
   *       of the concrete target sequence given at construction. (Effectively
   *       this means you need a suitable DiffApplicationStrategy specialisation,
   *       e.g. for a target sequence within a vector)
   */
  template<class TAR>
  class DiffApplicator
    : util::NonCopyable
    {
      using Interpreter = DiffApplicationStrategy<TAR>;
      
      Interpreter target_;
      
    public:
      explicit
      DiffApplicator(TAR& targetStructure)
        : target_(targetStructure)
        { }
      
      template<class DIFF>
      void
      consume (DIFF&& diff)
        {
          target_.initDiffApplication();
          for ( ; diff; ++diff )
            diff->applyTo(target_);
          target_.completeDiffApplication();
        }
    };
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_DIFF_LANGUAGE_H*/
