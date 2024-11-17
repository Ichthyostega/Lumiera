/*
  VERB-TOKEN.hpp  -  double dispatch based on DSL tokens

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file verb-token.hpp
 ** Building blocks for a simple DSL using double dispatch to a handler function.
 ** Actually this is a specialised variation of the visitor pattern, where the
 ** "verb tokens" of the language are the domain objects accepting a "receiver"
 ** (visitor) to provide the concrete implementation function for each "verb".
 ** 
 ** The intended usage is to set up a language comprised of several abstract actions ("verbs"),
 ** but to defer the concrete implementation to a specific set of handler functions, which
 ** is provided late, at application time. This way, we can send a sequence of verbs towards
 ** an unknown receiver, which supplies the actual meaning within the target context. In
 ** the end, there is a double-dispatch based both on the individual verb given and the
 ** concrete receiver, which needs to implement the interface used in the definition
 ** of the verb tokens. The handler functions defined within this interface may
 ** take additional arguments, which are passed through on application to
 ** the concrete receiver, e.g. `VERB_doit (receiver, arg1, arg2)`
 ** results in the invocation of \c receiver.doit(arg1,arg2)
 ** 
 ** @see [prominent usage: the Diff system](\ref lib::diff::DiffLanguage)
 ** @see VerbFunctionDispatch_test
 ** 
 */


#ifndef LIB_VERB_TOKEN_H
#define LIB_VERB_TOKEN_H


#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>


namespace lib {
  
  using std::string;
  
  
  /**
   * Action token implemented by double dispatch to a handler function,
   * as defined in the "receiver" interface (parameter `REC`).
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
      
    protected:
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
      getID()  const
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
  
  
  
  
  
} // namespace lib  
#endif /*LIB_VERB_TOKEN_H*/
