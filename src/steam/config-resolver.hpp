/*
  CONFIG-RESOLVER.hpp  -  concrete setup for rule based configuration

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
    2012

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file config-resolver.hpp
 ** Definition of the concrete frontend for rule based configuration within the session.
 ** 
 ** @remarks This code will act as a hub to pull in, instrument and activate a lot of further code.
 **          All the types mentioned in the steam::InterfaceTypes typelist will be prepared to be used
 **          in rules based setup and configuration; this definition will drive the generation of
 **          all the necessary bindings and registration entries to make this work. This is in
 **          accordance with the principle of *generic programming*: Instead of making things
 **          uniform, we use related things in a similar manner.
 ** @note this is placeholder code using a preliminary/mock implementation... don't take this code too literal!
 ** @todo clarify the relation of config query and query-for-defaults   ///////////////TICKET #705
 ** 
 ** @see lumiera::Query
 ** @see mobject::session::DefsManager
 ** @see asset::StructFactory
 ** @see fake-configrules.hpp currently used dummy-implementation
 ** @see 
 **
 */


#ifndef STEAM_CONFIG_RESOLVER
#define STEAM_CONFIG_RESOLVER

#include "common/query.hpp"
#include "common/config-rules.hpp"
#include "lib/depend.hpp"

// types for explicit specialisations....
#include "steam/mobject/session/fork.hpp"
#include "steam/asset/procpatt.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"



namespace steam {
  
  
  
  
  /* ============= global configuration ==================== */
  
  /**
   *  the list of all concrete types participating in the
   *  rule based config query system
   */
  typedef lib::meta::Types < steam::mobject::session::Fork
                           , steam::asset::Pipe
                           , const steam::asset::ProcPatt
                           , steam::asset::Timeline
                           , steam::asset::Sequence
                           > ::List
                             InterfaceTypes;
  
  /**
   * user-visible Interface to the ConfigRules subsystem.
   * Configured as Singleton (with hidden Implementation class)
   */
  class ConfigResolver
    : public lumiera::query::ConfigRules<InterfaceTypes>
    {
  
    public:
      static lib::Depend<ConfigResolver> instance;
      
    };
  
  
} // namespace steam
#endif
