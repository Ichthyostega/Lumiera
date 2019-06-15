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
 ** here we allow for pre-binding of arbitrary _handler functions_ on an interface with
 ** together with individual, suitable arguments. Yet similar to the classic visitor, the
 ** _actual receiver_ can be a subclass of the visitor target interface, which causes the
 ** _second_ indirection in the dispatch chain, thus completing a full double-dispatch.
 ** Since the actually distinguishing factor is not so much a type, but a specific operation,
 ** we refer to the delayed invocation handles created by this binding as _verb token_
 ** on a _receiver_ object (which is the concrete visitor).
 ** 
 ** This setup is an extension or derivative of the [generic verb token](\ref verb-token-hpp)
 ** used for the diff system and similar applications; likewise the intended usage is to establish
 ** a language comprised of several abstract actions ("verbs"), but to allow the concrete operation
 ** to be supplied later, at application time, and within a different code context. The most notable
 ** use case is for the drawing of track contents in the user interface, where this pattern allows
 ** the separation of actual drawing code from the nested track controller structure.
 ** 
 ** 
 ** ## implementation technique
 ** 
 ** The actual foundation is quite simple: we store a [member pointer]. Typically, this embedded
 ** pointer-to-member shall be bound to an abstract virtual function on the _visitor interface._
 ** So basically the "verb" boils down to storing an offset into the VTable on the interface.
 ** Later, on invocation, a reference to the actual _receiver_ is passed in, typically a concrete
 ** subclass of the visitor interface. The invocation then combines this receiver reference with
 ** the offset (the member pointer) to invoke the desired virtual function.
 ** 
 ** However, the complications and technicalities arise from the ability to bind arbitrary
 ** function signatures, even together with the actual arguments to use at invocation. Those
 ** function arguments are supplied when creating the "packaged verb", and thus need to be stored
 ** within this package, together with the member-pointer. The result is a _materialised_ and
 ** _delayed_ invocation of an abstract (interface) function, while the actual concrete function
 ** implementation shall be supplied later. Obviously, such a ["verb pack"](\ref VerbPack) has
 ** _value semantics_ -- we want to store it, copy it and pass it along, often even within a
 ** sequence of "verbs". And even more: we do not want to pass "hidden references" and we
 ** do not want to rely on some management service and heap allocations. Rather, each
 ** VerbPack shall be a self-contained value object. Within certain limitations,
 ** this is possible in C++ by using an opaque buffer embedded within the
 ** outer value object; basically the pre-defined buffer size must be
 ** sufficient to hold all possible argument tuples to bind.
 ** 
 ** The actual implementation here relies on two other components from the Lumiera library:
 ** - the lib::VerbToken provides us with the dispatch through a stored member pointer
 ** - the lib::PolymorphicValue allows to embed a subclass within an opaque inline buffer,
 **   just exposing the common interface.
 ** Yet another challenge is the necessity to unpack the argument values from the storage
 ** tuple and pass them to an (at this point abstracted) function with arbitrary signature.
 ** Here we rely on the common implementation technique from [std::apply], here with the
 ** special twist that we don't use a pre-bound function, but rather need to combine the
 ** actual invocation target at the moment of the invocation.
 ** 
 ** [member pointer]: https://en.cppreference.com/w/cpp/language/pointer
 ** [std::apply]: https://en.cppreference.com/w/cpp/utility/apply
 ** 
 ** @see [drawing on the track canvas](\ref body-canvas-widget.cpp)
 ** @see VerbVisitorDispatch_test
 ** 
 */


#ifndef LIB_VERB_VISITOR_H
#define LIB_VERB_VISITOR_H


#include "lib/meta/variadic-helper.hpp"
#include "lib/polymorphic-value.hpp"
#include "lib/verb-token.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <tuple>


namespace lib {
  
  namespace {
    using JustSomeIrrelvantType = struct{};
    const size_t VERB_TOKEN_SIZE = sizeof(VerbToken<JustSomeIrrelvantType, void(void)>);
    
    constexpr size_t
    storageOverhead(size_t argStorage)
    {
      return argStorage + VERB_TOKEN_SIZE;
    }
  }
  
  
  /** Building block: the Interface to cause the invocation */
  template<class REC, class RET>
  struct VerbInvoker
    : polyvalue::CloneValueSupport<polyvalue::EmptyBase> // mark and mix-in virtual copy construction support
    {
      virtual ~VerbInvoker() { }
      
      virtual RET applyTo (REC&)    =0;
      virtual Literal getID() const =0;
      
      bool operator== (VerbInvoker const& o)  const { return getID() == o.getID(); }
      bool operator!= (VerbInvoker const& o)  const { return getID() != o.getID(); }
    };
  
  
  template<class REC, class SIG>
  struct VerbHolder;
  
