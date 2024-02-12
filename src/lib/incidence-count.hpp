/*
  INCIDENCE-COUNT.hpp  -  instrumentation helper to watch possibly multithreaded task activations

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file incidence-count.hpp
 ** Record and evaluate concurrent activations.
 ** This observation and instrumentation helper is typically used to capture
 ** density of incidence and effective concurrency of performance critical tasks.
 ** The overhead per measurement call amounts to one SystemClock invocation plus
 ** some heap memory access, assuming that sufficient memory was pre-allocated
 ** prior to the actual observation phase. Moreover, on first invocation per Thread,
 ** a thread local ID is constructed, thereby incrementing an global atomic counter.
 ** Statistics evaluation is comprised of integrating and sorting the captured
 ** event log, followed by a summation pass.
 ** @see IncidenceCount_test
 ** @see vault::gear::TestChainLoad::ScheduleCtx
 ** @see SchedulerStress_test
 */


#ifndef LIB_INCIDENCE_COUNT_H
#define LIB_INCIDENCE_COUNT_H


//#include "lib/meta/function.hpp"

//#include <utility>


namespace lib {
  
//  using std::forward;
  
  class IncidenceCount
    {
    public:
    };
  
  
} // namespace lib
#endif /*LIB_INCIDENCE_COUNT_H*/
