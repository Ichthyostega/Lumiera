/*
  NOP-JOB-FUNCTOR.hpp  -  a render job to do nothing

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file nop-job-functor.hpp
 ** Generic implementation of a JobFunctor to perform no calculations.
 ** Shall be used whenever playback or rendering must be implemented
 ** over a part of the effective timeline with no model at all.
 ** @todo WIP draft for the »Playback Vertical Slice« 4/2023
 ** @warning not clear if this solution is feasible   ///////////////////////////////////////////////////////TICKET #1296 : do we need to produce "empty output" explicitly?
 ** 
 */


#ifndef VAULT_GEAR_NOP_JOB_FUNCTOR_H
#define VAULT_GEAR_NOP_JOB_FUNCTOR_H



#include "lib/hash-standard.hpp"
#include "lib/hash-combine.hpp"
#include "lib/time/timevalue.hpp"
#include "vault/gear/job.h"

#include <string>


namespace vault{
namespace gear {
  
  using lib::time::Time;
  using lib::HashVal;
  
  
  /**
   * Stub/Test implementation of the JobFunctor interface
   * for a render job _to do nothing at all_
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
      
      std::string
      diagnostic()  const override
        {
          return "NopJobFunctor";
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
      
    public:
      NopJobFunctor();
    };


}} // namespace vault::gear
#endif /*VAULT_GEAR_NOP_JOB_FUNCTOR_H*/
