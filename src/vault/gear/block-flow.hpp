/*
  BLOCK-FLOW.hpp  -  specialised custom allocator to manage scheduler data

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


/** @file block-flow.hpp
 ** Memory management scheme for activities and parameter data passed through
 ** the Scheduler within the Lumiera render engine. While — conceptually — the
 ** intended render operations are described as connected activity terms, sent
 ** as messages through the scheduler, the actual implementation requires a fixed
 ** descriptor record sitting at a stable memory location while the computation
 ** is underway. Moreover, activities can spawn further activities, implying that
 ** activity descriptor records for various deadlines need to be accommodated
 ** and the duration to keep those descriptors in valid state is contingent.
 ** On the other hand, ongoing rendering produces a constant flow of further
 ** activities, necessitating timely clean-up of obsolete descriptors.
 ** Used memory should be recycled, calling for an arrangement of
 ** pooled allocation tiles, extending the underlying block
 ** allocation on increased throughput.
 ** 
 ** # Implementation technique
 ** 
 ** The usage within the [Scheduler](\ref scheduler.hpp) can be arranged in a way
 ** to avoid concurrency issues altogether; while allocations are not always done
 ** by _the same thread,_ it can be ensured at any given time that only a single
 ** Worker performs Scheduler administrative tasks (queue management and allocation);
 ** a read/write barrier is issued whenever some Worker enters this management mode.
 ** 
 ** Memory is allocated in larger _extents,_ which are then used to place individual
 ** fixed-size allocations. These are not managed further, assuming that the storage
 ** is used for POD data records, and the destructors need not be invoked at all.
 ** This arrangement is achieved by interpreting the storage extents as temporal
 ** *Epochs*. Each #Epoch holds an Epoch::EpochGate to define a deadline and to allow
 ** blocking this Epoch by pending IO operations (with the help of a count-down latch).
 ** The rationale is based on the observation that any render activity for late and
 ** obsolete goals is pointless and can be just side stepped. Once the scheduling has
 ** passed a defined deadline (and no further pending IO operations are around), the
 ** Epoch can be abandoned as a whole and the storage extent can be re-used.
 ** 
 ** Dynamic adjustments are necessary to keep this scheme running efficiently.
 ** Ideally, the temporal stepping between subsequent Epochs should be chosen such
 ** as to accommodate all render activities with deadlines falling into this Epoch,
 ** without wasting much space for unused storage slots. But the throughput and thus
 ** the allocation pressure of the scheduler can change intermittently, necessitating
 ** to handle excess allocations by shifting them into the next Epoch. These _overflow
 ** events_ are registered, and on clean-up the actual usage ratio of each Epoch is
 ** detected, leading to exponentially damped adjustments of the actual Epoch duration.
 ** The increasing of capacity on overflow and the exponential targeting of an optimal
 ** fill factor counteract each other, typically converging after some »duty cycles«. 
 ** 
 ** @remark 7/2023 this implementation explicates the intended memory management pattern,
 **         yet a lot more measurements and observations with real-world load patterns
 **         seem indicated. The _characteristic parameters_ in blockFlow::DefaultConfig
 **         expose the most effective tuning points. In its current form, the underlying
 **         ExtendFamily allocates the Extents directly from the default heap allocator,
 **         which does not seem to be of relevance performance-wise, since the pool of
 **         Extents, once allocated, is re-used cyclically.
 ** @see BlockFlow_test
 ** @see SchedulerUsage_test
 ** @see extent-family.hpp underlying allocation scheme
 **
 */


#ifndef SRC_VAULT_GEAR_BLOCK_FLOW_H_
#define SRC_VAULT_GEAR_BLOCK_FLOW_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "vault/mem/extent-family.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>


namespace vault{
namespace gear {
  
  using util::isnil;
  using lib::time::Time;
  using lib::time::FSecs;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::FrameRate;
  
  namespace err = lumiera::error;
  
  
  namespace blockFlow {///< Parametrisation of Scheduler memory management scheme
    
