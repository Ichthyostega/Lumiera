/*
  ENGINE-OBSERVER.hpp  -  collection and maintenance of engine performance indicators

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file engine-observer.hpp
 ** Render Engine performance data collection service. Data indicative of the
 ** current operational state is emitted at various levels of processing as
 ** synchronous notification calls. The information transmitted must be offloaded
 ** quickly for asynchronous processing to generate the actual observable values.
 ** 
 ** @see scheduler.hpp
 ** @see job-planning.hpp
 ** @see Activity::Verb::WORKSTART
 ** 
 ** @todo WIP-WIP-WIP 10/2023 »Playback Vertical Slice« created as a stub
 ** 
 */


#ifndef SRC_VAULT_GEAR_ENGINE_OBSERVER_H_
#define SRC_VAULT_GEAR_ENGINE_OBSERVER_H_


#include "lib/error.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/scheduler-commutator.hpp"
#include "vault/gear/scheduler-invocation.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Collector and aggregator for performance data.
   * @todo WIP-WIP 10/2023 - stub as placeholder for later development
   * @see Scheduler
   */
  class EngineObserver
    : util::NonCopyable
    {
      
    public:
      explicit
      EngineObserver()
        { }
    };
  
  
  
}}// namespace vault::gear
#endif /*SRC_VAULT_GEAR_ENGINE_OBSERVER_H_*/
