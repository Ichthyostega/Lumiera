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
typedef uint64_t InvocationInstanceID;  /////////////////TODO


enum JobState
  { 
    DONE,      ///< already done, nothing to do
    RUNNING,   ///< job is currently running
    WAITING,   ///< waits for some prerequisite resource
    REJECTED,  ///< sorry, can't do that Dave
    EXPIRED,   ///< deadline expired
    ABORTED    ///< got aborted
  };

enum JobKind
  {
    CALC_JOB,  ///< calculating frame data, CPU bound
    LOAD_JOB,  ///< accessing prerequisites, IO bound
    META_JOB   ///< render process self organisation
  };


/** 
 * closure representing the execution context of a job.
 * The information reachable through this closure is specific
 * for this kind of job, but static and typically shared among
 * all jobs for a given feed and segment of the timeline
 */ 
struct lumiera_jobClosure { /* placeholder */ };
typedef struct lumiera_jobClosure* LumieraJobClosure;


/** 
 * invocation parameter for the individual frame calculation job.
 * Embedded into the job descriptor and passed to #lumiera_job_invoke when triggering
 */
struct lumiera_jobParameter_struct
  {
    gavl_time_t nominalTime;
    InvocationInstanceID invoKey;
    //////////////////////////////////////////////////////////////TODO: place an additional parameter value here, or make the instanceID globally unique?
  };
typedef struct lumiera_jobParameter_struct lumiera_jobParameter;
typedef lumiera_jobParameter* LumieraJobParameter;


/** complete definition of an individual job */
struct lumiera_jobDefinition_struct
  {
    LumieraJobClosure jobClosure;     ///< type and context of the job, including the actual functor
    lumiera_jobParameter parameter;   ///< the "moving parts" for this individual invocation (Job)
  };
typedef struct lumiera_jobDefinition_struct lumiera_jobDefinition;


/**
 * descriptor record used by the scheduler to organise job invocation.
 * The actual job's definition, i.e. the invocation parameter and
 * the closure necessary to invoke the job as a function
 * is embedded (by value) into this descriptor.
 * 
 * @note while this descriptor as such is self-contained,
 *       the referred LumieraJobClosure needs to be allocated
 *       and managed separately. Indeed, this closure happens
 *       to live within the segment data, as part of the JobTicket.
 */
struct lumiera_jobDescriptor_struct
  {
    gavl_time_t when;
    JobState jobState;
    
    lumiera_jobDefinition jobDefinition;
    
    /* == Job prerequisites == */
    LList waiting;
    LList failed;
    LList completed;
  };
typedef struct lumiera_jobDescriptor_struct lumiera_jobDescriptor;
typedef lumiera_jobDescriptor* LumieraJobDescriptor;








#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"



namespace proc {
namespace engine {
  
  using lib::time::TimeValue;
  using lib::time::Time;
  
  typedef lumiera_jobParameter const& JobParameter;
  
  
  /**
   * Individual frame rendering task, forwarding to a closure.
   * This functor encodes all information necessary to trigger
   * and invoke the actual rendering operation. It will be embedded
   * by value into a job descriptor and then enqueued with the scheduler
   * for invocation just in time. The job interface exposes everything necessary
   * to plan, handle, schedule and abort jobs. The implementation refers to the
   * concrete "execution plan" encoded into the corresponding engine::JobTicket.
   * The latter is embedded into the storage for one segment of the low-level model
   * and thus is shared for all frames and channels within this part of the timeline.
   * Thus, the lumiera_jobParameter struct contains the "moving parts"
   * different for each \em individual job.
   * 
   * @todo 2/12 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  class Job
    : public lumiera_jobDefinition
    {
      
    public:
      
      Job()
        {
          UNIMPLEMENTED ("job creation, planning and scheduling");
        }
      
      // using standard copy operations
      
      
      void triggerJob()    const;
      void signalFailure() const;
      
      
      Time
      getNominalTime()  const
        {
          return Time (TimeValue(parameter.nominalTime));
        }
      
      InvocationInstanceID
      getInvocationInstanceID()  const
        {
          return this->parameter.invoKey;
        }
      
      JobKind getKind()  const;
      bool isValid()  const;
    };
  
  
  /**
   * Interface of the closure for frame rendering jobs.
   * Hidden behind this interface resides all of the context re-building
   * and invocation mechanics to get the actual calculations going. While
   * the job descriptor, as handled by the scheduler, contains the variable
   * "moving parts", the corresponding job closure represents the execution
   * context of a job and is shared between several jobs within the same
   * segment of the timeline.
   * 
   * This allows us to enqueue simple job-"functions" with the scheduler.
   * By virtue of the JobClosure-pointer, embedded into #lumiera_jobDefinition,
   * the invocation of such a job may re-gain the full context, including the
   * actual ProcNode to pull and further specifics, like the media channel.
   */
  class JobClosure
    : public lumiera_jobClosure
    {
    public:
      virtual ~JobClosure();     ///< this is an interface
      
      
      virtual void invokeJobOperation (JobParameter parameter)  =0;
      virtual void signalFailure      (JobParameter parameter)  =0;
      
      virtual JobKind getJobKind()  const                       =0;
      virtual bool verify (Time nominalJobTime)  const          =0;
    };
  
  
}} // namespace proc::engine




extern "C" {
#endif /* =========================== CL Interface ===================== */


/** trigger execution of a specific job,
 *  assuming availability of all prerequisites */
void lumiera_job_invoke  (lumiera_jobDefinition);

/** signal inability to invoke this job
 * @todo decide what and how to communicate details of the failure
 * @remarks the purpose of this function is to allow for reliable checkpoints
 *          within the network of dependent job invocations, even after
 *          missing deadlines or aborting a sequence of jobs */
void lumiera_job_failure (lumiera_jobDefinition);



#ifdef __cplusplus
}
#endif
#endif
