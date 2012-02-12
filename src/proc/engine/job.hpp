/*
  JOB.hpp  -  render job closure

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

*/


#ifndef PROC_ENGINE_JOB_H
#define PROC_ENGINE_JOB_H


#include <gavl/gavl.h>


typedef void* LList; ////////////////////////////////////TODO


enum JobState
  { 
    DONE,      ///< already done, nothing to do
    RUNNING,   ///< job is currently running
    WAITING,   ///< waits for some prerequisite resource
    REJECTED,  ///< sorry, can't do that Dave
    EXPIRED,   ///< deadline expired
    ABORTED    ///< got aborted
  };


struct lumiera_jobDescriptor_struct
  {
    gavl_time_t when;
    
    /* == Job prerequisites == */
    LList waiting;
    LList failed;
    LList completed;
  
    JobState jobstate;
  
    void (*jobfn)();
    void (*failfn)();
  };
typedef struct lumiera_JobDescriptor_struct lumiera_jobDescriptor;
typedef lumiera_jobDescriptor* LumieraJobDescriptor;





#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "proc/common.hpp"
//#include "proc/state.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"

#include <tr1/functional>


namespace proc {
namespace engine {
  
//using lib::time::TimeSpan;
//using lib::time::Duration;
//using lib::time::FSecs;
//using lib::time::Time;
//  
//class ExitNode;
  
  /**
   * Frame rendering task, represented as closure.
   * This functor encodes all information necessary actually to
   * trigger and invoke the rendering operation. It will be embedded
   * into a job descriptor and then enqueued with the scheduler for
   * invocation just in time.
   * 
   * @todo 1/12 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  class Job
    {
      
    public:
      
      Job()
        {
          UNIMPLEMENTED ("job representation, planning and scheduling");
        }
      
      // using standard copy operations
      
      
      bool
      isValid()  const
        {
          UNIMPLEMENTED ("validity self check");
        }
    };
  
  
  
}} // namespace proc::engine




extern "C" {
#endif /* =========================== CL Interface ===================== */


////////////////////////////////////TODO define a C binding for use by the scheduler


#ifdef __cplusplus
}
#endif
#endif
