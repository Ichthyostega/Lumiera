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
 ** Changes sent by diff serve as a generic meta-representation to keep separate
 ** and different representations of the same logical structure in sync. This allows
 ** for tight cooperation between strictly separated components, without the need
 ** of a fixed, predefined and shared data structure.
 ** 
 ** \par Basic Assumptions
 ** While the \em linearisation folds knowledge about the underlying data structure
 ** down into the actual diff, we deliberately assume that the data to be diffed is
 ** \em structured data. Moreover, we'll assume implicitly that this data is \em typed,
 ** and we'll assume explicitly that the atomic elements in the data structure have a
 ** well-defined identity and can be compared with the \c == operator. We treat those
 ** elements as values, which can be copied and moved cheaply. We include a copy of
 ** all content elements right within the tokens of the diff language, either to
 ** send the actual content data this way, or to serve as redundancy to verify
 ** proper application of the changes at the diff receiver.
 ** 
 ** \par Solution Pattern
 ** The representation of this linearised diff language relies on a specialised form
 ** of the <b>visitor pattern</b>: We assume the vocabulary of the diff language to be
 ** relatively fixed, while the actual effect when consuming the stream of diff tokens
 ** is provided as a private detail of the receiver, implemented as a concrete "Interpreter"
 ** (visitor) of the specific diff language flavour in use. Thus, our implementation relies
 ** on \em double-dispatch, based both on the type of the individual diff tokens and on the
 ** concrete implementation of the Interpreter. The typical usage will employ an DiffApplicator,
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
 ** in according to that rule). Such a combination of verb and data argument is called
 ** a DiffStep, since it represents a single step in the process of describing changes
 ** or transforming a data structure. For example, a list diff language can be built
 ** using the following four verbs:
 ** - pick-next
 ** - insert-new
 ** - delete-next
 ** - push-back-next
 ** 
 ** @see list-diff-application.hpp
 ** @see diff-list-application-test.cpp
 ** @see VerbToken
 ** 
 */


#ifndef LIB_DIFF_DIFF_LANGUAGE_H
#define LIB_DIFF_DIFF_LANGUAGE_H


#include "lib/error.hpp"
#include "lib/verb-token.hpp"

#include <boost/noncopyable.hpp>
#include <tuple>



namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  LUMIERA_ERROR_DECLARE(DIFF_CONFLICT); ///< Collision in diff application: contents of target not as expected.
  
  
  /**
   * Definition frame for a language to describe differences in data structures.
   * We use a \em linearised representation as a sequence of DiffStep messages
   * of constant size. The actual verbs of the diff language in use are defined
   * through the operations of the \em Interpreter; each #VerbToken corresponds
   * to a handler function on the Interpreter interface. In addition to the verb,
   * each DiffStep also carries an content data element, like e.g. "insert elm
   * at next position".
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
  
  
  
  
  
  
  
  
  
  
  /* ==== Implementation Pattern for Diff Application ==== */
  
  /**
   * Extension point: define how a specific diff language
   * can be applied to elements in a concrete container
   * @remarks the actual diff fed to the DiffApplicator
   *          assumes that this DiffApplicationStrategy is
   *          an Interpreter for the given diff language.
   * @warning the actual language remains unspecified;
   *          it is picked from the visible context.
   */
  template<class CON>
  class DiffApplicationStrategy;
  
  
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
      using Interpreter = DiffApplicationStrategy<SEQ>;
      
      Interpreter target_;
      
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
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_DIFF_LANGUAGE_H*/