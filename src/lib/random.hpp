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
#include "lib/hash-value.h"
#include "lib/nocopy.hpp"

#include <random>


namespace lib {
  namespace {
    inline uint constexpr
    _iBOUND()
    {
      return 1u+uint(std::numeric_limits<int>::max());
    }
  }
  
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
    {
      GEN generator_;
      
    public:
      /** Build new generator, drawing seed from a virtual seed source */
      RandomSequencer (SeedNucleus&);
      
      /** Build new generator, drawing a seed from given parent generator */
      template<class G>
      RandomSequencer (RandomSequencer<G>&);
      
      // default copy operations (can copy and assign a state)
      
      
      int      i(uint bound =_iBOUND());                   ///< drop-in replacement for `rand() % bound`
      int      i32();                                      ///< random number from _full integer range_ (incl. negative values)
      uint64_t u64();                                      ///< random 64bit number from full range.
      double   uni();                                      ///< random double drawn from interval `[0.0 ... 1.0[`
      double   range (double start, double bound);         ///< random double from designated interval (upper bound excluded)
      double   normal(double mean=0.0, double stdev=1.0);  ///< normal distribution (gaussian)
      HashVal  hash();                                     ///< _non-zero_ hash value from full 64bit range
      
      /** generic adapter: draw next number to use the given distribution */
      template<class DIST>
      auto distribute(DIST);
      
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
   * PRNG engine to use by default: 64bit Mersenne twister.
   */
  using Random = RandomSequencer<std::mt19937_64>;
  
  
  /** a global default RandomSequencer for mundane purposes */
  extern Random defaultGen;
  
  /** a global RandomSequencer seeded with real entropy */
  extern Random entropyGen;
  
  
  
  /* ===== convenience accessors ===== */
  
  /** @return a random integer ∈ [0 ... bound[  */
  inline int
  rani (uint bound  =_iBOUND())
    {
      return defaultGen.i(bound);
    }
  
  /** @return a random double ∈ [start ... bound[  */
  inline double
  ranRange (double start, double bound)
    {
      return defaultGen.range (start, bound);
    }
  
  inline double
  ranNormal(double mean =0.0, double stdev =1.0)
    {
      return defaultGen.normal (mean, stdev);
    }
  
  /** @return a random *non-zero* HashVal  */
  inline lib::HashVal
  ranHash()
    {
      return defaultGen.hash();
    }
  
  
  /** inject true randomness into the #defaultGen */
  void randomiseRandomness();
  
  /** draw seed another Generator from the default RandomSequencer  */
  SeedNucleus& seedFromDefaultGen();
  
  
  
  
  /* ===== Implementation details ===== */
  
  template<class GEN>
  inline
  RandomSequencer<GEN>::RandomSequencer (SeedNucleus& nucleus)
    : generator_{nucleus.getSeed()}
    { }
  
  template<class GEN>
  template<class G>
  inline
  RandomSequencer<GEN>::RandomSequencer (RandomSequencer<G>& parent)
    : generator_{RandomSequencer<GEN>::Seed{parent}.getSeed()}
    { }
  
  
  template<class GEN>
  inline void
  RandomSequencer<GEN>::reseed (SeedNucleus& nucleus)
  {
    generator_.seed (nucleus.getSeed());
  }
  
  
  template<class GEN>
  template<class DIST>
  inline auto
  RandomSequencer<GEN>::distribute (DIST distribution)
    {
      return distribution (generator_);
    }
  
  /** @param bound upper bound (exclusive!) */
  template<class GEN>
  inline int
  RandomSequencer<GEN>::i (uint bound)
    {
      if (!bound) bound=1;
      --bound;
      uint upper{std::numeric_limits<int>::max()};
      upper = bound < upper? bound : upper;
      return distribute (std::uniform_int_distribution<int> (0, upper));
    }
  
  template<class GEN>
  inline int
  RandomSequencer<GEN>::i32()
    {
      return distribute (std::uniform_int_distribution<int> {std::numeric_limits<int>::min()
                                                            ,std::numeric_limits<int>::max()});
    }
  
  template<class GEN>
  inline uint64_t
  RandomSequencer<GEN>::u64()
    {
      return distribute (std::uniform_int_distribution<uint64_t> {std::numeric_limits<uint64_t>::min()
                                                                 ,std::numeric_limits<uint64_t>::max()});
    }
  
  template<class GEN>
  inline double
  RandomSequencer<GEN>::uni()
    {
      return range (0.0, 1.0);
    }
  
  template<class GEN>
  inline double
  RandomSequencer<GEN>::range (double start, double bound)
    {
      return distribute (std::uniform_real_distribution<double>{start,bound});
    }
  
  template<class GEN>
  inline double
  RandomSequencer<GEN>::normal (double mean, double stdev)
    {
      return distribute (std::normal_distribution<double>{mean,stdev});
    }
  
  template<class GEN>
  inline HashVal
  RandomSequencer<GEN>::hash()
    {
      return distribute (std::uniform_int_distribution<lib::HashVal>{lib::HashVal(1)});
    }
  
  
  
  /**
   * Adapter to protect against data corruption caused by concurrent access.
   * Random number generators in general are _not thread safe;_ when used from
   * several threads concurrently, it is not a question _if_, but only a question
   * _when_ the internal state will become corrupted, leading to degraded and biased
   * distributions of produced numbers. For some usage scenarios however, ignoring
   * this fact and still using a single generator from several threads may be acceptable,
   * if the quality of the distribution actually does not matter and only some diffusion
   * of numbers is required (e.g. adding a random sleep interval). But there is a catch:
   * whenever the value range of generated numbers is less than the total range of the used
   * data representation, then corruption of the internal state may lead to producing numbers
   * outside the defined range. This adapter can be used to safeguard against this scenario.
   * @remark typically using a 64bit generator on a 64bit platform is inherently safe, yet
   *   using a 32bit generator may rely on 64bit values internally, and then this problem
   *   may be triggered. See RandomConcurrent_test with the 32bit Mersenne twister as demo.
   */
  template<class GEN>
  class CappedGen
    : public GEN
    {
      public:
      using GEN::GEN;
      
      typename GEN::result_type
      operator()()
        {
          if constexpr (GEN::max() < std::numeric_limits<typename GEN::result_type>::max())
              return GEN::operator()() % (GEN::max()+1);
          else
              return GEN::operator()();
        }
    };
  
  template<class GEN>
  auto
  buildCappedSubSequence (RandomSequencer<GEN>& src)
    {
      typename RandomSequencer<GEN>::Seed seedChain(src);
      RandomSequencer<CappedGen<GEN>> subSeq{seedChain};
      return subSeq;
    }
  
  
  
} // namespace lib
#endif /*LIB_RANDOM_H*/
