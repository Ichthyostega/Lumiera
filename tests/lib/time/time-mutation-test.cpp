/*
  TimeMutation(Test)  -  explicitly changing time spcifications

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
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
//#include "lib/time/display.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <cstdlib>
#include <string>

using boost::lexical_cast;
using util::isnil;
//using std::rand;
using std::cout;
using std::endl;
using std::string;

//using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;

namespace lib {
namespace time{
namespace test{
  
  namespace {
    inline string
    pop (Arg arg)
    {
      if (isnil (arg)) return "";
      string entry = arg[0];
      arg.erase (arg.begin());
      return entry;
    }
  }
  
  
  /****************************************************************
   * @test cover all basic cases for mutating a time specification.
   *       - change to a given value
   *       - change by an offset
   *       - change using a grid value
   *       - apply an (grid) increment
   */
  class TimeMutation_test : public Test
    {
      gavl_time_t
      random_or_get (string arg)
        {
          if (isnil(arg))
            return 1 + (rand() % 10000);
          else
            return lexical_cast<gavl_time_t> (arg);
        }
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue o (random_or_get (pop(arg)));
          TimeValue c (random_or_get (pop(arg)));
          
          PQuant fixQ (new FixedFrameQuantiser(FrameRate::PAL));
          QuTime qVal (o, fixQ);
          FrameNr count(qVal);
          
          mutate_by_Value (o, c);
          mutate_by_Offset (o, Offset(c));
          mutate_quantised (o, qVal);
          mutate_by_Increment(o, count);
        } 
      
      
      void
      mutate_by_Value(TimeValue o, TimeValue change)
        {
        }
      
      
      void
      mutate_by_Offset (TimeValue o, Offset change)
        {
        }
      
      
      void
      mutate_quantised (TimeValue o, QuTime change)
        {
        }
      
      
      void
      mutate_by_Increment (TimeValue o, int change)
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeMutation_test, "unit common");
  
  
  
}}} // namespace lib::time::test
