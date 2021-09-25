/*
 *  statistic - helpers for generic statistics calculations
 *
 *  Copyright 2021, Hermann Vosseler <Ichthyostega@web.de>
 *
 *  This file is part of the Yoshimi-Testsuite, which is free software:
 *  you can redistribute and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  Yoshimi-Testsuite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with yoshimi.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************/


/** @file statistic.cpp
 ** Support for generic statistics calculations.
 ** 
 ** @todo WIP as of 9/21
 ** 
 */



#ifndef TESTRUNNER_UTIL_STATISTIC_HPP_
#define TESTRUNNER_UTIL_STATISTIC_HPP_


#include "util/error.hpp"

//#include <algorithm>
//#include <cassert>
#include <vector>
#include <array>
#include <tuple>
#include <cmath>

namespace util {

//using util::formatVal;
//using std::log10;
//using std::fabs;
//using std::max;
//using std::min;

using VecD = std::vector<double>;

namespace { // Implementation details
}//(End)Implementation namespace


inline double averageLastN(VecD const& data, size_t n)
{
    n = std::min(n, data.size());
    size_t oldest = data.size() - n;
    double sum = 0.0;
    for (size_t i=data.size(); oldest < i; --i)
        sum += data[i-1];
    return 0<n? sum/n : 0.0;
}


/**
 * Single data point used for linear regression.
 * Simple case: single predictor variable (x).
 * @remark including a weight factor
 */
struct RegressionPoint
{
    double x;
    double y;
    double w;
};

using RegressionData = std::vector<RegressionPoint>;


/** "building blocks" for weighted mean, weighted variance and covariance */
inline auto computeWeightedStatSums(RegressionData const& points)
{
    std::array<double,6> sums = {0.0,0.0,0.0,0.0,0.0,0.0};
    auto& [wsum, wxsum, wysum, wxxsum, wyysum, wxysum] = sums;
    for (auto& p : points)
    {
        wsum += p.w;
        wxsum += p.w * p.x;
        wysum += p.w * p.y;
        wxxsum += p.w * p.x*p.x;
        wyysum += p.w * p.y*p.y;
        wxysum += p.w * p.x*p.y;
    }
    return sums;
}

/**
 * Compute simple linear regression with a single predictor variable (x).
 * @param points data to fit the linear model with
 * @return the computed linear model `b + a·x`, and the resulting fit
 *         - socket (constant offset `b`)
 *         - gradient (linear factor `a`)
 *         - a vector with a predicted `y` value for each `x` value
 *         - a vector with the error, i.e `Δ = y - y_predicted`
 *         - correlation between x and y values
 *         - maximum absolute delta
 *         - delta standard deviation
 */
inline std::tuple<double,double, VecD,VecD, double,double,double> computeLinearRegression(RegressionData const& points)
{
    auto [wsum, wxsum, wysum, wxxsum, wyysum, wxysum] = computeWeightedStatSums(points);

    double xm = wxsum / wsum;                                       // weighted mean x = 1/Σw · Σwx
    double ym = wysum / wsum;
    double varx = wxxsum + xm*xm * wsum - 2*xm * wxsum;             // Σw · x-Variance = Σw(x-xm)²
    double vary = wyysum + ym*ym * wsum - 2*ym * wysum;
    double cova = wxysum + xm*ym * wsum - ym * wxsum - xm * wysum;  // Σw · Covariance = Σw(x-xm)(y-ym)

    // Linear Regression minimising σ²
    double gradient = cova / varx;                                  // gradient = correlation · σy / σx ; σ = √Variance
    double socket   = ym - gradient * xm;                           // Regression line:  Y-ym = gradient · (x-xm)  ; set x≔0 yields socket

    // Correlation (Pearson's r)
    double correlation = gradient * sqrt(varx/vary);

    // calculate error Δ for all measurement points
    VecD predicted;  predicted.reserve(points.size());
    VecD deltas;     deltas.reserve(points.size());
    double maxDelta = 0.0;
    double variance = 0.0;
    for (auto& p : points)
    {
        double y_pred = socket + gradient * p.x;
        double delta  = p.y - y_pred;
        predicted.push_back(y_pred);
        deltas.push_back(delta);
        maxDelta = std::max(maxDelta, fabs(delta));
        variance += p.w * delta*delta;
    }
    return std::make_tuple(socket,gradient
                          ,move(predicted)
                          ,move(deltas)
                          ,correlation
                          ,maxDelta
                          ,sqrt(variance/wsum)
                          );
}


}//(End)namespace util
#endif /*TESTRUNNER_UTIL_STATISTIC_HPP_*/
