/*
  Appconfig(Test)  -  accessing the always-available AppState singleton

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file §§§
 ** unit test TODO §§§
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

