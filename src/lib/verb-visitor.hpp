/*
  VERB-VISITOR.hpp  -  double dispatch to arbitrary functions on a common interface

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file verb-visitor.hpp
 ** A specific double dispatch variation for function invocation.
 ** While the classic visitor invokes a common `handle` function with varying arguments,
 ** here we allow for pre-binding of arbitrary functions on an interface with individual
 ** suitable arguments. Yet similar to the classic visitor, the actual receiver can be a
 ** subclass of the target interface, which causes the _second_ indirection in the dispatch
 ** chain. Since the actually distinguishing factor is not so much a type, but a specific
 ** operation, we refer to the delayed invocation handles created by this binding as
 ** _verb token_ on a _receiver_ object (which is the concrete visitor).
 ** 
 ** This setup is an extension or derivative of the [generic verb token](\ref verb-token-hpp)
 ** used for the diff system and similar applications; likewise the intended usage is to establish
 ** a language comprised of several abstract actions ("verbs"), but to allow the concrete operation
 ** to be supplied later, at application time, and within a different code context. The most notable
 ** use case is for the drawing of track contents in the user interface, where this pattern allows
 ** the separation of actual drawing code from the nested track controller structure.
 ** 
 ** @see [drawing on the track canvas](\ref body-canvas-widget.cpp)
 ** @see VerbVisitorDispatch_test
 ** 
 */


#ifndef LIB_VERB_VISITOR_H
#define LIB_VERB_VISITOR_H


#include "lib/meta/variadic-helper.hpp"
#include "lib/polymorphic-value.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <array>
#include <tuple>


namespace lib {
  
  using std::string;
  
  
  /**
   * Action token implemented by double dispatch to a handler function,
   * as defined in the "receiver" interface (parameter \c REC).
   * The token is typically part of a DSL and can be applied
   * to a concrete receiver subclass.
   * @tparam REC the type receiving the verb invocations
   * @tparam SIG signature of the actual verb function, expected
   *             to exist on the receiver (REC) interface
   * @remarks while the included ID Literal is mostly for diagnostics,
   *       it also serves as identity for comparisons. Conceptually what
   *       we want is to compare the function "offset", but this leads
   *       into relying on implementation defined behaviour.
   * @note the #VERB macro simplifies definition of actual tokens
   */
  template<class REC, class SIG>
  class VerbToken;
  
  template<class REC, class RET, typename... ARGS>
  class VerbToken<REC, RET(ARGS...)>
    {
    public:
      typedef RET (REC::*Handler) (ARGS...);
      
    private:
      Handler handler_;
      Literal token_;
      
    public:
      RET
      applyTo (REC& receiver, ARGS&& ...args)
        {
          REQUIRE ("NIL" != token_);
          return (receiver.*handler_)(std::forward<ARGS>(args)...);
        }
      
      VerbToken(Handler handlerFunction, Literal token)
        : handler_(handlerFunction)
        , token_(token)
        { }
      
      VerbToken()
        : handler_{}
        , token_("NIL")
        { }
      
      /* default copyable */
      
      operator string()  const
        {
          return string(token_);
        }
      
      Literal const&
      getID()
        {
          return token_;
        }
      
      /** equality of VerbToken, based on equality of the #token_ Literal
       * @remarks member pointers to virtual functions aren't comparable, for good reason
       */
      bool operator== (VerbToken const& o)  const { return token_ == o.token_; }
      bool operator!= (VerbToken const& o)  const { return token_ != o.token_; }
    };
  
#define VERB(RECEIVER, FUN) VERB_##FUN (&RECEIVER::FUN, STRINGIFY(FUN))
  
  using JustSomeIrrelvantType = struct{};
  const size_t VERB_TOKEN_SIZE = sizeof(VerbToken<JustSomeIrrelvantType, void(void)>);
  
  template<class REC, class RET>
  struct Hook
    {
      virtual ~Hook() { }
      
      virtual RET applyTo (REC&)  =0;
    };
  
  template<class REC, class SIG>
  struct Holder;
  
  template<class REC, class RET, typename... ARGS>
  struct Holder<REC, RET(ARGS...)>
    : polyvalue::CopySupport<                   // mix-in virtual copy/move support
        Hook<REC,RET>>                         //  ...the common interface to use
    {
      using Verb = VerbToken<REC,RET(ARGS...)>;
      using Args = std::tuple<ARGS...>;
      
      /** meta-sequence to pick argument values from the storage tuple */
      using SequenceIterator = typename meta::BuildIdxIter<ARGS...>::Ascending;
      
      Verb verb_;
      Args args_;
      
      Holder (typename Verb::Handler handlerRef, Literal verbID, ARGS... args)
        : verb_{handlerRef, verbID}
        , args_{std::forward<ARGS> (args)...}
        { }
      
      RET
      applyTo (REC& receiver)  override
        {
          return invokeVerb (receiver, SequenceIterator());
        }
      
      template<size_t...idx>
      RET
      invokeVerb (REC& receiver, meta::IndexSeq<idx...>)
        {
          return verb_.applyTo (receiver, std::get<idx> (args_)...);
        }
    };
  
  static constexpr size_t
  storageOverhead(size_t argStorage)
  {
    return argStorage + VERB_TOKEN_SIZE;
  }
  
  
  template<class REC, class RET, size_t arg_storage>
  class VerbPack
    : public PolymorphicValue<Hook<REC,RET>, storageOverhead(arg_storage)>
    {
      using PolyHolder = PolymorphicValue<Hook<REC,RET>, storageOverhead(arg_storage)>;
      
      template<typename...ARGS>
      using PayloadType = Holder<REC, RET(ARGS...)>*;
      
      template<typename...ARGS>
      using Handler = typename VerbToken<REC, RET(ARGS...)>::Handler;
      
    public:
      template<typename...ARGS>
      VerbPack (Handler<ARGS...> handler, Literal verbID, ARGS&&... args)
        : PolyHolder(PayloadType<ARGS...>(), handler, verbID, std::forward<ARGS>(args)...)
        { }
    };
  
  
  
  
  
} // namespace lib  
#endif /*LIB_VERB_VISITOR_H*/
