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


#include "lib/stat/csv.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/gen-node.hpp"

#include <string>
#include <vector>
#include <tuple>

using std::string;

namespace lib {
namespace gnuplot_gen { ///< preconfigured setup for Gnuplot data visualisation
  
  
  /**
   * Wrapper to simplify notation in tests.
   * Accepts data suitable for representation as CSV
   * - either as an std::initializer_list<string> for pre-formatted rows
   * - or a list of strings for the header, and then a list of data tuples,
   *   which will be rendered into data rows in CSV format
   * Since this wrapper is-a `vector<string>`, the rows can be retrieved
   * directly and then rendered, or the \ref operator string() can be used
   * to retrieve the complete data set in a single string of data lines.
   */
  struct CSVData
    : std::vector<string>
    {
      CSVData (std::initializer_list<string> lines)
        : vector<string>(lines)
        { }
      
      template<class DAT>
      CSVData (std::initializer_list<string> header
              ,std::initializer_list<std::initializer_list<DAT>> data)
        {
          resize (data.size()+1);
          string line;
          for (string key : header)
            stat::appendCsvField (line, key);
          emplace_back (move(line));
          for (auto& row : data)
            {
              line = "";
              for (DAT const& val : row)
                stat::appendCsvField (line, val);
              emplace_back (move(line));
            }
        }
      
      // standard copy operations acceptable
      
      
      operator string()  const
        {
          return util::join (*this, "\n");
        }
    };
  
  
  using ParamRecord = diff::Rec::Mutator;
  
  const string KEY_CSVData     = "CSVData";
  const string KEY_DiagramKind = "DiagramKind";
  
  
  
  /**
   * Generate a Gnuplot diagram to visualise the given data points.
   */
  string dataPlot (ParamRecord);
  string dataPlot (string csvData) { return dataPlot (ParamRecord().set (KEY_CSVData, csvData)); }
  
  
}} // namespace lib::gnuplot_gen
#endif /*LIB_GNUPLOT_GEN_H*/
