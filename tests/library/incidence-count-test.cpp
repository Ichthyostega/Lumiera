/*
  IncidenceCount(Test)  -  observe and evaluate concurrent activations

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

/** @file incidence-count-test.cpp
 ** unit test \ref IncidenceCount_test
 */


#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"//////////////TODO RLY?
#include "lib/incidence-count.hpp"

//#include <string>
#include <thread>


//using std::string;
using std::this_thread::sleep_for;


namespace lib {
namespace test{
  
  
  
  
  /***************************************************************************//**
   * @test verifies capturing and restoring of std::ostream formatting state.
   * @see ios-savepoint.hpp
   */
  class IncidenceCount_test
    : public Test
    {
      void
      run (Arg)
        {
          demonstrate_usage();
          verify_incidentCount();
          verify_multithreadCount();
        }
      
      
      
      /** @test TODO
       * @todo WIP 2/24 🔁 define ⟶ implement
       */
      void
      demonstrate_usage()
        {
        }
      
      
      /** @test TODO verify proper counting of possibly overlapping incidences
       * @todo WIP 2/24 🔁 define ⟶ implement
       */
      void
      verify_incidentCount()
        {
          UNIMPLEMENTED("verify proper counting of possibly overlapping incidences");
        }
      
      
      /** @test TODO verify thread-safe operation under pressure
       * @todo WIP 2/24 🔁 define ⟶ implement
       */
      void
      verify_multithreadCount()
        {
          UNIMPLEMENTED("verify thread-safe operation under pressure");
        }
    };
  
  LAUNCHER (IncidenceCount_test, "unit common");
  
  
}} // namespace lib::test

