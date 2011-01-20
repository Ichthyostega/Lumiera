/*
  UtilFloorwrap(Test)  -  verify integer scale remainder wrapping

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
#include "lib/util.hpp"

#include <cmath>
//#include <vector>
#include <iostream>
#include <boost/format.hpp>

using ::Test;
using std::cout;
//using std::rand;
using boost::format;


namespace util {
namespace test {
  
  
  
  namespace{ // Test data and operations
    
    div_t
    floorwrap (int num, int den)
    {
      div_t res = div (num,den);
      if (0 > (num^den) && res.rem)
        { // wrap similar to floor()
          --res.quot;
          res.rem = den - (-res.rem);
        }
      return res;
    }
    

    void
    showWrap (int val, int scale)
      {
        div_t wrap = floorwrap(val,scale);
        cout << format ("% 3d /% 1d =% 1d  %% =% d     floor=% 4.1f  wrap = (%2d,%2d) \n")
                         % val % scale % (val/scale) 
                                       % (val%scale) % floor(double(val)/scale)
                                                                   % wrap.quot % wrap.rem; 
      }
    
  
    
  } // (End) test data and operations
  
  
  
  /**********************************************************************
   * @test Evaluate a custom built integer floor function.
   *       This function is crucial for Lumiera's rule of quantisation
   *       of time values into frame intervals. This rule requires time
   *       points to be rounded towards the next lower frame border always,
   *       irrespective of the relation to the actual time origin.
   *       Contrast this to the built-in integer division operator, which
   *       truncates towards zero.
   * 
   * @note if invoked with an non empty parameter, this test performs
   *       some interesting timing comparisons, which initially were
   *       used to tweak the implementation a bit.
   * @see util.hpp
   * @see QuantiserBasics_test
   */
  class UtilFloorwrap_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          showWrap ( 12,4);
          showWrap ( 11,4);
          showWrap ( 10,4);
          showWrap (  9,4);
          showWrap (  8,4);
          showWrap (  7,4);
          showWrap (  6,4);
          showWrap (  5,4);
          showWrap (  4,4);
          showWrap (  3,4);
          showWrap (  2,4);
          showWrap (  1,4);
          showWrap (  0,4);
          showWrap (- 1,4);
          showWrap (- 2,4);
          showWrap (- 3,4);
          showWrap (- 4,4);
          showWrap (- 5,4);
          showWrap (- 6,4);
          showWrap (- 7,4);
          showWrap (- 8,4);
          showWrap (- 9,4);
          showWrap (-10,4);
          showWrap (-11,4);
          showWrap (-12,4);
        }
      
      
    };
  
  
  
  
  LAUNCHER (UtilFloorwrap_test, "unit common");
  
  
}} // namespace util::test
