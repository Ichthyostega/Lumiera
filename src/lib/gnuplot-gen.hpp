/*
  GNUPLOT-GEN.hpp  -  setup for simplified data visualisation via Gnuplot

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file gnuplot-gen.hpp
 ** Preconfigured setup for data visualisation with Gnuplot.
 ** The visualisation tool [gnuplot] allows for simple data visualisation
 ** in various formats, integrated into a *NIX commandline work environment.
 ** 
 ** The namespace lib::gnuplot_gen allows to generate diagrams relying on
 ** some common layout schemes, which can be customised. Data is passed in
 ** as CSV string; the generated Gnuplot script adapts dynamically to the
 ** number of data columns given, where the first column always holds
 ** the common x-axis values. Additional parameters can be added to
 ** the _data binding_ used for script generation; this binding
 ** is comprised of key = value settings in a `Rec<GenNode>`
 ** (Lumiera's »ETD« format for structural data)
 ** 
 ** @todo 3/2024 this is an initial draft, shaped by the immediate need to visualise
 **       [measurement data](\ref vault::gear::test::SchedulerStress_test) collected
 **       while testing the new [Scheduler](\ref scheduler.hpp) implementation.
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
  
  
  
  using ParamRecord = diff::Rec::Mutator;
  
  const string KEY_CSVData     = "CSVData";
  const string KEY_DiagramKind = "DiagramKind";
  
  const string KEY_Term        = "Term";
  const string KEY_TermSize    = "TermSize";
  
  const string KEY_Xtics       = "Xtics";
  const string KEY_Xrange      = "Xrange";
  const string KEY_Yrange      = "Yrange";
  const string KEY_Y2range     = "Y2range";
  const string KEY_Y3range     = "Y3range";
  const string KEY_Xlabel      = "Xlabel";
  const string KEY_Ylabel      = "Ylabel";
  const string KEY_Y2label     = "Y2label";
  const string KEY_Y3label     = "Y3label";
  
  const string KEY_RegrSocket  = "RegrSocket";
  const string KEY_RegrSlope   = "RegrSlope";
  
  
  
  /**
   * Generate a Gnuplot diagram to visualise the given data points.
   */
  string dataPlot (ParamRecord);
  
  inline string
  dataPlot (string csvData)
  {
    return dataPlot (ParamRecord().set (KEY_CSVData, csvData));
  }
  
  
  /**
   * Generate a (X,Y)-scatter plot with regression line
   */
  string scatterRegression (ParamRecord);
  
  inline string
  scatterRegression (string csvData)
  {
    return scatterRegression (ParamRecord().set (KEY_CSVData, csvData));
  }
  
  
}} // namespace lib::gnuplot_gen
#endif /*LIB_GNUPLOT_GEN_H*/
