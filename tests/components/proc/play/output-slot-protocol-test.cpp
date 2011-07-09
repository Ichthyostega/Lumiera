/*
  OutputSlotProtocol(Test)  -  covering the basic usage cycle of an output slot

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
//#include "lib/util.hpp"
#include "proc/play/diagnostic-output-slot.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;


namespace engine{
namespace test  {
  
//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
  
  
  namespace { // Test fixture
    
  }
  
  
  /*******************************************************************
   * @test basic render node properties and behaviour.
   */
  class OutputSlotProtocol_test : public Test
    {
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("build a mock output slot and perform a full lifecycle");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputSlotProtocol_test, "unit player");
  
  
  
}} // namespace engine::test