    /** limit for maximum number of blocks allowed in Epoch expansion
     * @note SchedulerCommutator::sanityCheck() defines a similar limit,
     *      but there the same reasoning is translated into a hard limit for
     *      deadlines to be < 20sec, while this limit here will only be triggered
     *      if the current block duration has been lowered to the OVERLOAD_LIMIT
     * @see scheduler-commutator.hpp
     */
    const size_t BLOCK_EXPAND_SAFETY_LIMIT = 3000;
    
    
    /**
     * Lightweight yet safe parametrisation of memory management.
     * Used as default setting and thus for most tests.
     */
    struct DefaultConfig
      {
        /* === characteristic parameters === */
        const static size_t EPOCH_SIZ = 100;    ///< Number of storage slots to fit into one »Epoch«
        const Duration DUTY_CYCLE{FSecs(1)};    ///< typical relaxation time or average pre-roll to deadline
        const size_t INITIAL_STREAMS = 2;       ///< Number of streams with #REFERENCE_FPS to expect for normal use
        
        /* === algorithm tuning settings === */
        const double TARGET_FILL = 0.90;        ///< aim at using this fraction of Epoch space on average (slightly below 100%)
        const double BOOST_FACTOR = 0.85;       ///< adjust capacity by this factor on Epoch overflow/underflow events
        const double DAMP_THRESHOLD = 0.08;     ///< do not account for (almost) empty Epochs to avoid overshooting regulation
        
        /* === contextual assumptions === */
        const size_t ACTIVITIES_PER_FRAME = 10; ///< how many Activity records are typically used to implement a single frame
        const size_t REFERENCE_FPS  =  25;      ///< frame rate to use as reference point to relate DUTY_CYCLE and default counts
        const size_t OVERLOAD_LIMIT =  60;      ///< load factor over normal use where to assume saturation and limit throughput
      };
    
    /**
     * Parametrisation tuned for Render Engine performance.
     */
    struct RenderConfig
      : DefaultConfig
      {
        const static size_t EPOCH_SIZ = 500;
        const size_t INITIAL_STREAMS = 5;
      };
    
    /**
     * Policy template to mix into the BlockFlow allocator,
     * providing the parametrisation for self-regulation
     */
    template<class CONF>
    struct Strategy
      {
        CONF const&
        config()  const
          {           // Meyers Singleton
            static const CONF configInstance;
            return configInstance;
          }
        
        size_t
        framesPerEpoch()  const
          {
            return config().EPOCH_SIZ / config().ACTIVITIES_PER_FRAME;
          }
        
        size_t
        initialFrameRate()  const
          {
            return config().INITIAL_STREAMS * config().REFERENCE_FPS;
          }
        
        Duration
        initialEpochStep()  const
          {
            return Duration{TimeValue(framesPerEpoch() * TimeValue::SCALE / initialFrameRate())};
          }
        
        size_t
        initialEpochCnt()  const       ///< reserve allocation headroom for two duty cycles
          {
            return util::max(2*_raw(config().DUTY_CYCLE) / _raw(initialEpochStep()), 2u);
          }
        
        size_t
        averageEpochs()  const
          {
            return util::max (initialEpochCnt(), 6u);
          }
        
        double
        boostFactor()  const
          {
            return config().BOOST_FACTOR;
          }
        
        double
        boostFactorOverflow()  const   ///< reduced logarithmically, since overflow is detected on individual allocations
          {
            return pow(config().BOOST_FACTOR, 5.0/config().EPOCH_SIZ);
          }
        
