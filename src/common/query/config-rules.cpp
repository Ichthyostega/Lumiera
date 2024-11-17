
/*
  ConfigRules  -  interface for rule based configuration

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config-rules.cpp
 ** Implementation backend part for a configuration rules / query system.
 ** @todo placeholder and mostly not implemented as of 11/2016
 */


#include "lib/error.hpp"
#include "common/query.hpp"
#include "common/config-rules.hpp"

using lumiera::QueryKey;


namespace lumiera {
  namespace error {
    LUMIERA_ERROR_DEFINE (CAPABILITY_QUERY, "unresolvable capability query");
  }
  namespace query {
    
    
    
  } // namespace query
  
  
  
  
  
  namespace query {
    namespace { // local definitions: implementing a backdoor for tests
        QueryKey fakeBypass;
    } 
    
    void setFakeBypass(QueryKey const& q)  { fakeBypass = q; }
    bool isFakeBypass (QueryKey const& q)  { return q == fakeBypass; }
    /////////////////////////////////////////////////////////////////////////////TICKET 710
    
  }// namespace query
} // namespace lumiera
