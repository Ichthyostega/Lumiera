/*
  WorkForce.hpp  -  actively coordinated pool of workers for rendering

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

* *****************************************************/

/** @file work-force.cpp
 ** Implementation of render worker coordination.
 ** 
 ** @todo WIP-WIP 11/2023 »Playback Vertical Slice«
 ** 
 */


#include "vault/gear/work-force.hpp"
#include "lib/util.hpp"


namespace vault{
namespace gear {
  
  namespace { // internal details
    
    size_t MINIMAL_CONCURRENCY = 2;
    
  } // internal details
  
  
  
  /**
   * Nominal »full size« of a pool of concurrent workers.
   * This value is [initialised](\ref Config::getDefaultComputationCapacity)
   * to use all available concurrent computing cores, but can be adjusted.
   * Adjustments should be done before a worker pool scales up.
   * @warning this value is taken as-is; setting it to zero will disable
   *          many (but not all) aspects of concurrent processing.
   */
  size_t work::Config::COMPUTATION_CAPACITY = Config::getDefaultComputationCapacity();
  
  /**
   * default value for full computing capacity is to use all (virtual) cores.
   */
  size_t
  work::Config::getDefaultComputationCapacity()
  {
    return util::max (std::thread::hardware_concurrency()
                     , MINIMAL_CONCURRENCY);
  }
  
  
  /**
   * This is part of the weak level of anti-contention measures.
   * When a worker is kicked out from processing due to contention, the immediate
   * reaction is to try again; if this happens repeatedly however, increasingly strong
   * delays are interspersed. Within the _weak zone,_ a short spinning wait is performed,
   * and then the thread requests a `yield()` from the OS scheduler; this cycle is repeated. 
   */
  void
  work::performRandomisedSpin (size_t stepping, size_t randFact)
  {
    size_t degree = CONTEND_SOFT_FACTOR * (1+randFact) * stepping;
    for (volatile size_t i=0; i<degree; ++i) {/*SPIN*/}
  }
  
  /**
   * Calculate the delay time for a stronger anti-contention wait.
   * If the contention lasts, the worker must back out temporarily to allow other workers
   * to catch up. The delay time is stepped up quickly up to a saturation level, where the
   * worker sleeps in the microseconds range — this level is chosen as a balance between
   * retaining some reactivity vs not incurring additional load. The stepping of the
   * anti-contention measures is »sticky« to some degree, because it is not set to
   * zero, once contention ends, but rather stepped down gradually.
   */
  microseconds
  work::steppedRandDelay (size_t stepping, size_t randFact)
  {
    REQUIRE (stepping > 0);
    uint factor = 1u << (stepping-1);
    return (CONTEND_WAIT + 10us*randFact) * factor;
  }


}} // namespace vault::gear
