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


#include "lib/nocopy.hpp"
#include "lib/iter-explorer.hpp"

#include <cstdint>
#include <atomic>
#include <vector>
#include <chrono>
#include <limits>
#include <algorithm>


namespace lib {
  
  using std::vector;
  
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
      
      using Sequence =  vector<Inc>;
      using Recording = vector<Sequence>;
      
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
          rec_.resize (cnt);
          return *this;
        }
      
      IncidenceCount&
      expectIncidents(size_t cnt)
        {
          REQUIRE (cnt);
          for (Sequence& s : rec_)
            s.reserve (2*cnt);
          return *this;
        }
      
      
      
      /* ===== Measurement API ===== */
      
      void markEnter(uint8_t caseID =0) { addEntry(caseID, false); }
      void markLeave(uint8_t caseID =0) { addEntry(caseID, true); }
      
      
      /* ===== Evaluations ===== */
      
      struct Statistic
        {
          size_t eventCnt{0};
          size_t activationCnt{0};
          double cumulatedTime{0};    ///< aggregated time over all cases
          double activeTime{0};       ///< compounded time of thread activity
          double coveredTime{0};      ///< overall timespan of observation
          double avgConcurrency{0};   ///< amortised concurrency in timespan
          
          vector<size_t> caseCntr{};  ///< counting activations per case
          vector<size_t> thrdCntr{};  ///< counting activations per thread
          vector<double> caseTime{};  ///< aggregated time per case
          vector<double> thrdTime{};  ///< time of activity per thread
          vector<double> concTime{};
          
          template<typename VAL>
          static VAL
          access (vector<VAL> const& data, size_t idx)
            {
              return idx < data.size()? data[idx]
                                      : VAL{};
            }
          size_t cntCase  (size_t  id) { return access (caseCntr, id); }
          size_t cntThread(size_t  id) { return access (thrdCntr, id); }
          double timeCase  (size_t id) { return access (caseTime, id); }
          double timeThread(size_t id) { return access (thrdTime, id); }
          double timeAtConc(size_t id) { return access (concTime, id); }
        };
      
      Statistic evaluate();
      
      double
      calcCumulatedTime()
        {
          return evaluate().cumulatedTime;
        }
      
    };
  
  
  
  /**
   * Visit all data captured thus far, construct an unified timeline
   * and then compute statistics evaluations to characterise observations.
   * @warning caller must ensure there was a barrier or visibility sync before invocation.
   */
  inline IncidenceCount::Statistic
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
      std::stable_sort (timeline.begin(), timeline.end()
                       ,[](Inc const& l, Inc const& r) { return l.when < r.when; }
                       );
      
      int active{0};
      vector<int> active_case;
      vector<int> active_thrd(numThreads);
      stat.thrdCntr.resize (numThreads);
      stat.thrdTime.resize (numThreads);
      stat.concTime.resize (numThreads+1); // also accounting for idle times in range
      
      // Integrate over the timeline...
      // - book the preceding interval length into each affected partial sum
      // - adjust current active count in accordance to the current event
      Instance prev = timeline.front().when;
      for (Inc& event : timeline)
        {
          if (event.caseID >= stat.caseCntr.size())
            {
              active_case  .resize (event.caseID+1);
              stat.caseCntr.resize (event.caseID+1);
              stat.caseTime.resize (event.caseID+1);
            }
          Dur timeSlice = event.when - prev;
          stat.cumulatedTime += active * timeSlice.count();
          for (uint i=0; i < stat.caseCntr.size(); ++i)
            stat.caseTime[i] += active_case[i] * timeSlice.count();
          for (uint i=0; i < numThreads; ++i)
            if (active_thrd[i])   // note: counting activity per thread, without overlapping cases
              stat.thrdTime[i] += timeSlice.count();
          size_t concurr = explore(active_thrd).filter([](int a){ return 0 < a; }).count();
          ENSURE (concurr <= numThreads);
          stat.avgConcurrency += concurr * timeSlice.count(); // contribution for weighted average
          stat.concTime[concurr] += timeSlice.count();
          if (event.isLeave)
            {
              ASSERT (0 < active);
              ASSERT (0 < active_case[event.caseID]);
              ASSERT (0 < active_thrd[event.thread]);
              --active;
              --active_case[event.caseID];
              --active_thrd[event.thread];
            }
          else
            {
              ++active;
              ++active_case[event.caseID];
              ++active_thrd[event.thread];
              ++stat.caseCntr[event.caseID];
              ++stat.thrdCntr[event.thread];
              ++stat.activationCnt;
            }
          prev = event.when;
        }
      Dur covered = timeline.back().when - timeline.front().when;
      stat.coveredTime = covered.count();
      stat.eventCnt = timeline.size();
      ENSURE (0 < stat.activationCnt);
      ENSURE (stat.eventCnt % 2 == 0);
      stat.avgConcurrency /= stat.coveredTime; // time used as weight sum
      stat.activeTime = explore(stat.thrdTime).resultSum();
      return stat;
    }
  
  
} // namespace lib
#endif /*LIB_INCIDENCE_COUNT_H*/
