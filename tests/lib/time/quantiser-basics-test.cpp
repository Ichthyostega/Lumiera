/*
  QuantiserBasics(Test)  -  a demo quantiser to cover the basic quantiser API

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
//#include "lib/test/test-helper.hpp"
//#include "lib/time/timequant.hpp"
#include "lib/time/quantiser.hpp"
//#include "lib/time/display.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/join.hpp>
#include <iostream>
//#include <cstdlib>

using boost::lexical_cast;
using util::isnil;
//using util::contains;
//using std::rand;
using std::cout;
using std::endl;

//using boost::algorithm::join;


namespace lib {
namespace time{
namespace test{
  
  namespace {
    
    const uint MAX_FRAMES = 25*500;
    const uint MAX_DIRT   = 50;
    
    const FSecs F25(1,25);
    
  }
  
  
  
  /********************************************************
   * @test cover the basic Quantiser API.
   * This test uses a special quantiser implementation
   * with hard coded behaviour to demonstrate and verify
   * the usage of a quantiser entity in isolation.
   */
  class QuantiserBasics_test : public Test
    {
    
      virtual void
      run (Arg) 
        {
          checkSimpleQuantisation ();
        }
      
      
      void
      checkSimpleQuantisation ()
        {
          FixedFrameQuantiser fixQ(25);
          
          uint frames = (rand() % MAX_FRAMES);
          FSecs dirt  = (F25 / (rand() % MAX_DIRT));
          
          Time rawTime = FSecs(frames, 25) + dirt;
          CHECK (Time(    frames*F25) <= rawTime);
          CHECK (Time((frames+1)*F25) >  rawTime);
          
          Time quantTime = fixQ.align (rawTime);
          
          CHECK (Time(frames*F25) == quantTime);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (QuantiserBasics_test, "unit common");
  
  
  
}}} // namespace lib::time::test
