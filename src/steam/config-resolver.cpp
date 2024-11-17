/*
  ConfigResolver  -  concrete setup for rule based configuration

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config-resolver.cpp
 ** Implementation of bindings between Steam-Layer objects and rules-based configuration.
 ** @todo draft from the early design phase of the Lumiera application. As of 2016, nothing
 **       was done on behalf of rules-based configuration, other then preparing the whole architecture
 **       for this capability. So this remains one of the most relevant goals of the Project.
 */


#include "lib/error.hpp"
#include "include/lifecycle.h"
#include "steam/config-resolver.hpp"
#include "steam/mobject/session/query/fake-configrules.hpp"
#include "lib/depend-inject.hpp"

using lumiera::LifecycleHook;
using lumiera::ON_GLOBAL_INIT;


namespace steam {
  namespace {
    
    /**
     * Install the actual ConfigResolver implementation.
     * The ConfigResolver answers queries about configuration and default configuration
     * of various aspects of the session, based on configuration rules.
     * @todo PLANNED: use an embedded Prolog-System or similar rules engine.
     *       For the time being (as of 2008-2018),
     *       we use preconfigured fake answers for some common Config-Queries
     */
    void
    configure_ConfigResolver()
    {
      lib::DependInject<ConfigResolver>::useSingleton<mobject::session::query::MockConfigRules>();
    }

    LifecycleHook schedule_ (ON_GLOBAL_INIT, &configure_ConfigResolver);
  }
  
  
  /** Singleton factory instance, configured with the actual implementation type.
   * @see DefsManager::DefsManager()
   * @see defs-manager-impl.hpp
   */
  lib::Depend<ConfigResolver> ConfigResolver::instance;
  
} // namespace steam








#include "common/query/defs-manager-impl.hpp"



   /***************************************************************/
   /* explicit template instantiations for querying various Types */
   /***************************************************************/

#include "steam/asset/procpatt.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"
#include "steam/mobject/session/fork.hpp"

namespace lumiera{
namespace query  {
  
  using steam::asset::Pipe;
  using steam::asset::PPipe;
  using steam::asset::ProcPatt;
  using steam::asset::PProcPatt;
  using steam::asset::Timeline;
  using steam::asset::PTimeline;
  using steam::asset::Sequence;
  using steam::asset::PSequence;
  
  using steam::mobject::session::Fork;
  using PFork = lib::P<Fork>;
  
  template PPipe       DefsManager::operator() (Query<Pipe>     const&);
  template PProcPatt   DefsManager::operator() (Query<const ProcPatt> const&);
  template PFork       DefsManager::operator() (Query<Fork>     const&);
  template PTimeline   DefsManager::operator() (Query<Timeline> const&);
  template PSequence   DefsManager::operator() (Query<Sequence> const&);
  
  template bool DefsManager::define (PPipe const&, Query<Pipe> const&);
  template bool DefsManager::forget (PPipe const&);
  
  
}} // namespace lumiera::query