  /**
   * Building block: actual storage for a "verb" (function pointer),
   * together with the pre-bound invocation arguments for this specific operation.
   */
  template<class REC, class RET, typename... ARGS>
  struct VerbHolder<REC, RET(ARGS...)>
    : VerbInvoker<REC,RET>
    , VerbToken<REC,RET(ARGS...)>
    {
      using Verb = VerbToken<REC,RET(ARGS...)>;
      using Args = std::tuple<ARGS...>;
      
      /** meta-sequence to pick argument values from the storage tuple */
      using SequenceIterator = typename meta::BuildIdxIter<ARGS...>::Ascending;
      
      /** Storage for the argument tuple */
      Args args_;
      
      template<typename...PARS>
      VerbHolder (typename Verb::Handler handlerRef, Literal verbID, PARS&&... args)
        : Verb{handlerRef, verbID}
        , args_{std::forward<PARS> (args)...}
        { }
      
      Literal
      getID()  const override
        {
          return Verb::getID();
        }
      
      RET
      applyTo (REC& receiver)  override
        {
          return invokeVerb (receiver, SequenceIterator());
        }
      
    private:
      /** @internal actual function invocation, thereby unpacking the argument tuple */
      template<size_t...idx>
      RET
      invokeVerb (REC& receiver, meta::IndexSeq<idx...>)
        {
          return (receiver.*Verb::handler_) (std::get<idx> (args_)...);
        }
    };
  
  
  
  /******************************************************************************************//**
   * A self-contained token to embody a specific yet abstracted operation,
   * together with a concrete set of suitable arguments. The concrete operation
   * is suppled on invocation, when the VerbPack is combined with an actual _receiver_
   * object, implementing the interface `REC` and thus providing the function implementation.
   * VerbPack represents a kind of double-dispatch, flexible both on the actual operation
   * (embodied into the given VerbPack object) and also flexible in the concrete receiver.
   * @tparam REC the "visitor interface" to invoke operations on
   * @tparam RET expected (common) return value of the bound operations (can be `void`)
   * @tparam arg_storage maximum storage size to reserve as buffer for actual function parameters.
   * @remarks
   *   - binding an operation with arguments exceeding `arg_storage` triggers a static assertion
   *   - the resulting VerbPack object has value semantics and is copyable, to the extent any
   *     embedded function arguments are copyable by themselves.
   */
  template<class REC, class RET, size_t arg_storage>
  class VerbPack
    : public PolymorphicValue<VerbInvoker<REC,RET>, storageOverhead(arg_storage)>
    {
      using Dispatcher = VerbInvoker<REC,RET>; // the interface to talk to our payload
      using PolyHolder = PolymorphicValue<Dispatcher, storageOverhead(arg_storage)>;
      
      template<typename FUN>
      struct HandlerTypeDetector
        {
          static_assert (!sizeof(FUN), "handler must be a function member pointer for the given receiver");
        };
      template<typename...ARGS>
      struct HandlerTypeDetector<RET (REC::*) (ARGS...)>
        {
          using Verb    = VerbToken<REC, RET(ARGS...)>;
          using Payload = VerbHolder<REC, RET(ARGS...)>;
        };
      
      template<typename FUN>
      using PayloadType = typename HandlerTypeDetector<FUN>::Payload *;
      
      
    public:
      /** setup a VerbPack for a given operation on the interface `REC`
       * @param handler function member-pointer to define the operation
       * @param verbID  unique ID to designate the token; equality is based
       *                on this `verbID`, all tokens with same ID count as equal
       * @param args arbitrary (yet suitable) arguments to pre-bind and use later
       *                when actually invoking the operation on a concrete receiver
       */
      template<typename FUN, typename...ARGS>
      VerbPack (FUN handler, Literal verbID, ARGS&&... args)
        : PolyHolder(PayloadType<FUN>(), handler, verbID, std::forward<ARGS>(args)...)
        { }
      
      /**
       * Core operation: invoke the operation for this "verb" with the pre-bound parameters
       * @param receiver a subclass of `REC`, providing the function to invoke
       * @return result of performing the actual operation
       */
      RET
      applyTo (REC& receiver)
        {
          return PolyHolder::getPayload().applyTo (receiver);
        }
      
      operator string()  const
        {
          return "VerbPack("
               + string{util::unConst(this)->getPayload().getID()}
               + ")";
        }
      
      /** unsafe downcast and access to an embedded payload argument value */
      template<typename ARG>
      ARG&
      accessArg ()
        {
          using EmbeddedPayload = lib::VerbHolder<REC, RET(ARG)>;
          REQUIRE (INSTANCEOF (EmbeddedPayload, &this->getPayload()));
          EmbeddedPayload& embedded = static_cast<EmbeddedPayload&>(this->getPayload());
          return std::get<0> (embedded.args_);
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_VERB_VISITOR_H*/
