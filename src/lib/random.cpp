/*
  random.cpp  -  storage and implementation for random number framework

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file random.cpp
 ** Implementation details and common storage for random number generation.
 ** @todo 3/2024 work out how some executions or performances are configured reproducibly.
 */


#include "lib/random.hpp"

#include <string>

using std::string;


namespace lib {
  namespace {
    
    const string ENTROPY_SOURCE_SPEC{"/dev/random"};
    
    
    class EntropyNucleus
      : public SeedNucleus
      {
        std::random_device entropySource_;
        
        uint64_t
        getSeed()  override
          {
            return entropySource_();
          }
        
      public:
        EntropyNucleus(string const& SPEC)
          : entropySource_{SPEC}
          { }
      };
    
    class EternalNucleus
      : public SeedNucleus
      {
        uint64_t
        getSeed()  override
          {
            return LIFE_AND_UNIVERSE_4EVER;
          }
      };
    
    /** static global entropy source instance */
    EntropyNucleus entropyNucleus{ENTROPY_SOURCE_SPEC};
    
    /// @todo this one should somehow be configurable
    EternalNucleus eternalNucleus;
    
    Random::Seed defaultNucleus{defaultGen};
  }
  
  Random defaultGen{eternalNucleus};
  Random entropyGen{entropyNucleus};
  
  SeedNucleus&
  seedFromDefaultGen()
  {
    return defaultNucleus;
  }
  
  
  SeedNucleus::~SeedNucleus() { }
  
  void
  randomiseRandomness()
  {
    entropyGen.reseed(entropyNucleus);
    defaultGen.reseed(entropyNucleus);
  }
  
  
} // namespace lib
