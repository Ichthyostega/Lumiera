/*
  TEST-SCOPE-INVALID.hpp  -  helper for placement scope and scope stack tests

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file test-scope-invalid.hpp
 ** Unit test helper to generate deliberately wrong placement scopes.
 ** Usable to cover error handling related to QueryFocus and ScopePath
 */


#ifndef MOBJECT_SESSION_TEST_SCOPE_INVALID_H
#define MOBJECT_SESSION_TEST_SCOPE_INVALID_H


#include "steam/mobject/placement.hpp"
#include "steam/mobject/session/scope.hpp"



namespace steam {
namespace mobject {
namespace session {
namespace test    {
  
  
  namespace { // nifty subversive test helper...
    
    Scope const&
    fabricate_invalidScope()
    {        /** 
              * assumed to have identical memory layout
              * to a Scope object, as the latter is implemented
              * by a PlacementRef, which in turn is just an
              * encapsulated Placement-ID
              */
             struct Ambush
               {
                 /** random ID assumed to be
                  *  nowhere in the model */
                 PlacementMO::ID derailed_;
               };
      
      static Ambush _shady_scope_;
      return *reinterpret_cast<Scope*> (&_shady_scope_);
    }
  }
  
  
  
}}}} // namespace steam::mobject::session::test
#endif
