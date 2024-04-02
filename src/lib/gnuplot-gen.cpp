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
#include "lib/text-template-gen-node-binding.hpp"
#include "lib/gnuplot-gen.hpp"
#include "lib/format-util.hpp"

#include <string>

using std::string;


namespace lib {
namespace gnuplot_gen {
  
  namespace { // Template and defaults definitions for diagram generation
    
    
    const string GNUPLOT_CommonStyleDef = R"~(#
#---common-styles-for-plots-from-Lumiera---------
set style line 1 linetype  1 linewidth 2 linecolor rgb '#240CC3'
set style line 2 linetype  3 linewidth 2 linecolor rgb '#1149D6'
set style line 3 linetype  2 linewidth 2 linecolor rgb '#0B7FCC'
set style line 4 linetype  6 linewidth 2 linecolor rgb '#37999D'
set style line 5 linetype  7 linewidth 2 linecolor rgb '#248269'
set style line 6 linetype 13 linewidth 2 linecolor rgb '#00C443'
set style line 7 linetype 12 linewidth 2 linecolor rgb '#B86E1C'
set style line 8 linetype  5 linewidth 2 linecolor rgb '#762C14'
set style line 9 linetype  4 linewidth 2 linecolor rgb '#AA0519'

set style line 10 linetype 1 linewidth 1 linecolor rgb '#303030'
set style line 11 linetype 0 linewidth 1 linecolor rgb '#A0A0A0' dashtype 3
#---(End)styles----------------------------------
)~";
    
    const string GNUPLOT_AxisGridSetup = R"~(#
#---axis-and-grid-setup--------------------------
unset border
set tics nomirror
set grid back linestyle 11

set arrow 10 from graph 0,0 to graph 1.04,0 size screen 0.025,15,60 filled ls 10
set arrow 11 from graph 0,0 to graph 0,1.08 size screen 0.025,15,60 filled ls 10
)~";
    
    
    const string GNUPLOT_BASIC_PLOT_DEF = R"~(#
#
#   GNUPLOT - data plot from Lumiera
#

${if Term}set term ${Term} ${
if TermSizeSpec}size ${TermSizeSpec}${endif}${
endif Term}

set datafile separator ",;"

####---------Data--------------------------------
$RunData << _End_of_Data_
${CSVData}
_End_of_Data_


${CommonStyleDef}
${AxisGridSetup}

${if XLabel
}set xlabel '${XLabel}'
${else
}stats $RunData using (abscissaName=strcol(1)) every ::0::0 nooutput

set xlabel abscissaName
${end if XLabel
}${if YLabel
}set ylabel '${YLabel}' ${end if YLabel
}
${if Yrange}
set yrange [${Yrange}]
${endif
}
set key autotitle columnheader tmargin

)~";
    
    const string GNUPLOT_SIMPLE_DATA_PLOT = R"~(#
####---------------------------------------------
plot for [i=2:*] $RunData using 1:i with ${DiagramKind} linestyle i-1

)~";
    
    
    const string GNUPLOT_SCATTER_REGRESSION = R"~(#
#
####---------Scatter-Regression-Plot-------------
#
stats $RunData using 1:2 nooutput

# draw regression line as arrow
regLine(x) = STATS_slope * x + STATS_intercept
set arrow 1 from graph 0, first regLine(STATS_min_x) \
              to graph 1, first regLine(STATS_max_x) \
              nohead linestyle 9

plots = STATS_columns - 1
# Adjust layout based on number of data sequences;
# additional sequences placed into secondary diagram
#
if (plots > 1) {
    set multiplot layout 2,1     # 2 rows 1 column
    set lmargin at screen 0.12   # fixed margins to align diagrams
    set rmargin at screen 0.88
}
####-------------------------------
plot $RunData using 1:2 with points linestyle 1

if (plots > 1) {
    # switch off decorations for secondary diagram
    unset arrow 1
    unset arrow 10
    unset arrow 11
    set border 2+8

${if Y2range}
    set yrange [${Y2range}]
${endif
}    unset x2label
    set format x ""
${if Y2label
}    set ylabel '${Y2label}' ${endif
}
    if (plots <= 2) {
        ####---------------------------------
        plot $RunData using 1:3 with impulses linestyle 3

    } else {
        # more than one additional data sequence
        #
${if Y3range
}        set y2range [${Y3range}] ${endif
}        set y2tics
${if Y3label
}        set y2label '${Y3label}'  offset -1 ${endif
}
        ####---------------------------------------------
        plot             $RunData using 1:3 with impulses linestyle 3, \
             for [i=4:*] $RunData using 1:i with points   linestyle 5+(i-4) axes x1y2
    }
}
)~";
    
    
  }//(End)template and defaults definitions
  
  
  
  /**
   * @remark each column of the given data is featured as sequence
   * over the first column interpreted as common abscissa. The name
   * of the abscissa and the row names in the legend are extracted
   * from the header names expected in the first row of CSV data.
   */
  string
  dataPlot (ParamRecord params)
  {
    TextTemplate plot{GNUPLOT_BASIC_PLOT_DEF
                     +GNUPLOT_SIMPLE_DATA_PLOT};
    
    params.set ("CommonStyleDef", GNUPLOT_CommonStyleDef)
          .set ("AxisGridSetup",  GNUPLOT_AxisGridSetup)
          .set (KEY_DiagramKind,  "points")
          ;
    return plot.render (params.genNode());
  }
  
  
  string
  scatterRegression (ParamRecord params)
  {
    TextTemplate plot{GNUPLOT_BASIC_PLOT_DEF
                     +GNUPLOT_SCATTER_REGRESSION};
    
    params.set ("CommonStyleDef", GNUPLOT_CommonStyleDef)
          .set ("AxisGridSetup",  GNUPLOT_AxisGridSetup)
          ;
    return plot.render (params.genNode());
  }
  
}} // namespace lib::gnuplot_gen
