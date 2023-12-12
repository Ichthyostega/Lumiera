/*
  SPECIAL-JOB-FUN.hpp  -  a one-time render job to do something special

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


/** @file special-job-fun.hpp
 ** A configurable one-time job to invoke some _special_ function.
 ** The actual operation is configured as λ-function and the instance
 ** manages itself into heap storage and automatically destroys itself
 ** after the predetermined invocation. The intended usage is to supply
 ** a specifically wired one-time »fire-and-forget« action to the Scheduler
 ** as answer so some special processing situation. The front-end handle
 ** SpecialJobFun itself is disposable and only serves as builder; even
 ** after the front-end is gone, the actual JobFunctor will maintain
 ** one self-reference — unless it is invoked...
 ** @warning obviously this is a dangerous tool; the user *must ensure*
 **       that it is at most *invoked once* — after that, the instance
 **       will self-destroy, leaving a dangling reference.
 ** @todo WIP 12/2023 invented to help with Scheduler load testing as
 **       part of the »Playback Vertical Slice« — this idea however
 **       might be generally useful to handle one-time adjustments
 **       from within a play- or planning process.
 ** @see SpecialJobFun_test
 ** @see TestChainLoad::ScheduleCtx::continuation() usage example
 */


#ifndef VAULT_GEAR_SPECIAL_JOB_FUN_H
#define VAULT_GEAR_SPECIAL_JOB_FUN_H



#include "lib/handle.hpp"
#include "vault/gear/nop-job-functor.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/meta/util.hpp"

#include <string>
#include <memory>
#include <utility>


namespace vault{
namespace gear {
  namespace err = lumiera::error;
  
  using util::_Fmt;
  using lib::meta::disable_if_self;
  using std::move;
  using std::forward;
  using std::make_shared;
  
  
  /**
   * Interface: JobFunctor configured to invoke a function a limited number of times. 
   */
  class SpecialFunPrototype
    : public NopJobFunctor
    {
    public:
      virtual uint remainingInvocations()  const  =0;
    };
  
  
  
  
  /**
   * Front-end to configure a special job functor for one-time use.
   */
  class SpecialJobFun
    : protected lib::Handle<SpecialFunPrototype>
    {
      using _Handle = lib::Handle<SpecialFunPrototype>;
      
      template<class FUN>
      class SpecialExecutor
        : SpecialFunPrototype
        {
          FUN fun_;
          uint lives_{1};
          _Handle selfHook_;
          
        public:
          explicit
          SpecialExecutor (FUN theFun)
            : fun_{move (theFun)}
            , selfHook_(this)
            { }
          
          /** @internal allow the front-end to attach
           *  itself to the self-managing hook */
          friend _Handle&
          selfAttached (SpecialExecutor* instance)
          {
            REQUIRE (instance->selfHook_);
            REQUIRE (instance->remainingInvocations());
            // expose the self-managing handle
            return instance->selfHook_;
          }
          
          
          uint
          remainingInvocations()  const
            {
              return lives_;
            }
          
          
          /* === JobFunctor Interface === */
          
          void
          invokeJobOperation (JobParameter param)  override
            {
              if (not remainingInvocations())
                throw err::Logic{"invoking deceased SpecialJobFun"
                                ,err::LUMIERA_ERROR_LIFECYCLE};
              fun_(param);
              
              if (not --lives_)
                selfHook_.close();
            }          // Suicide.
          
          std::string
          diagnostic()  const override
            {
              return _Fmt{"SpecialJob(%d)-%s"}
                         % lives_
                         % util::showHash(size_t(this), 2);
            }
        };
      
      
      
    public:
      SpecialJobFun()  = default;
      
      /**
       * Establish a new SpecialJobFun variation
       * directly by wrapping a given functor.
       * The JobFunctor instance itself will be heap allocated.
       * @warning while a direct reference to this JobFunctor can be
       *          obtained through conversion, this reference must not be
       *          used after invoking the job one single time; after that,
       *          this reference is dangling and using it further will
       *          lead to SEGFAULT or memory corruption.
       */
      template<class FUN,            typename =disable_if_self<FUN, SpecialJobFun>>
      explicit
      SpecialJobFun (FUN&& someFun)
        : _Handle{selfAttached (new SpecialExecutor(forward<FUN> (someFun)))}
        { }
      
      // standard copy operations acceptable
      
      explicit
      operator bool()  const
        {
          return 0 < remainingInvocations();
        }
      
      operator JobClosure&()  const
        {
          REQUIRE (operator bool());
          return _Handle::impl();
        }
      
      uint
      remainingInvocations()  const
        {
          return _Handle::isValid()? _Handle::impl().remainingInvocations()
                                   : 0;
        }
      
      long
      use_count()  const
        {
          return _Handle::smPtr_.use_count();
        }
    };
  
  
}} // namespace vault::gear
#endif /*VAULT_GEAR_SPECIAL_JOB_FUN_H*/