        Duration
        timeStep_cutOff()  const       ///< prevent stalling Epoch progression when reaching saturation
          {
            return Duration{TimeValue(_raw(initialEpochStep()) / config().OVERLOAD_LIMIT)};
          }
      };
    
    
    
    
    /**
     * Allocation Extent holding _scheduler Activities_ to be performed altogether
     * before a common _deadline._ Other than the underlying raw Extent, the Epoch
     * maintains a deadline time and keeps track of storage slots already claimed.
     * This is achieved by using the Activity record in the first slot as a GATE term
     * to maintain those administrative information.
     * @remark rationale is to discard the Extent as a whole, once deadline passed.
     */
    template<class ALO>
    class Epoch
      : public ALO::Extent
      {
        using RawIter = typename ALO::iterator;
        using SIZ     = typename ALO::Extent::SIZ;
        
        /// @warning will be faked, never constructed
        Epoch()    = delete;
        
      public:
        /**
         * specifically rigged GATE Activity,
         * used for managing Epoch metadata
         * - the Condition::rest tracks pending async IO operations
         * - the Condition::deadline is the nominal deadline of this Epoch
         * - the field `next` points to the next free allocation Slot to use
         */
        struct EpochGate
          : Activity
          {
            /** @note initially by default there is...
             *      - effectively no deadline
             *      - no IO operations pending (i.e. we can just discard the Epoch)
             *      - the `next` usable Slot is the last Storage slot, and will be
             *        decremented until there is only one slot left (EpochGate itself)
             *  @warning EpochGate is assumed to sit in the Epoch's first slot
             */
            EpochGate()
              : Activity{int(0), Time::ANYTIME}
              {
                // initialise allocation usage marker: start at last usable slot
                next = this + (Epoch::SIZ() - 1);
                ENSURE (next != this);
              }
            // default copyable
            
            activity::Instant&
            deadline()
              {
                return data_.condition.dead;
              }
            
            bool
            isAlive (Time deadline)
              {
                return data_.condition.isHold() // NOTE: expected callback keeps alive
                    or not data_.condition.isDead (deadline);
              }
            
            size_t
            filledSlots()  const
              {
                const Activity* firstAllocPoint{this + (Epoch::SIZ()-1)};
                return firstAllocPoint - next;
              }
            
            bool
            hasFreeSlot()  const
              { // see C++ § 5.9 : comparison of pointers within same array
                return next > this;
              }
            
            Activity*
            claimNextSlot()
              {
                REQUIRE (hasFreeSlot());
                return next--;
              }
          };
        
        
        EpochGate& gate() { return static_cast<EpochGate&> ((*this)[0]); }
        Time   deadline() { return Time{gate().deadline()};              }
        
        double
        getFillFactor()
          {
            return double(gate().filledSlots()) / (SIZ()-1);
          }
        
        
        static Epoch&
        implantInto (RawIter storageSlot)
          {
            Epoch& target = static_cast<Epoch&> (*storageSlot);
            new(&target[0]) EpochGate{};
            return target;
          }
        
        static Epoch&
        setup (RawIter storageSlot, Time deadline)
          {
            Epoch& newEpoch{implantInto (storageSlot)};
            newEpoch.gate().deadline() = deadline;
            return newEpoch;
          }
      };
  
  
  }//(End)namespace blockFlow
  
