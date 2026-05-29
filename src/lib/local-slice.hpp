/*
  LOCAL-SLICE.hpp  -  access front-end to thread-local instances

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-slice.hpp
 ** Front-end to attach to a set of thread-local instances.
 ** This setup helps to adapt a given service implementation to be deployed
 ** as per-thread instances, operating in a massively concurrent situation.
 ** The services installed this way must be able to work as self-contained
 ** instances transparently, possibly cooperating with a central hub behind
 ** the scenes, which however must be coordinated by the services themselves.
 ** 
 ** Since thread-local storage is effectively static, the configuration of
 ** instances happens at some point _prior_ to the first use from within a
 ** thread, and otherwise not further specified. The LocalSlice access front-end
 ** is a _monostate_ and all instances thus expose the same set of per-thread
 ** service instances, which are created on-demand (by the C++ runtime system).
 ** 
 ** @warning as of 2022 the implementation of `thread_local` in the GNU runtime
 **          was observed to incur seemingly unnecessary overhead, which however
 **          is relevant only in a high-performance environment. However, some
 **          [casual benchmarks](\ref LocalSlice_test::investigatePerformance)
 **          indicated that the problem does not seem to be _that serious_...
 ** @see LocalSlice_test
 ** @see LocalBufferStage (usage example)
 ** @see buffer-provider-load-test.cpp
 ** 
 */

#ifndef LIB_LOCAL_SLICE_H_
#define LIB_LOCAL_SLICE_H_

#include "lib/integral.hpp"


namespace lib {

  /** Extension point for LocalSlice: Service initialisation */
  template<class SRV>
  struct LocalSlice_DefaultSetup
    {
      using Service = SRV;
      static Service init() { return Service(); }
    };
  
  
  /**
   * Common access point to thread-local service instances.
   * @tparam CONF setup to initialise the service.
   */
  template<class SRV, class CONF =LocalSlice_DefaultSetup<SRV>>
  struct LocalSlice
    {
      LocalSlice()  = default;
      // using default copy/assignment
      
      
      using Service = CONF::Service;
      
      /** service access */
      Service&
      operator*()  const
        {
          thread_local Service srvInstance{CONF::init()};
          return srvInstance;
        }
      
      Service*
      operator->()  const
        {
          return & LocalSlice::operator*();
        }
    };
  
  
} // namespace lib
#endif /*LIB_LOCAL_SLICE_H_*/
