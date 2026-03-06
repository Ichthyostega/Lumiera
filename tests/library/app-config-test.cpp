/*
  AppConfig(Test)  -  accessing the always-available Voyage singleton

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file app-config-test.cpp
 ** unit test \ref Appconfig_test
 */



#include "test/run.hpp"
#include "vessel/spine/config-facade.h"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>

using lib::Literal;
using util::isnil;



namespace vessel {
namespace test {
  
  using spine::Config;
  
  
  class Appconfig_test : public Test
    {
      virtual void
      run (Arg)
        {
          fetchSetupValue();
        }
      
      
      /** @test accessing a value from setup.ini */
      void fetchSetupValue ()
      {
        Literal key("Lumiera.version");
        std::string ver = Config::get(key);
        CHECK (not isnil(ver));
      }
    };
  
  
  LAUNCHER (Appconfig_test, "function common");
  
  
}} // namespace vessel::test

