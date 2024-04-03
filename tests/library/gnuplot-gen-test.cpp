/*
  GnuplotGen(Test)  -  verify the minimalistic text substitution engine

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

/** @file gnuplot-gen-test.cpp
 ** unit test \ref GnuplotGen_test
 */



#include "lib/test/run.hpp"
#include "lib/gnuplot-gen.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

using lib::stat::CSVData;
using util::contains;

namespace lib {
namespace test{
  
  
  
  /***************************************************************************//**
   * @test verify data visualisation by generated Gnuplot scripts
   *       - generate a simple pot
   *       - generate a scatter plot with regression line
   * @see  gnuplot-gen.hpp
   * @see  TextTemplate_test
   */
  class GnuplotGen_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpeUsage();
          plot_scatter_regression();
          verify_customisation();
        }
      
      
      /** @test Create simple (x,y) data point visualisation */
      void
      simpeUsage()
        {
          string gnuplot = gnuplot_gen::dataPlot(
              CSVData{{"step","fib"}
                     ,{{0,1}
                      ,{1,1}
                      ,{2,2}
                      ,{3,3}
                      ,{4,5}
                      ,{5,8}
                      ,{6,13}
                      ,{7,21.55}
                     }});
//        cout << gnuplot <<endl;
          //Hint: gnuplot -p <scriptfile>
          
          CHECK (contains (gnuplot, "set datafile separator \",;\""));
          CHECK (contains (gnuplot, "\"step\",\"fib\""));
          CHECK (contains (gnuplot, "7,21.55"));
          CHECK (contains (gnuplot, "set key autotitle columnheader"));
          CHECK (contains (gnuplot, "plot for [i=2:*] $RunData using 1:i with points"));
        }
      
      
      
      /** @test Create a (x,y) scatter plot with regression line
       *      - in the simple case, there is only one diagram
       *      - use the `stats` command to let Gnuplot calculate the linear regression
       *      - draw a regression line as function representing the regression model
       */
      void
      plot_scatter_regression()
        {
          string gnuplot = gnuplot_gen::scatterRegression(
              CSVData{{"step","fib"}
                     ,{{0,1}
                      ,{1,1}
                      ,{2,2}
                      ,{3,3}
                      ,{4,5}
                      ,{5,8}
                      ,{6,13}
                      ,{7,21.55}
                     }});
//        cout << gnuplot <<endl;
          
          CHECK (contains (gnuplot, "\"step\",\"fib\""));
          CHECK (contains (gnuplot, "7,21.55"));
          CHECK (contains (gnuplot, "regLine(x) = STATS_slope * x + STATS_intercept"));
          CHECK (contains (gnuplot, "plot $RunData using 1:2 with points"));
          
          
          gnuplot = gnuplot_gen::scatterRegression(
              CSVData{{"step","fib","one","two","three"}
                     ,{{0,1 , 1.0, 170,200}
                      ,{1,1 , 0.1, 160,210}
                      ,{2,2 , 1.1, 150,220}
                      ,{3,3 , 0.0, "" ,230}
                      ,{4,5 , 1.1, 130,240}
                      ,{5,8 , 1.2, 120,250}
                      ,{6,13, 1.3, 110    }
                      ,{7,21, 1.4, 100    }
                     }});
//        cout << gnuplot <<endl;
          
          // more than one data row given => using multiplot layout
          CHECK (contains (gnuplot, "set multiplot"));
          CHECK (contains (gnuplot, "\"step\",\"fib\",\"one\",\"two\",\"three\""));
          CHECK (contains (gnuplot, "0,1,1,170,200"));
        }
      
      
      /** @test various customisations through additional parameters
       *      - a custom defined regression line
       *      - use a specific output »term« and specify canvas size
       *      - define the common horizontal data range and x-tic spacing
       *      - define display ranges for 3 different Y-axis
       *      - define custom labels for all axes
       * @note  when using additional parameters, csv data
       *        must also be given explicitly as `KEY_CSVData`
       */
      void
      verify_customisation()
        {
          string csv =
              CSVData{{"abscissa","points","e1","e2","e3"}
                     ,{{1,1 , 1.1,"" ,210}
                      ,{2,2 , 1.2,150,220}
                      ,{3,5 , 5.5,140}
                     }};
          using namespace gnuplot_gen;
          string gnuplot = scatterRegression(
              ParamRecord()
                .set(KEY_CSVData , csv)
                .set(KEY_RegrSocket, 3)
                .set(KEY_RegrSlope, -1.5)
                .set(KEY_Xtics  ,    2)
                .set(KEY_Xrange , "-1:5.5")
                .set(KEY_Yrange , "0:6")
                .set(KEY_Y2range, "1.1:1.5")
                .set(KEY_Y3range, "100:*")
                .set(KEY_Xlabel , "common axis")
                .set(KEY_Ylabel , "measurement")
                .set(KEY_Y2label, "auxiliary-1")
                .set(KEY_Y3label, "auxiliary-2")
                .set(KEY_TermSize, "500,800")
                );
//        cout << gnuplot <<endl;
          
          CHECK (contains (gnuplot, "set term wxt size 500,800"));
          CHECK (contains (gnuplot, "\"abscissa\",\"points\",\"e1\",\"e2\",\"e3\""));
          CHECK (contains (gnuplot, "regLine(x) = -1.5 * x + 3"));
          CHECK (contains (gnuplot, "set xlabel 'common axis'"));
          CHECK (contains (gnuplot, "set ylabel 'measurement'"));
          CHECK (contains (gnuplot, "set xrange [-1:5.5]"));
          CHECK (contains (gnuplot, "set yrange [0:6]"));
          CHECK (contains (gnuplot, "set yrange [1.1:1.5]"));
          CHECK (contains (gnuplot, "set ylabel 'auxiliary-1'"));
          CHECK (contains (gnuplot, "set y2range [100:*]"));
          CHECK (contains (gnuplot, "set y2label 'auxiliary-2'"));
        }
    };
  
  LAUNCHER (GnuplotGen_test, "unit common");
  
  
}} // namespace lib::test
