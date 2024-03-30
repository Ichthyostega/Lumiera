/*
  GnuplotGen  -  building blocks for simplified data visualisation via Gnuplot

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


/** @file gnuplot-gen.cpp
 ** Definition of building blocks and preconfigured styles for Gnuplot scripts.
 ** The purpose of this setup is to generate visualisations of measurement data easily.
 ** A collection of plotting definitions, together with uniform styles is maintained
 ** within this translation unit, exposing a simple interface where the user only needs
 ** to supply the actual data to be plotted. Intentionally, this collection is started
 ** without much structure or preconceptions and written in a way to encourage adding
 ** more use-cases by copy-and-paste. Following this pragmatic approach, generalisations
 ** and common schemes will emerge eventually.
 ** @todo WIP-WIP 4/2024 first usage as part of Scheduler stress testing.
 */


#include "lib/text-template.hpp"
#include "lib/gnuplot-gen.hpp"

#include <string>

using std::string;


namespace lib {
namespace gnuplot_gen {
  namespace {
    
    const string GNUPLOT_SCATTER_REGRESSION = R"~(#
#
#

set term wxt size 600,800

set datafile separator ",;"

$RunData << _End_of_Data_
"graph length";"duration (ms)";"concurrency";"job time"
49, 24.3764, 5
5 , 4.30955, 5, 700
32, 16.039 , 5, 740
20, 6.47043, 4, 930
39, 19.424 , 4, 888
49, 15.951 , 4, 688
51, 32.7247, 5, 1200
62, 31.4712, 5, 812
15, 13.552 , 4
56, 36.1978, 4
32, 16.4677, 5
57, 22.2576, 6, 833
17, 14.3244, 5, 844
54, 27.4692, 5
46, 12.4055
52, 19.9593
39, 19.4265
41, 22.0513
64, 33.744
2,  3.04284
_End_of_Data_


set style line 1 linetype  1 linewidth 2 linecolor rgb '#1700D4'
set style line 2 linetype  3 linewidth 2 linecolor rgb '#113DD6'
set style line 3 linetype  2 linewidth 2 linecolor rgb '#0B7FCC'
set style line 4 linetype  6 linewidth 2 linecolor rgb '#37999D'
set style line 5 linetype  7 linewidth 2 linecolor rgb '#248269'
set style line 6 linetype 13 linewidth 2 linecolor rgb '#00C443'
set style line 7 linetype 12 linewidth 2 linecolor rgb '#B86E1C'
set style line 8 linetype  5 linewidth 2 linecolor rgb '#762C14'
set style line 9 linetype  4 linewidth 2 linecolor rgb '#AA0519'

set style line 10 linetype 1 linewidth 1 linecolor rgb '#303030'
set style line 11 linetype 0 linewidth 1 linecolor rgb '#A0A0A0' dashtype 3

unset border
set tics nomirror
set grid back linestyle 11

#set size ratio 0.6

set arrow 10 from graph 0,0 to graph 1.04,0 size screen 0.025,15,60 filled ls 10
set arrow 11 from graph 0,0 to graph 0,1.05 size screen 0.025,15,60 filled ls 10

set key autotitle columnheader
set x2label 'X axis'
set ylabel 'Y axis'

set arrow 1 from graph 0, first 1 to graph 1, first 30 nohead ls 9

set multiplot layout 2,1
set lmargin at screen 0.12
set rmargin at screen 0.88

plot $RunData using 1:2 with points linestyle 1
unset arrow 1
unset arrow 10
unset arrow 11
set border 2+8

set yrange [0:8]
set y2range [500:2000]

unset x2label
set format x ""
set ylabel "Y1 axis"
set y2label "Y2 axis" offset -2
set y2tics
plot $RunData using 1:3 with impulses linestyle 3, \
     $RunData using 1:4 with points   linestyle 5 axes x1y2


)~";
    
  }
  
  
  /** */
  string
  dataPlot (CSVRowIter& rowIT)
  {
    UNIMPLEMENTED ("generate gnuplot");
  }
  
}} // namespace lib::gnuplot_gen
