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
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <cmath>
#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using ::Test;
using std::cout;
using std::endl;
using boost::format;
using boost::lexical_cast;
using lib::test::showType;


namespace util {
namespace test {
  
  
  
  
  
  /***************************************************************************
   * @test Verify a custom built integer scale division and wrapping function.
   *       This function is relevant for decimating values into a given scale,
   *       like splitting time measurements in hours, minutes, seconds etc.
   *       Basically, in Lumiera the quantisation into a scale is always
   *       done with the same orientation, irrespective of the zero point
   *       on the given scale. Contrast this to the built-in integer
   *       division and modulo operators working symmetrical to zero.
   * @see util.hpp
   * @see TimeFormats_test
   */
  class UtilFloorwrap_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          int range =  0 < arg.size()? lexical_cast<int> (arg[0]) : 12;
          int scale =  1 < arg.size()? lexical_cast<int> (arg[1]) :  4;
          
          checkWrap       (range,  scale);
          checkWrap       (range, -scale);
          checkWrap<long> (range,  scale);
          checkWrap<long> (range, -scale);
        }
      
      
      template<typename I>
      void
      checkWrap (I range, I scale)
        {
          cout << "--------"<< showType<I>()
               << "--------"<< range<<"/"<<scale<<endl;
          for (I i=range; i >=-range; --i)
            showWrap (i, scale);
        }
      
      template<typename I>
      void
      showWrap (I val, I scale)
        {
          IDiv<I> wrap = floorwrap(val,scale);
          cout << format ("% 3d /% 1d =% 1d  %% =% d     floor=% 4.1f  wrap = (%2d,%2d)\n")
                           % val % scale % (val/scale) 
                                         % (val%scale) % floor(double(val)/scale)
                                                                     % wrap.quot % wrap.rem;
        }
    };
  
  
  
  
  LAUNCHER (UtilFloorwrap_test, "unit common");
  
  
}} // namespace util::test
