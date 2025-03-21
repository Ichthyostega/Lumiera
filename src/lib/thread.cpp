/*
  THREAD.hpp  -  thin convenience wrapper for starting threads

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file sync.cpp
 ** This compilation unit holds some implementation details
 ** of the [thread wrapper](\ref lib::Thread), relegated here
 ** to reduce header inclusions.
 ** @warning setting the thread name only works on some POSIX systems,
 **          and using a GNU specific addition to the libC here.
 */


#include "lib/thread.hpp"
#include "lib/format-string.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <chrono>
#include <atomic>
#include <pthread.h>

using util::_Fmt;
using lib::Literal;
using std::atomic_uint;
using std::chrono::steady_clock;
using std::chrono_literals::operator ""ms;



namespace lib {
namespace thread{
  
  namespace {
    const auto SHUTDOWN_GRACE_PERIOD = 20ms;
    
    string
    lifecycleMsg (Literal phase, string threadID)
    {
      return _Fmt{"Thread '%s' %s"} % threadID % phase;
    }
  }
  
  
  /** Helper to create a suffix to the thread-ID with running count */
  string
  ThreadWrapper::decorate_with_global_count (string const& rawID)
  {
    static atomic_uint globalCnt{1};
    return _Fmt{"%s.%03i"} % rawID % globalCnt.fetch_add (+1, std::memory_order_acq_rel);
  }
  
  
  /** @note implies get_id() != std::thread::id{} ==> it is running */
  bool
  ThreadWrapper::invokedWithinThread()  const
  {
    return threadImpl_.get_id() == std::this_thread::get_id();
  }

  
  void
  ThreadWrapper::markThreadStart()
  {
    TRACE (thread, "%s", lifecycleMsg ("start...", threadID_).c_str());
    setThreadName();
  }
  
  
  void
  ThreadWrapper::markThreadEnd()
  {
    TRACE (thread, "%s", lifecycleMsg ("terminates.", threadID_).c_str());
  }
  
  
  void
  ThreadWrapper::setThreadName()
  {
    pthread_t nativeHandle = threadImpl_.native_handle();
    
    // API limitation: max 15 characters + \0
    pthread_setname_np(nativeHandle, threadID_.substr(0, 15).c_str());
  }
  
  
  void
  ThreadWrapper::waitGracePeriod()  noexcept
  {
    try {
        auto start = steady_clock::now();
        while (threadImpl_.joinable()
               and steady_clock::now () - start < SHUTDOWN_GRACE_PERIOD
              )
          std::this_thread::yield();
      }
    ERROR_LOG_AND_IGNORE (thread, lifecycleMsg("shutdown wait", threadID_).c_str());
    
    if (threadImpl_.joinable())
      ALERT (thread, "Thread '%s' failed to terminate after grace period. Abort.", threadID_.c_str());
    // invocation of std::thread dtor will presumably call std::terminate...
  }
  
  
  
}}// namespace lib::thread
