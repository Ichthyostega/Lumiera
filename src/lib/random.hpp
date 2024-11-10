/*
  RANDOM.hpp  -  support for random number generation with controlled seed

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


/** @file random.hpp
 ** Generating (pseudo) random numbers with controlled seed.
 ** As an extension on top of the [C++ random number framework], several instances
 ** of random number sequence generators can be easily created with a controlled seed.
 ** For simplified usage, two default instances are exposed as global variable
 ** - lib::defaultGen uses fixed seeding (planned: make this configurable)
 ** - lib::entropyGen always uses true randomness as seed value.
 ** [C++ random number framework]: https://en.cppreference.com/w/cpp/numeric/random
 ** @see Random_test
 */


#ifndef LIB_RANDOM_H
#define LIB_RANDOM_H


#include "lib/integral.hpp"
#include "lib/nocopy.hpp"

#include <random>


namespace lib {
  
  /** Establishes a seed point for any instance or performance. */
  class SeedNucleus
    {
    public:
      virtual ~SeedNucleus();     ///< this is an interface
      virtual uint64_t getSeed()  =0;
    };
  
  /**
   * Access point to a selection of random number sources.
   * For each kind of performance or usage, a common execution scheme
   * is established to initiate generated number sequences, allowing
   * for seemingly random yet reproducible behaviour — or for actually
   * contingent behaviour when necessary. Client code should rely on
   * [Dependency-Injection](\ref lib::Depend) or the static accessors.
   */
  template<class GEN>
  class RandomSequencer
    : util::NonCopyable
    {
      std::uniform_int_distribution<int>      uniformI_;
      std::uniform_int_distribution<uint64_t> uniformU_;
      std::uniform_real_distribution<double>  uniformD_;
      
      GEN generator_;
      
    public:
      /** Random instances are created as part of an execution scheme */
      RandomSequencer(SeedNucleus&);
      
      int      i32() { return uniformI_(generator_); }
      uint64_t u64() { return uniformU_(generator_); }
      double   uni() { return uniformD_(generator_); }
      
      /** inject controlled randomisation */
      void reseed (SeedNucleus&);
      
      /** wrapper to use this generator for seeding other generators */
      class Seed;
    };
  
  template<class GEN>
  class RandomSequencer<GEN>::Seed
    : public SeedNucleus
    {
      RandomSequencer& srcGen_;
    public:
      Seed(RandomSequencer& parent)
        : srcGen_{parent}
        { }
      
      uint64_t
      getSeed() override
        {
          return srcGen_.u64();
        }
    };
  
  
  /**
   * PRNG engine to use by default: 64bit mersenne twister.
   */
  using Random = RandomSequencer<std::mt19937_64>;
  
  
  /** a global default RandomSequencer for mundane purposes */
  extern Random defaultGen;
  
  /** a global RandomSequencer seeded with real entropy */
  extern Random entropyGen;
  
  
  /* ===== convenience accessors ===== */
  
  inline int      rani() { return defaultGen.i32(); }  ///< using default params: min ≡ 0, max ≡ numeric_limits
  inline uint64_t ranu() { return defaultGen.u64(); }
  inline double   runi() { return defaultGen.uni(); }
  
  
  /** inject true randomness into the #defaultGen */
  void randomiseRandomness();
  
  /** draw seed another Generator from the default RandomSequencer  */
  SeedNucleus& seedFromDefaultGen();
  
  
  
  /* ===== Implementation details ===== */
  
  template<class GEN>
  inline
  RandomSequencer<GEN>::RandomSequencer (SeedNucleus& nucleus)
    : uniformI_{0}
    , uniformU_{0}
    , uniformD_{}
    , generator_{nucleus.getSeed()}
    { }
  
  
  template<class GEN>
  inline void
  RandomSequencer<GEN>::reseed (SeedNucleus& nucleus)
  {
    generator_.seed (nucleus.getSeed());
  }
  
  
} // namespace lib
#endif /*LIB_RANDOM_H*/
