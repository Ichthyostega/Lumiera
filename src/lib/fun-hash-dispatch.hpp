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
 ** @see FunHashDispatch_test
 */


#ifndef LIB_SEVERAL_H
#define LIB_SEVERAL_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/meta/function.hpp"
#include "lib/util.hpp"

//#include <boost/functional/hash.hpp>
#include <unordered_map>
//#include <cstddef>
//#include <functional>


namespace lib {
  
  namespace {// implementation details
    
    /** @internal mix-in for self-destruction capabilities
     */
  }//(End)implementation details
  
  
  
  /************************************************//**
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
