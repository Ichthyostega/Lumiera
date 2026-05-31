/*
  RANDOM-RESEED.hpp  -  establish controlled seed for deterministic randomness

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file random-reseed.hpp
 ** A setup to allow for coordinated reseeding of otherwise independent components.
 ** The primary purpose of this setup is to allow for probabilistic unit-testing,
 ** yet including some facilities which are more intricate to setup. These will have
 ** to configure themselves to be registered with this random-reseeding service.
 ** Each registration-»slot« allocates an unique index number, and for each such
 ** index, some random values will be drawn from the global lib::defaultGen.
 ** @remark reseeding for »deterministic randomness« is an intricate topic.
 **   This setup is intended to be used as "one-stop solution" for all basic stuff,
 **   commonly used in unit tests whenever "some random XYZ" seems appropriate.
 **   Obviously, such a solution can only be provisional, and must be augmented,
 **   once new requirements arise. In 2026, using pseudo-random values and chained
 **   hash computations has become quite commonplace in the Lumiera test suite;
 **   since 2024, there is a framework in place to allow for controlled re-seeding
 **   and deterministic computation. However, in practice, the necessary setup
 **   and initialisation was overlooked frequently, and thus we now have to raise
 **   to the next level, by providing this one and central implementation function.
 ** @warning _deliberately_ there is no coordination in place to ensure consistency
 **   of registration or initialisation. This is only a convenient front-end, while
 **   the actual registration and setup remains essentially hard-wired.
 ** @see Test::seedRand()
 ** @see Random_test
 */


#ifndef LIB_RANDOM_RESEED_H
#define LIB_RANDOM_RESEED_H


#include "lib/integral.hpp"


namespace lib {
  
  class SeedNucleus; // see lib/random.hpp
  
  
  /**
   * Install a callback \a Hook to be invoked whenever
   * _reseeding_ of random number generators becomes necessary.
   * @param idx number of a distinct »slot« to allocate;
   *        invocation will happen in ascending order
   * @remark this function can be used to plant a static variable
   *        to ensure registration happens early and once within
   *        some implementation translation unit. The callback
   *        is called way later, typically when the first test
   *        case invokes test::Test::seedRand()
   */
  uint register_at_seq (uint idx, void(*)(void) );
  
  /**
   * Reseed generic randomness used for probabilistic unit-testing.
   * After calling this function, the default random generator in the Lumiera library
   * is reconfigured and reseeded based on the given seeding nucleus; furthermore, a
   * small, hard-wired collection of test facilities will be re-set and re-seeded
   * from this newly configured random source.
   */
  void reseed_for_test (SeedNucleus&);
  
  
} // namespace lib
#endif /*LIB_RANDOM_RESEED_H*/
