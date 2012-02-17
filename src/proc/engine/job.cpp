/*
  Job  -  render job closure

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "proc/engine/job.hpp"
#include "proc/engine/job-ticket.hpp"


namespace proc {
namespace engine {
  
  namespace { // Details...

    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  /** @todo WIP-WIP 2/12  
   */
  void
  Job::triggerJob (lumiera_jobParameter param)  const
  {
    UNIMPLEMENTED ("how to access the JobTicket and build the RenderInvocation");
  }
  
  
  void
  Job::signalFailure (lumiera_jobParameter)  const
  {
    UNIMPLEMENTED ("how to organise job failure and abortion");
  }

  
  
}} // namespace proc::engine

namespace {
  using proc::engine::Job;
    
  inline Job& 
  forwardInvocation (LumieraJobClosure jobFunctor)
  {
    Job* job = static_cast<Job*> (jobFunctor);
    
    REQUIRE (job);
    REQUIRE (job->isValid());
    return *job;
  }
}


extern "C" { /* ==== implementation C interface for job invocation ======= */
  
void
lumiera_job_invoke  (LumieraJobClosure jobFunctor, lumiera_jobParameter param)
{
  forwardInvocation(jobFunctor).triggerJob (param);
}

void
lumiera_job_failure (LumieraJobClosure jobFunctor, lumiera_jobParameter param)
{
  forwardInvocation(jobFunctor).signalFailure (param);
}
  
  
}
