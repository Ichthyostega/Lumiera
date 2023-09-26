/*
  THREAD.hpp  -  thin convenience wrapper for starting threads

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

* *****************************************************/

/** @file sync.cpp
 ** This compilation unit holds some implementation details
 ** of the [thread wrapper](\ref lib::Thread), relegated here
 ** to reduce header inclusions.
 ** @warning setting the thread name only works on some POSIX systems,
 **          and using a GNU specific addition to the libC here.
 */


#include "lib/thread.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <chrono>
#include <pthread.h>

using std::chrono::steady_clock;
using std::chrono_literals::operator ""ms;



namespace lib {
namespace thread{
  
  namespace {
    
    void
    setThreadName (std::thread& handle, string name)
    {
      // API limitation: max 15 characters + \0
      name = util::sanitise(name).substr(0, 15);
      
      pthread_t nativeHandle = handle.native_handle();
      pthread_setname_np(nativeHandle, name.c_str());
    }
    
    void
    lifecycle (string format, string threadID)
    {
      string message = util::_Fmt{format} % threadID;
      TRACE (thread, "%s", message.c_str());
    }
  }
  
  
  
  template<bool autoTerm>
  void
  ThreadWrapper<autoTerm>::markThreadStart (string const& threadID)
  {
    lifecycle ("Thread '%s' start...", threadID);
    setThreadName (threadImpl_, threadID);
  }
  
  
  template<bool autoTerm>
  void
  ThreadWrapper<autoTerm>::markThreadEnd (string const& threadID)
  {
    lifecycle ("Thread '%s' finished.", threadID);
  }
  
  
  template<bool autoTerm>
  void
  ThreadWrapper<autoTerm>::waitGracePeriod()  noexcept
  {
    try {
        auto start = steady_clock::now();
        while (threadImpl_.joinable()
               and steady_clock::now () - start < 20ms
              )
          std::this_thread::yield();
      }
    ERROR_LOG_AND_IGNORE (thread, "Thread shutdown wait")
    
    if (threadImpl_.joinable())
      ALERT (thread, "Thread failed to terminate after grace period. Abort.");
    // invocation of std::thread dtor will presumably call std::terminate...
  }
  
  
  template void ThreadWrapper<true>::markThreadStart (string const&);
  template void ThreadWrapper<true>::markThreadEnd   (string const&);
  template void ThreadWrapper<true>::waitGracePeriod ()  noexcept;
  
  template void ThreadWrapper<false>::markThreadStart (string const&);
  template void ThreadWrapper<false>::markThreadEnd   (string const&);
  template void ThreadWrapper<false>::waitGracePeriod ()  noexcept;
  
  
}}// namespace lib::thread
