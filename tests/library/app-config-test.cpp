/*
  Appconfig(Test)  -  accessing the always-available AppState singleton

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



#include "lib/test/run.hpp"
#include "include/config-facade.h"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

using lib::Literal;
using util::isnil;



namespace lumiera {
namespace test {
  
  
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
        string ver = Config::get(key);
        CHECK (!isnil(ver));
      }
    };
  
  
  LAUNCHER (Appconfig_test, "function common");
  
  
}} // namespace util::test

