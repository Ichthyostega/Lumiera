/*
  JOB.h  -  render job closure

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


/** @file job.h
 ** Definition of a render job.
 ** Jobs are defined within Steam-Layer and passed to the scheduler in the Back-end
 ** for time bound invocation. This header defines the data structures used to describe
 ** a job, and the basic data structures used by the scheduler to keep track of individual
 ** jobs. Moreover, within the C++ part of this header, some classes are layered on top
 ** of these data structures; especially the JobClosure ABC describes the _generic part_
 ** of each job, while the "moving parts" are embedded within the lumiera_jobParameter.
 ** 
 ** A render job is a parameterless function, ready to be invoked by the scheduler..
 ** Since every non trivial job actually needs some parameters (at least a frame number)
 ** and relies on additional definitions and data structures, a _closure_ is created
 ** to make these dependencies explicit and opaque for the scheduler. The actual
 ** job invocation is forwarded to a virtual function JobClosure#invokeJobOperation(JobParameter),
 ** which is defined "somewhere" in a subclass and remains opaque for the scheduler;
 ** the \link steam::engine::Dispatcher frame dispatcher \endlink takes care to configure
 ** each job descriptor with the correct pointer to a concrete closure prior to handing
 ** the job over to the scheduler.
 ** 
 ** @warning as of 4/2023 the Job datastructure will be remoulded ///////////////////////////////////////////TICKET #1280
 ** 
 ** @see SchedulerFrontend
 ** @see JobTicket
 **
 */


#ifndef VAULT_GEAR_JOB_H
#define VAULT_GEAR_JOB_H


#include "lib/llist.h"
#include "lib/hash-value.h"
#include "lib/time.h"



