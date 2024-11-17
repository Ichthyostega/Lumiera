/*
  ZombieCheck(Test)  -  verify a trap for accessing deceased objects

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file zombie-check-test.cpp
 ** unit test \ref Singleton_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/zombie-check.hpp"
#include "lib/util.hpp"

#include <string>

using util::contains;
using util::isnil;
using std::string;


namespace lib {
namespace test{
  
  
  
  
  
  
  /**********************************************************************//**
   * @test verify the ability to trap access to deceased objects.
   * @remark For actual usage, the zombie detector needs to be planted into
   *         static memory. This test can only verify the trigger mechanics.
   * @see zombie-check.hpp
   * @see lib::DependencyFactory::zombieCheck
   */
  class ZombieCheck_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          auto zombieID = randStr(50);
          ZombieCheck trap{zombieID};
          CHECK (not trap);
          
          trap.~ZombieCheck(); // note: unconventional kill
          CHECK (trap);       //  accessing deceased object...
          
          try {             //
              trap();      //     trigger the tripwire
            }
          catch(lumiera::error::Fatal& casuality)
            {
              string obituary = casuality.what();
              CHECK (not isnil (obituary));
              CHECK (    contains (obituary, zombieID.substr(0,41)));
              CHECK (not contains (obituary, zombieID));
            }
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ZombieCheck_test, "unit common");
  
  
  
}} // namespace lib::test
