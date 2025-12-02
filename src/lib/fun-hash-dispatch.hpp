/*
  FUN-HASH-DISPATCH.hpp  -  enrol and dispatch prepared functors keyed by hash-ID

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file fun-hash-dispatch.hpp
 ** Service to register and dispatch opaque functions.
 ** Under the hood, the implementation is a hash table holding function pointers.
 ** An instance is thus always tied to one specific function signature. Yet due
 ** to the implicit conversion, simple capture-less λ can be attached as well.
 ** 
 ** The purpose for such a setup is to provide a simple per-signature backend for
 ** some advanced registration scheme involving specific function patterns. The
 ** hash-IDs may be tied to target properties, which sometimes allows to limit
 ** the number of actual functions in the dispatcher tables and can thus be
 ** superior to a classic OO interface when subclasses would be templated.
 ** @see FunHashDispatch_test
 ** @see proc-id.hpp "usage example"
 */


#ifndef LIB_SEVERAL_H
#define LIB_SEVERAL_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/util.hpp"

#include <unordered_map>


namespace lib {
  
  
  /**
   * Dispatcher-table for state-less functions with a given signature.
   * Entries are keyed by hashID and can not be changed, once entered.
   */
  template<class SIG>
  class FunHashDispatch
    : util::MoveOnly
    {
      using DispatchTab = std::unordered_map<HashVal, SIG*>;
      DispatchTab dispatchTab_{};
      
    public:
      SIG*
      enrol (HashVal key, SIG* fun)
        {
          auto [entry,_] = dispatchTab_.emplace (key,fun);
          return entry->second;
        }
      
      bool
      contains (HashVal key)  const
        {
          return util::contains (dispatchTab_, key);
        }
      
      /** retrieve entry, which can be invoked directly */
      SIG*
      select (HashVal key)
        {
          auto it = dispatchTab_.find (key);
          if (it == dispatchTab_.end())
            throw lumiera::error::Logic{"Expect function for given hash to be previously enrolled."};
          else
            return it->second;
        }
    };
  
  
  
} // namespace lib
#endif
