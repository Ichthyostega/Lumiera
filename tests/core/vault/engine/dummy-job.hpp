/*
  DUMMY-JOB.hpp  -  diagnostic job for unit tests

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dummy-job.hpp
 ** Unit test helper to generate dummy render jobs
 */


#ifndef VAULT_ENGINE_DUMMY_JOB_H
#define VAULT_ENGINE_DUMMY_JOB_H



#include "vault/engine/job.h"
#include "lib/time/timevalue.hpp"


namespace vault{
namespace engine {
  
  using lib::time::Time;
  
  
  /**
   * Test helper: generate test dummy jobs with built-in diagnostics.
   * Each invocation of such a dummy job will be logged internally
   * and can be investigated and verified afterwards.
   */
  struct DummyJob
    {
      static Job build(); ///< uses random job definition values
      static Job build (Time nominalTime, int additionalKey);
      
      static bool was_invoked (Job const& job);
      static Time invocationTime (Job const& job);
      static Time invocationNominalTime (Job const& job);
      static int  invocationAdditionalKey (Job const& job);
      
      static JobClosure& getFunctor();
    };
  

}} // namespace vault::engine
#endif