  template<class CONF>
  class FlowDiagnostic;
  
  
  
  
  /******************************************************//**
   * Allocation scheme for the Scheduler, based on Epoch(s).
   * Scheduling entails to provide a chain of Activity definitions,
   * which will then »flow« through the priority queue until invocation.
   * 
   * @see ActivityLang
   * @see SchedulerCommutator
   * @see BlockFlow_test
   */
  template<class CONF = blockFlow::DefaultConfig>
  class BlockFlow
    : public blockFlow::Strategy<CONF>
    , util::NonCopyable
    {
      constexpr static size_t EPOCH_SIZ = CONF::EPOCH_SIZ;
      
    public:
      using Allocator = mem::ExtentFamily<Activity, EPOCH_SIZ>;
      using Strategy  = blockFlow::Strategy<CONF>;
      using RawIter   = typename Allocator::iterator;
      using Extent    = typename Allocator::Extent;
      using Epoch     = blockFlow::Epoch<Allocator>;
      
      using Strategy::config;
      
    private:
      Allocator alloc_;
      TimeVar epochStep_;
      
      
      /** @internal use a raw storage Extent as Epoch (unchecked cast) */
      static Epoch&
      asEpoch (Extent& extent)
        {
          return static_cast<Epoch&> (extent);
        }
      
      struct StorageAdaptor : RawIter
        {
          StorageAdaptor()  = default;
          StorageAdaptor(RawIter it) : RawIter{it} { }
          Epoch& yield()  const  { return asEpoch (RawIter::yield()); }
        };
      
      

    public:
      BlockFlow()
        : alloc_{Strategy::initialEpochCnt()}
        , epochStep_{Strategy::initialEpochStep()}
        { }
      
      Duration
      getEpochStep()  const
        {
          return Duration{epochStep_};
        }
      
      void
      adjustEpochStep (double factor)
        {
          double stretched = _raw(epochStep_) * factor;
          gavl_time_t microTicks(floor (stretched));
          epochStep_ = TimeValue{microTicks};
        }
      
      
      
      /** Adapted storage-Extent iterator, directly exposing Epoch& */
      using EpochIter = lib::IterableDecorator<Epoch, StorageAdaptor>;
      
      
      /**
       * Local handle to allow allocating a collection of Activities,
       * all sharing a common deadline. Internally, these records are
       * maintained in fixed-sized _extents_ and thus allocations may
       * _overflow_ — leading to allocation of further extents. However,
       * this extension is handled transparently by the embedded iterator.
       * Moreover, a back-connection to the BlockFlow instance is maintained,
       * enabling the latter to manage the Epoch spacing dynamically.
       */
      class AllocatorHandle
        {
          EpochIter epoch_;
          BlockFlow* flow_;
          
        public:
          AllocatorHandle(RawIter slot, BlockFlow* parent)
            : epoch_{slot}
            , flow_{parent}
          { }
          
          /*************************************************//**
           * Main API operation: allocate a new Activity record
           */
          template<typename...ARGS>
          Activity&
          create (ARGS&& ...args)
            {
              return *new(claimSlot()) Activity {std::forward<ARGS> (args)...};
            }
          
          Time currDeadline() const { return epoch_->deadline(); }
          bool hasFreeSlot()  const { return epoch_->gate().hasFreeSlot(); }
          
          
        private:
          void*
          claimSlot() ///< EX_SANE
            {
              while (not (epoch_ and
                          epoch_->gate().hasFreeSlot()))
                  // Epoch overflow...
                {//  shift to following Epoch; possibly allocate
                  if (not epoch_)
                    {
                      auto lastDeadline = flow_->lastEpoch().deadline();
                      epoch_.expandAlloc(); // may throw out-of-memory..
                      ENSURE (epoch_);
                      Epoch::setup (epoch_, lastDeadline + flow_->getEpochStep());
                    }
                  else
                    {
                      flow_->markEpochOverflow();
                      ++epoch_;
                    }
                }
              return epoch_->gate().claimNextSlot();
            }
        };
      
      
      /* ===== public BlockFlow API ===== */
      
      /**
       * initiate allocations for activities to happen until some deadline
       * @return opaque handle allowing to perform several allocations.
       */
      AllocatorHandle
      until (Time deadline)
        {
          if (isnil (alloc_))
            {//just create new Epoch one epochStep ahead
              alloc_.openNew();
              Epoch::setup (alloc_.begin(), deadline + Time{epochStep_});
              return AllocatorHandle{alloc_.begin(), this};
            }
          else
            {//find out how the given time relates to existing Epochs
              if (firstEpoch().deadline() >= deadline)
                // way into the past ... put it in the first available Epoch
                return AllocatorHandle{alloc_.begin(), this};
              else
              if (lastEpoch().deadline() < deadline)
                {  // a deadline beyond the established Epochs...
                  //  create a grid of new epochs up to the requested point
                  TimeVar lastDeadline = lastEpoch().deadline();
                  auto distance = _raw(deadline) - _raw(lastDeadline);
                  EpochIter nextEpoch{alloc_.end()};
                  ENSURE (not nextEpoch);      // not valid yet, but we will allocate starting there...
                  auto requiredNew = distance / _raw(epochStep_);
                  ___sanityCheckAlloc(requiredNew);
                  if (distance % _raw(epochStep_) > 0)
                    ++requiredNew;  // fractional:  requested deadline lies within last epoch
                  alloc_.openNew(requiredNew);   // Note: nextEpoch now points to the first new Epoch
                  for ( ; 0 < requiredNew; --requiredNew)
                    {
                      REQUIRE (nextEpoch);
                      lastDeadline += epochStep_;
                      Epoch::setup (nextEpoch, lastDeadline);
                      if (deadline <= lastDeadline)
                        {
                          ENSURE (requiredNew == 1);
                          return AllocatorHandle{nextEpoch, this};
                        }     // break out and return handle to allocate into the matching Epoch
                      ++nextEpoch;
                    }
                  NOTREACHED ("Logic of counting new Epochs");
                }
              else
                for (EpochIter epochIt{alloc_.begin()}; epochIt; ++epochIt)
                  if (epochIt->deadline() >= deadline)
                    return AllocatorHandle{epochIt, this};
              
              NOTREACHED ("Inconsistency in BlockFlow Epoch deadline organisation");
            }
        }
      
      /**
       * Clean-up all storage related to activities before the given deadline.
       * @note when some Epoch is blocked by pending IO, all subsequent Epochs
       *       will be kept alive too, since the returning IO operation may trigger
       *       activities there (at least up to the point where the control logic
       *       detects a timeout and abandons the execution chain).
       */
      void
      discardBefore (Time deadline)
        {
          if (isnil (alloc_)
              or firstEpoch().deadline() > deadline)
            return;
          
          size_t toDiscard{0};
          for (Epoch& epoch : allEpochs())
            {
              if (epoch.gate().isAlive (deadline))
                break;
              ++toDiscard;
              auto currDeadline = epoch.deadline();
              auto epochDuration = currDeadline - updatePastDeadline(currDeadline);
              markEpochUnderflow (epochDuration, epoch.getFillFactor());
            }
          // ask to discard the enumerated Extents
          alloc_.dropOld (toDiscard);
        }
      
      
      /**
       * Notify and adjust Epoch capacity as consequence of exhausting an Epoch.
       * Whenever some Epoch can not accommodate a required allocation, the allocation
       * is placed into subsequent Epoch(s) and then this event is triggered, reducing
       * the epochStep_ by #OVERFLOW_BOOST_FACTOR to increase capacity.
       */
      void
      markEpochOverflow()
        {
          if (epochStep_ > _cache_timeStep_cutOff)
            adjustEpochStep (_cache_boostFactorOverflow);
        }
      // caching access to the config saves 15-30% per allocation
      Duration _cache_timeStep_cutOff = Strategy::timeStep_cutOff();
      double _cache_boostFactorOverflow = Strategy::boostFactorOverflow();
      
      /**
       * On clean-up of past Epochs, the actual fill factor is checked to guess an
       * Epoch duration to make optimal use of epoch storage. Assuming that requested
       * Activity deadlines are evenly spaced, for a simple heuristic we can just divide
       * actual Epoch duration by the fill factor (longer Epoch => less capacity).
       * To avoid control oscillations however, it seems prudent to use damping by
       * an exponential moving average, nominally over #Strategy::averageEpochs().
       * The current epochStep_ is assumed to be such a moving average,
       * and will be updated accordingly.
       */
      void
      markEpochUnderflow (TimeVar actualLen, double fillFactor)
        {
          auto interpolate = [&](auto f, auto v1, auto v2) { return f*v2 + (1-f)*v1; };
          
          // use actual fill as signal, set desired fill-level as goal
          fillFactor /= config().TARGET_FILL;
          auto THRESH = config().DAMP_THRESHOLD;
          double adjust =
            fillFactor > THRESH? fillFactor   //  limit signal for almost empty Epochs to avoid overshooting
                               : interpolate (1 - fillFactor/THRESH, fillFactor, Strategy::boostFactor());
          
          // damped adjustment towards ideal size
          double contribution = double(_raw(actualLen)) / _raw(epochStep_) / adjust;
          
          // Exponential MA: mean ≔ mean · (N-1)/N  + newVal/N
          auto N = Strategy::averageEpochs();
          double avgFactor = (contribution + N-1) / N;    // contribution = newVal / mean  => can extract factor
          adjustEpochStep (avgFactor);
        }
      
      
      /**
       * provide a hint to the self-regulating allocation scheme.
       * Signalling additional calculation flow in advance will
       * immediately reduce the Epoch stepping to accommodate for
       * more Activities per time unit, thereby preventing excessive
       * overflow and reduced performance, until the mechanism has
       * adapted itself to the actual situation after roughly 2sec.
       */
      void
      announceAdditionalFlow (FrameRate additionalFps)
        {
          FrameRate currFps{Strategy::framesPerEpoch(), Duration{epochStep_}};
          currFps += additionalFps;
          TimeVar adaptedSpacing = Strategy::framesPerEpoch() / currFps;
          epochStep_ = util::max (adaptedSpacing, _cache_timeStep_cutOff);
        }
      
      
    private:
      Epoch&
      firstEpoch()
        {
          REQUIRE (not isnil (alloc_));
          return asEpoch(*alloc_.begin());
        }
      Epoch&
      lastEpoch()
        {
          REQUIRE (not isnil (alloc_));
          return asEpoch(*alloc_.last());
        }
      
      EpochIter
      allEpochs()
        {
          return alloc_.begin();
        }
      
      /** @internal helper to calculate the duration of the oldest Epoch.
       * @remark since we store the deadline for each Epoch, not it's duration,
       *         we need to memorise and update a starting point, to calculate
       *         the duration, which is used to guess an averaged optimal duration.
       * @param current deadline of the oldest block, about to be discarded
       * @return the memorised previous oldest deadline
       */
      Time
      updatePastDeadline (TimeVar newDeadline)
        {
          if (pastDeadline_ == Time::ANYTIME)
            pastDeadline_ = newDeadline - epochStep_;
          TimeVar previous = pastDeadline_;
          pastDeadline_ = newDeadline;
          return previous;
        }
      TimeVar pastDeadline_{Time::ANYTIME};
      
      void
      ___sanityCheckAlloc (size_t newBlockCnt)
        {
          if (newBlockCnt > blockFlow::BLOCK_EXPAND_SAFETY_LIMIT)
            throw err::Fatal{"Deadline expansion causes allocation of "
                              +util::showSize(newBlockCnt) +"blocks > "
                              +util::showSize(blockFlow::BLOCK_EXPAND_SAFETY_LIMIT)
                            , err::LUMIERA_ERROR_CAPACITY};
        }
      
      
      /// „backdoor“ to watch internals from tests
      friend class FlowDiagnostic<CONF>;
    };
  
  
  
  
  
  
  
  
  /* ===== Test / Diagnostic ===== */
  