enum JobState
  { 
    DONE,      ///< mission accomplished
    RUNNING,   ///< job is currently running
    WAITING,   ///< waiting for some prerequisite
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

enum JobPriority
  {
    TIMEBOUND_JOB,   ///< regular job scheduled for time-bound delivery
    PAUSED_JOB,      ///< @todo do we need this special state?
    ASAP_JOB,        ///< job for freewheeling calculation of final results
    BACKGROUND_JOB   ///< background rendering job
  };


/**
 * @todo find out about the possible kinds of failure
 */
enum JobFailureReason
  {
    TIMEOUT, 
    PREREQUISITE_NOT_AVAILABLE  ///////////////TODO
  };

typedef int64_t FrameCnt;



/**
 * closure representing the execution context of a job.
 * The information reachable through this closure is specific
 * for this kind of job, but static and typically shared among
 * all jobs for a given feed and segment of the timeline
 */
struct lumiera_jobClosure { /* placeholder */ };
typedef struct lumiera_jobClosure* LumieraJobClosure;



/** opaque ID attached to each individual job invocation.
 *  Used by the implementation of the Jobs (i.e. the JobClosure)
 *  for internal organisation; will be fed back on job activation.
 *  @todo 4/2023 for the time being, this is a mere marker for test code;
 *        for the real engine it is planned to generate a unique reproducible
 *        hash key for each invocation, which can be used for caching; obviously
 *        this hash need to be built from the JobTicket, based on ProcNode structure
 *        and the nominal Time. ///////////////////////////////////TICKET #1293
 */
union InvocationInstanceID
  {
    lumiera_uid luid{0};
    
    /* ----- alternative accessors for test code ---- */
    FrameCnt frameNumber;
    struct { int32_t a,b;
             int64_t t;
           } part;
  };




/** 
 * invocation parameter for the individual frame calculation job.
 * Embedded into the job descriptor and passed to #lumiera_job_invoke when triggering
 */
struct lumiera_jobParameter_struct
  {
    gavl_time_t nominalTime;        /////////////////////////////////////////////////////////////////////////TICKET #1295 job invocation parameter: framework to interpret this time
    InvocationInstanceID invoKey;
                         //////////////////////////////////////////////////////////////TODO: place an additional parameter value here, or make the instanceID globally unique?
                         ////////////////////////////////////////////////////////////////////////////////////TICKET #1293 job invocation identity
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
typedef lumiera_jobDefinition* LumieraJobDefinition;


/**
 * Description of a job. Jobs are passed by the Steam-Layer to the Back-End. 
 * 
 * This descriptor record is used by the scheduler to organise job invocation.
 * The actual job's definition, i.e. the invocation parameter and the closure
 * necessary to invoke the job as a function is embedded (by value)
 * into this descriptor.
 * 
 * @remarks all fields of interest only to the vault layer,
 *       except #jobDefinition, which is provided by and of
 *       interest to the Steam-Layer
 * @note while this descriptor as such is self-contained,
 *       the referred LumieraJobClosure needs to be allocated
 *       and managed separately. Indeed, this closure happens
 *       to live within the segment data, as part of the JobTicket.
 */
struct lumiera_jobDescriptor_struct
  {
    gavl_time_t deadline;                ///< given in real wall clock time
    JobState jobState;
    
    lumiera_jobDefinition jobDefinition; ///< of interest only to Steam-Layer
    
    /* == Job prerequisites == */
    LList waiting;
    LList failed;
    LList completed;
  };
typedef struct lumiera_jobDescriptor_struct lumiera_jobDescriptor;
typedef lumiera_jobDescriptor* LumieraJobDescriptor;








#ifdef __cplusplus  /* ============== C++ Interface ================= */




namespace vault{
namespace gear {
  
  using lib::time::TimeValue;
  using lib::time::Time;
  using lib::HashVal;
  
  typedef lumiera_jobParameter const& JobParameter;
  
  
  /** @todo refactoring 4/23 -- will replace JobClosure */
  class JobFunctor                                      /////////////////////////////////////////////////////TICKET #1287 : rework Job representation
    : util::NonCopyable            // ....has distinct identity and stable address
    {
    public:
      virtual ~JobFunctor();     ///< this is an interface
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
   * @deprecated 4/23 plain-C data structures are removed from the Scheduler interface; transition to JobFunctor
   */                                                   /////////////////////////////////////////////////////TICKET #1287 : rework Job representation
  class JobClosure
    : public lumiera_jobClosure
    , public JobFunctor                                 /////////////////////////////////////////////////////TICKET #1287 : rework Job representation
    {
    public:
      virtual ~JobClosure();     ///< this is an interface
      
      
      virtual void invokeJobOperation (JobParameter parameter)   =0;
      virtual void signalFailure (JobParameter,JobFailureReason) =0;
      
      virtual JobKind getJobKind()                         const =0;
      virtual bool verify (Time, InvocationInstanceID)     const =0;
      virtual HashVal hashOfInstance(InvocationInstanceID) const =0;
      virtual InvocationInstanceID buildInstanceID(HashVal)const =0;             ////////////////////////////TICKET #1293 : a size_t hash? or a LUID?
      
      lib::HashVal hash_value (JobParameter)  const;
    };
  
  
  
  
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
   */
  class Job
    : public lumiera_jobDefinition
    {
      
    public:
      
      Job (JobClosure& specificJobDefinition
          ,InvocationInstanceID invoKey
          ,Time nominalFrameTime)
        {
          this->jobClosure = &specificJobDefinition;
          this->parameter.nominalTime = _raw(nominalFrameTime);
          this->parameter.invoKey = invoKey;
        }
      
      // using standard copy operations
      
      
      void triggerJob()                     const;
      void signalFailure (JobFailureReason) const;
      
      
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
      
      JobKind getKind() const;
      bool isValid()    const;
      
      bool usesClosure (JobClosure const&)  const;
      
      /** provide a hash based Job ID */
      friend lib::HashVal hash_value (Job const&);
    };
  
  inline bool
  operator== (Job const& left, Job const& right)
  {
    return hash_value (left) == hash_value (right);
  }
  
  inline bool
  operator!= (Job const& left, Job const& right)
  {
    return hash_value (left) != hash_value (right);
  }
  
  
  
  
  
}} // namespace vault::gear




extern "C" {
#endif /* =========================== C Interface ===================== */


/** trigger execution of a specific job,
 *  assuming availability of all prerequisites */
void lumiera_job_invoke  (LumieraJobDefinition);

/** signal inability to invoke this job
 * @todo decide what and how to communicate details of the failure
 * @remarks the purpose of this function is to allow for reliable checkpoints
 *          within the network of dependent job invocations, even after
 *          missing deadlines or aborting a sequence of jobs */
void lumiera_job_failure (LumieraJobDefinition, JobFailureReason);

/** calculate a hash value based on the Job's \em identity. */
size_t lumiera_job_get_hash (LumieraJobDefinition);

int lumiera_invokey_eq (void* l, void* r);


#ifdef __cplusplus
}
#endif
#endif /*VAULT_GEAR_JOB_H*/
