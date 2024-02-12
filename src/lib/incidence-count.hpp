/*
  INCIDENCE-COUNT.hpp  -  instrumentation helper to watch possibly multithreaded task activations

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file incidence-count.hpp
 ** Record and evaluate concurrent activations.
 ** This observation and instrumentation helper is typically used to capture
 ** density of incidence and effective concurrency of performance critical tasks.
 ** The overhead per measurement call amounts to one SystemClock invocation plus
 ** some heap memory access, assuming that sufficient memory was pre-allocated
 ** prior to the actual observation phase. Moreover, on first invocation per Thread,
 ** a thread local ID is constructed, thereby incrementing an global atomic counter.
 ** Statistics evaluation is comprised of integrating and sorting the captured
 ** event log, followed by a summation pass.
 ** 
 ** # Usage and limitations
 ** This helper is intended for tests and one-time usage. Create an instance,
 ** launch a test, retrieve the observed statistics, destroy the object. Each
 ** separate Threads encountered gets the next consecutive ID. Thus it is *not possible*
 ** to have long-living instances or even multiple instances of IncidenceCount; doing so
 ** would require a much more elaborate ID management, which is beyond requirement's scope. 
 ** 
 ** @see IncidenceCount_test
 ** @see vault::gear::TestChainLoad::ScheduleCtx
 ** @see SchedulerStress_test
 */


#ifndef LIB_INCIDENCE_COUNT_H
#define LIB_INCIDENCE_COUNT_H


//#include "lib/meta/function.hpp"
#include "lib/nocopy.hpp"
#include "lib/iter-explorer.hpp"

//#include <utility>
#include <cstdint>
#include <atomic>
#include <vector>
#include <chrono>
#include <limits>
#include <algorithm>


namespace lib {
  
//  using std::forward;
  
  /**
   * A recorder for concurrent incidences.
   * Start and end of individual activations are recorded by direct calls,
   * automatically detecting the thread identity; for further differentiation,
   * an additional `caseID` can be given. Accumulated observations can be
   * integrated into a statistics evaluation.
   * @warning never operate multiple instances of this helper at the same time
   */
  class IncidenceCount
    : util::NonCopyable
    {
      using TIMING_SCALE = std::micro;       // Results are in µ-sec
      using Clock = std::chrono::steady_clock;

      using Instance = decltype(Clock::now());
      using Dur = std::chrono::duration<double, TIMING_SCALE>;
      
      struct Inc
        {
          Instance when;
          uint8_t thread :8;
          uint8_t caseID :8;
          bool   isLeave :1;
        };
      
      using Sequence = std::vector<Inc>;
      using Recording = std::vector<Sequence>;
      
      Recording rec_;
      
      std::atomic_uint8_t slotID_{0};
      
      /** threadsafe allocation of thread/slotID */
      uint8_t
      allocateNextSlot()
        {                 // Note : returning previous value before increment
          return slotID_.fetch_add(+1, std::memory_order_relaxed);
        }
      
      uint8_t
      getMySlot()
        {
          thread_local uint8_t threadID{allocateNextSlot()};
          ASSERT (threadID < std::numeric_limits<uint8_t>::max(), "WOW -- so many threads?");
          return threadID;
        }
      
      Sequence&
      getMySequence(uint8_t threadID)
        {
          if (threadID >= rec_.size())
            {
              rec_.reserve (threadID+1);
              for (size_t i = rec_.size(); i < threadID+1u; ++i)
                rec_.emplace_back();
            }
          return rec_[threadID];
        }
      
      void
      addEntry(uint8_t caseID, bool isLeave)
        {
          uint8_t threadID{getMySlot()};
          Sequence& seq = getMySequence(threadID);
          Inc& incidence = seq.emplace_back();
          incidence.when = Clock::now();
          incidence.thread = threadID;
          incidence.caseID = caseID;
          incidence.isLeave = isLeave;
        }
      
      
    public:
      IncidenceCount() = default;
      
      
      IncidenceCount&
      expectThreads(uint8_t cnt)
        {
          REQUIRE (cnt);
          rec_.reserve (cnt);
          for ( ; cnt; --cnt)
            rec_.emplace_back();
          return *this;
        }
      
      IncidenceCount&
      expectIncidents(size_t cnt)
        {
          REQUIRE (cnt);
          for (Sequence& s : rec_)
            s.reserve (cnt);
          return *this;
        }
      
      
      
      /* ===== Measurement API ===== */
      
      void markEnter(uint8_t caseID =0) { addEntry(caseID, false); }
      void markLeave(uint8_t caseID =0) { addEntry(caseID, true); }
      
      
      /* ===== Evaluations ===== */
      
      struct Statistic
        {
          double cumulatedTime{0};
        };
      
      Statistic evaluate();
      
      double
      calcCumulatedTime()
        {
          return evaluate().cumulatedTime;
        }
      
    };
  
  
  
  /**
   * Visit all data captured thus far, construct a unified timeline
   * and then compute statistics evaluations to characterise observations
   * @warning caller must ensure there was a barrier or visibility sync before invocation.
   */
  IncidenceCount::Statistic
  IncidenceCount::evaluate()
    {
      Statistic stat;
      size_t numThreads = rec_.size();
      if (numThreads == 0) return stat;
      
      size_t numEvents = explore(rec_)
                           .transform([](Sequence& seq){ return seq.size(); })
                           .resultSum();
      if (numEvents == 0) return stat;
      Sequence timeline;
      timeline.reserve(numEvents);
      for (Sequence& seq : rec_)
        for (Inc& event : seq)
          timeline.emplace_back(event);
      std::sort (timeline.begin(), timeline.end()
                ,[](Inc const& l, Inc const& r) { return l.when < r.when; }
                );
      
      int active{0};
      Instance prev = timeline.front().when;
      for (Inc& event : timeline)
        {
          if (event.isLeave)
            {
              ASSERT (0 < active);
              Dur timeSlice = event.when - prev;
              stat.cumulatedTime += active * timeSlice.count();
              --active;
            }
          else
            {
              ++active;
            }
          prev = event.when;
        }
      return stat;
    }
  
  
} // namespace lib
#endif /*LIB_INCIDENCE_COUNT_H*/
