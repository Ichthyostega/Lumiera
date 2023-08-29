/*
  ACTIVITY-TERM.hpp  -  definition language framework for scheduler activities

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


/** @file activity-term.hpp
 ** A term of the _activity language_ describing interconnected scheduler activities.
 ** The activity::Term is created as a transient object in _builder notation,_ and used
 ** to lay out the specifics of the planned operations necessary to calculate a single
 ** frame or to carry out some administrative task. The actual \ref Activity verbs are
 ** allocated independently, within the BlockFlow allocation scheme, while the Term
 ** links and configures these data records and prepares them to instruct the Scheduler.
 ** Through the activity::Term, relevant attachment points are exposed for configuration,
 ** yet after posting the entrance point of an activity chain to the Scheduler, the term
 ** is obsoleted and can be discarded without affecting the individual Activities awaiting
 ** activation through the Scheduler.
 ** 
 ** @see SchedulerActivity_test
 ** @see activity-lang.hpp Entrance point to Activity definition
 ** @see activity.hpp definition of verbs
 ** 
 ** @todo WIP-WIP-WIP 8/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_TERM_H_
#define SRC_VAULT_GEAR_ACTIVITY_TERM_H_


#include "vault/gear/activity.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/job.h"
//#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/util.hpp"

#include <string>
#include <utility>


namespace vault{
namespace gear {
  
  using lib::time::Time;
  using lib::time::TimeValue;
//  using util::isnil;
  using std::string;
  using std::move;
  
  using BlockFlowAlloc = BlockFlow<blockFlow::RenderConfig>;
  
  
  namespace activity {
    
    /**
     * A Term of the »Activity Language«, describing the steps necessary
     * to perform the calculation of a single frame or similar tasks.
     */
    class Term
      {
        
        using AllocHandle = BlockFlowAlloc::AllocatorHandle;
        
        AllocHandle alloc_;
        
        Activity* invoke_{nullptr};
        Activity* post_{nullptr};
        
        Activity* gate_{nullptr};
        
        
      public:
        enum Template {CALC_JOB  ///< scheme for a synchronous media calculation job
                      ,LOAD_JOB  ///< scheme for an asynchronous data retrieval job
                      ,META_JOB  ///< scheme for planning and organisational job
                      };
        
        explicit
        Term (AllocHandle&& allocHandle, Template kind, Time start, Time after, Job job)
          : alloc_{move (allocHandle)}
          , invoke_{setupInvocation (job)}
          , post_{setupPost (start,after, invoke_)}
          {
            configureTemplate (kind);
          }
        
        // standard copy acceptable
        
        operator std::string()  const
          {
            return "Term-"
                 + (post_? string{*post_} : util::BOTTOM_INDICATOR)
                 + "⧐"
                 + (invoke_? string{*invoke_} : util::BOTTOM_INDICATOR);
          }
        
        
        /**
         * @return entrance point to this Activity-chain setup
         * @remark use this call for instructing the Scheduler.
         */
        Activity&
        post()
          {
            REQUIRE (post_, "Activity Term not yet fully configured");
            return *post_;
          }
        
      private:
        void
        configureTemplate (Template kind)
          {
            
          }
        
        Activity*
        setupInvocation (Job& job)
          {
            Activity& feed1 = alloc_.create (job.parameter.invoKey.code.w1
                                            ,job.parameter.invoKey.code.w2);
            Activity& feed2 = alloc_.create (Activity::FEED);         ///////////////////////////////////////TICKET #1295 : rework Job parameters to accommodate input / output info required for rendering          
            feed1.next = &feed2;
            
            JobClosure* functor = static_cast<JobClosure*> (job.jobClosure);             ////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
            REQUIRE (functor);
            Activity& invo  = alloc_.create (*functor
                                            , Time{TimeValue{job.parameter.nominalTime}}
                                            , feed1);             ///////////////////////////////////////////TICKET #1287 : get rid of C-isms in Job descriptor
            return & invo;
          }
        
        Activity*
        setupPost (Time start, Time after, Activity* followUp)
          {
            return & alloc_.create (start,after,followUp);
          }
      };
    
    
    /** */
    
  }//(End)namespace activity
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_TERM_H_*/
