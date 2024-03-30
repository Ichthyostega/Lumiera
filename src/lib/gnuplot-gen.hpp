/*
  GNUPLOT-GEN.hpp  -  setup for simplified data visualisation via Gnuplot

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

*/

/** @file gnuplot-gen.hpp
 ** Preconfigured setup for data visualisation with Gnuplot.
 ** The visualisation tool [gnuplot] allows for simple data visualisation
 ** in various formats, integrated into a *NIX commandline work environment.
 ** 
 ** The namespace lib::gnuplot_gen provides ....
 ** 
 ** @todo 3/2024 this is an initial draft, shaped by the immediate need to visualise
 **       [measurement data](\ref vault::gear::test::SchedulerStress_test) collected
 **       while testing the new [Scheduler](\ref scheduler.hpp) implementation.
 ** 
 ** ## Usage
 ** TBW
 ** - blah
 ** - blubb
 ** 
 ** @see GnuplotGen_test
 ** @see SchedulerStress_test
 ** @see text-template.hpp
 ** [gnuplot]: http://gnuplot.info/documentation.html
 */


#ifndef LIB_GNUPLOT_GEN_H
#define LIB_GNUPLOT_GEN_H


//#include "lib/format-util.hpp"
#include "lib/iter-source.hpp"
//#include "lib/util.hpp"

//#include <utility>
//#include <sstream>
#include <string>
//#include <vector>


namespace lib {
namespace gnuplot_gen { ///< preconfigured setup for Gnuplot data visualisation
  
//  using util::toString;
//  using util::isnil;
  using std::string;
//  using std::move;
  using CSVRowIter = lib::IterSource<const string>::iterator;
  
  /**
   * Generate a Gnuplot diagram to visualise the given data points.
   */
  string dataPlot (CSVRowIter&);
  
  
}} // namespace lib::gnuplot_gen
#endif /*LIB_GNUPLOT_GEN_H*/
