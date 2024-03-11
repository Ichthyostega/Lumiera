/*
  random.cpp  -  storage and implementation for random number framework

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

* *****************************************************/


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
  }
  
  
  SeedNucleus::~SeedNucleus() { }
  
  Random defaultGen{eternalNucleus};
  Random entropyGen{entropyNucleus};
  
  
  void
  randomiseRandomness()
  {
    entropyGen.randomise(entropyNucleus);
    defaultGen.randomise(entropyNucleus);
  }
  
} // namespace lib