  template<class CONF>
  class FlowDiagnostic
    {
      using Epoch = typename BlockFlow<CONF>::Epoch;
      
      BlockFlow<CONF>& flow_;
      
    public:
      FlowDiagnostic(BlockFlow<CONF>& theFlow)
        : flow_{theFlow}
      { }
      
      Time   first()     { return flow_.firstEpoch().deadline();}
      Time   last()      { return flow_.lastEpoch().deadline(); }
      size_t cntEpochs() { return watch(flow_.alloc_).active(); }
      size_t poolSize()  { return watch(flow_.alloc_).size();   }
      
      /** find out in which Epoch the given Activity was placed */
      TimeValue
      find (Activity& someActivity)
        {
          for (Epoch& epoch : flow_.allEpochs())
            for (Activity& act : epoch)
              if (util::isSameObject (act, someActivity))
                return epoch.deadline();
          return Time::NEVER;
        }
      
      /** render deadlines of all currently active Epochs */
      std::string
      allEpochs()
        {
          if (isnil (flow_.alloc_)) return "";
          auto deadlines = lib::explore (flow_.allEpochs())
                               .transform([](Epoch& a){ return TimeValue{a.deadline()}; });
          return util::join(deadlines, "|");
        }
      
      /** count all currently active allocated elements */
      size_t
      cntElm()
        {
          size_t cnt{0};
          for (auto& epoch : flow_.allEpochs())
            cnt += epoch.gate().filledSlots();
          return cnt;
        }
    };
  
  template<class CONF>
  inline FlowDiagnostic<CONF>
  watch (BlockFlow<CONF>& theFlow)
  {
    return FlowDiagnostic{theFlow};
  }
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_BLOCK_FLOW_H_*/
