/*
  ConfigRules  -  interface for rule based configuration
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "common/configrules.hpp"
#include "common/query/fake-configrules.hpp"
//#include "lib/util.hpp"
#include "include/nobugcfg.h"



namespace lumiera
  {
  
  namespace query
    {
    
    LUMIERA_ERROR_DEFINE (CAPABILITY_QUERY, "unresolvable capability query");

    
      
  } // namespace query

  namespace {
  
    /** type of the actual ConfigRules implementation to use */
    singleton::UseSubclass<query::MockConfigRules> typeinfo;
  }
  
      
  /** Singleton factory instance, parametrised to actual impl. type. */
  SingletonSub<ConfigRules> ConfigRules::instance (typeinfo); 


  
  
    
  namespace query
    {
    namespace // local definitions: implementing a backdoor for tests
      {
        string fakeBypass; 
      } 
    
    void setFakeBypass(string const& q)  { fakeBypass = q; }
    bool isFakeBypass (string const& q)  { return q == fakeBypass; }
    
  } // namespace query

    
  
} // namespace lumiera
