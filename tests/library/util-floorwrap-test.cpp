/*
  UtilFloorwrap(Test)  -  verify integer scale remainder wrapping

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file util-floorwrap-test.cpp
 ** unit test \ref UtilFloorwrap_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util-quant.hpp"

#include <cmath>
#include <boost/lexical_cast.hpp>

using ::Test;
using boost::lexical_cast;
using util::typeStr;
using util::floorwrap;


namespace util {
namespace test {
  
  
  
  
  
  /***********************************************************************//**
   * @test Verify a custom built integer scale division and wrapping function.
   *       This function is relevant for decimating values into a given scale,
   *       like splitting time measurements in hours, minutes, seconds etc.
   *       Basically, in Lumiera the quantisation into a scale is always
   *       done with the same orientation, irrespective of the zero point
   *       on the given scale. Contrast this to the built-in integer
   *       division and modulo operators working symmetrical to zero.
   * @see lib/util.hpp
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
          cout << "--------"<< typeStr<I>()
               << "--------"<< range<<"/"<<scale<<endl;
          for (I i=range; i >=-range; --i)
            showWrap (i, scale);
        }
      
      template<typename I>
      void
      showWrap (I val, I scale)
        {
          IDiv<I> wrap = floorwrap(val,scale);
          cout << _Fmt ("% 3d /% 1d =% 1d  %% =% d     floor=% 4.1f  wrap = (%2d,%2d)\n")
                         % val % scale % (val/scale)
                                       % (val%scale) % floor(double(val)/scale)
                                                                   % wrap.quot % wrap.rem;
        }
    };
  
  
  
  
  LAUNCHER (UtilFloorwrap_test, "unit common");
  
  
}} // namespace util::test
