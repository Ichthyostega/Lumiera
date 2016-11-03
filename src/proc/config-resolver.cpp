
/*
  ConfigResolver  -  concrete setup for rule based configuration

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file config-resolver.cpp
 ** TODO config-resolver.cpp
 */


#include "lib/error.hpp"
#include "proc/config-resolver.hpp"
#include "proc/mobject/session/query/fake-configrules.hpp"



namespace proc {
  
  using lib::buildSingleton;
  
  /** Singleton factory instance, configured with the actual implementation type. */
  lib::Depend<ConfigResolver> ConfigResolver::instance (buildSingleton<proc::mobject::session::query::MockConfigRules>());
  
} // namespace proc








#include "common/query/defs-manager-impl.hpp"



   /***************************************************************/
   /* explicit template instantiations for querying various Types */
   /***************************************************************/

#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/mobject/session/fork.hpp"

namespace lumiera{
namespace query  {
  
  using proc::asset::Pipe;
  using proc::asset::PPipe;
  using proc::asset::ProcPatt;
  using proc::asset::PProcPatt;
  using proc::asset::Timeline;
  using proc::asset::PTimeline;
  using proc::asset::Sequence;
  using proc::asset::PSequence;
  
  using proc::mobject::session::Fork;
  using PFork = lib::P<Fork>;
  
  template PPipe       DefsManager::operator() (Query<Pipe>     const&);
  template PProcPatt   DefsManager::operator() (Query<const ProcPatt> const&);
  template PFork       DefsManager::operator() (Query<Fork>     const&);
  template PTimeline   DefsManager::operator() (Query<Timeline> const&);
  template PSequence   DefsManager::operator() (Query<Sequence> const&);
  
  template bool DefsManager::define (PPipe const&, Query<Pipe> const&);
  template bool DefsManager::forget (PPipe const&);
  
  
}} // namespace lumiera::query
