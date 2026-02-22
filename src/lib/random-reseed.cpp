/*
  RandomReseed  -  establish controlled seed for deterministic randomness

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file random-reseed.cpp
 ** Implementation of coordinated reseeding of random number generators.
 ** @todo 3/2024 work out how some executions or performances are configured reproducibly.
 */


#include "lib/error.hpp"
#include "lib/random.hpp"
#include "lib/random-reseed.hpp"
#include "lib/format-string.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/util.hpp"

#include <map>

namespace lib {
  namespace err = lumiera::error;
  
  using util::_Fmt;
  using util::contains;
  using iter_stl::eachVal;

  typedef void (*Hook)();
  
  
  namespace {// Implementation: a central registry for callbacks...
    
    class RandomReseed
      : util::NonCopyable
      {
        std::map<uint, Hook> registry_;
        
      public:
        void
        enrol (uint idx, Hook callback)
          {
            if (contains (registry_, idx))
              throw err::Fatal{_Fmt{"Duplicate registration for random-reseeding-slot %d"} % idx};
            
            registry_[idx] = callback;
          }
        
        void
        triggerAll()
          {
            for (Hook hook : eachVal (registry_))
              (*hook)();
          }
      };
    
    /**
     * Static internal access to a global (hidden) registry.
     * @note use Meyer's Singleton to allow registration from
     *       a static-init context, without prerequisites.
     */
    inline RandomReseed&
    randomReseedService()
    {
      static RandomReseed registry;
      return registry;
    }
  }//(End)hidden impl.
  
  
  
  uint
  register_at_seq (uint idx, Hook callback)
  {
    randomReseedService().enrol (idx, callback);
    return idx;
  }
  
  void
  reseed_for_test (SeedNucleus& reseedingNucleus)
  {
    lib::defaultGen.reseed (reseedingNucleus);
    randomReseedService().triggerAll();
  }
  
  
} // namespace lib
