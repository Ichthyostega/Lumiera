/*
  NOP-JOB-FUNCTOR.hpp  -  a render job to do nothing

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


/** @file nop-job-functor.hpp
 ** Generic implementation of a JobFunctor to perform no calculations.
 ** Shall be used whenever playback or rendering must be implemented
 ** over a part of the effective timeline with no model at all.
 ** @todo WIP draft for the »Playback Vertical Slice« 4/2023
 ** @warning not clear if this solution is feasible   ///////////////////////////////////////////////////////TICKET #1296 : do we need to produce "empty output" explicitly?
 ** 
 */


#ifndef VAULT_ENGINE_NOP_JOB_FUNCTOR_H
#define VAULT_ENGINE_NOP_JOB_FUNCTOR_H



#include "lib/hash-standard.hpp"
#include "vault/engine/job.h"
#include "lib/time/timevalue.hpp"


namespace vault {
namespace engine{
  
  using lib::time::Time;
  using lib::HashVal;
  
  
  /**
   * Stub implementation of the JobFunctor interface
   * for a render job _to do nothing at all_
   * @todo WIP as of 4/2023
   */
  class NopJobFunctor
    : public JobClosure
    {
      
      /* === JobFunctor Interface === */
      
      JobKind
      getJobKind()  const
        {
          return META_JOB;
        }
      
      bool
      verify (Time, InvocationInstanceID)  const
        {
          return true; // always happy
        }
      
      InvocationInstanceID
      buildInstanceID (HashVal)  const override
        {
          return InvocationInstanceID();
        }
      
      size_t
      hashOfInstance (InvocationInstanceID invoKey)  const override
        {
          std::hash<size_t> hashr;
          HashVal res = hashr (invoKey.frameNumber);
          lib::hash::combine (res, hashr (invoKey.part.t));
          return res;
        }
      
      void
      invokeJobOperation (JobParameter)  override
        {
          /* NOP */
        }
      
      void
      signalFailure (JobParameter, JobFailureReason)  override
        {
          ///////////TODO do we ever need to notify anyone? can this even happen?
        }
      
    public:
      NopJobFunctor();
    };


}} // namespace vault::engine
#endif /*VAULT_ENGINE_NOP_JOB_FUNCTOR_H*/
