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
 ** - average over the N last elements in a data sequence
 ** - simple linear regression with weights (single predictor variable)
 ** - also over a time series with zero-based indices
 **
 */



#ifndef TESTRUNNER_UTIL_STATISTIC_HPP_
#define TESTRUNNER_UTIL_STATISTIC_HPP_


#include "util/error.hpp"
#include "util/nocopy.hpp"
#include "util/format.hpp"
#include "util/utils.hpp"

#include <vector>
#include <array>
#include <tuple>
#include <cmath>

namespace util {

using std::fabs;
using std::array;
using std::tuple;
using std::make_tuple;

using VecD = std::vector<double>;

/** helper to unpack a std::tuple into a homogeneous std::array */
template<typename TUP>
constexpr auto array_from_tuple(TUP&& tuple)
{
    constexpr auto makeArray = [](auto&& ... x){ return std::array{std::forward<decltype(x)>(x) ... }; };
    return std::apply(makeArray, std::forward<TUP>(tuple));
}




/**
 * Read-only view into a segment within a sequence of data
 * @tparam D value type of the data series
 * @remark simplistic workaround since we don't support C++20 yet
 * @todo replace by const std::span
 */
template<typename D>
class DataSpan
    : util::Cloneable
{
    const D* const b_{nullptr};
    const D* const e_{nullptr};

public:
    DataSpan() = default;
    DataSpan(D const& begin, D const& end)
        : b_{&begin}
        , e_{&end}
    {   if (e_ < b_) throw error::Invalid("End point before begin."); }

    template<class CON>
    DataSpan(CON const& container)
        : DataSpan{*std::begin(container), *std::end(container)}
    { }


    using iterator = const D*;

    size_t size()  const { return e_ - b_; }
    bool empty()   const { return b_ == e_; }

    iterator begin() const { return b_; }
    iterator end()   const { return e_; }

    D const& operator[](size_t i) const { return b_ + i; }
    D const& at(size_t i) const
    {
        if (i >= size()) throw error::Invalid("Index "+str(i)+" beyond size="+str(size()));
        return this->operator[](i);
    }
};



/** summation of variances, for error propagation: √Σe² */
template<typename... NUMS>
inline double errorSum(NUMS ...vals)
{
    auto sqr = [](auto val){ return val*val; };
    return sqrt((sqr(vals)+ ... + 0.0));
}



template<typename D>
inline double average(DataSpan<D> const& data)
{
    if (isnil(data)) return 0.0;
    double sum = 0.0;
    for (auto val : data)
        sum += val;
    return sum / data.size();
}

template<typename D>
inline double sdev(DataSpan<D> const& data, D mean)
{
    if (isnil(data)) return 0.0;
    double sdev = 0.0;
    for (auto val : data)
    {
        D offset = val - mean;
        sdev += offset*offset;
    }
    size_t n = data.size();
    sdev /= n<2? 1: n-1;
    return sqrt(sdev);
}

inline double sdev(VecD const& data, double mean)
{   return sdev(DataSpan<double>{data}, mean); }



inline DataSpan<double> lastN(VecD const& data, size_t n)
{
    n = std::min(n, data.size());
    size_t oldest = data.size() - n;
    return DataSpan<double>{data[oldest], *data.end()};
}

inline double averageLastN(VecD const& data, size_t n)
{
    return average(lastN(data,n));
}

inline double sdevLastN(VecD const& data, size_t n, double mean)
{
    return sdev(lastN(data,n), mean);
}


/** "building blocks" for mean, variance and covariance of time series data */
template<typename D>
inline auto computeStatSums(DataSpan<D> const& series)
{
    double ysum = 0.0;
    double yysum = 0.0;
    double xysum = 0.0;
    size_t x = 0;
    for (auto& y : series)
    {
        ysum += y;
        yysum += y*y;
        xysum += x*y;
        ++x;
    }
    return make_tuple(ysum,yysum, xysum);
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
inline auto computeWeightedStatSums(DataSpan<RegressionPoint> const& points)
{
    std::array<double,6> sums;
    sums.fill(0.0);
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
 * @param points 2D data to fit the linear model with, including weights.
 * @return the computed linear model `b + a·x`, and the resulting fit
 *         - socket (constant offset `b`)
 *         - gradient (linear factor `a`)
 *         - a vector with a predicted `y` value for each `x` value
 *         - a vector with the error, i.e `Δ = y - y_predicted`
 *         - correlation between x and y values
 *         - maximum absolute delta
 *         - delta standard deviation
 */
inline auto computeLinearRegression(DataSpan<RegressionPoint> const& points)
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
    double correlation = wyysum==0.0? 0.0 : gradient * sqrt(varx/vary);

    // calculate error Δ for all measurement points
    size_t n = points.size();
    VecD predicted;  predicted.reserve(n);
    VecD deltas;     deltas.reserve(n);
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
    variance /= wsum * (n<=2? 1 : (n-2)/double(n)); // N-2 because it's an estimation,
                                                    // based on 2 other estimated values (socket,gradient)
    return make_tuple(socket,gradient
                     ,move(predicted)
                     ,move(deltas)
                     ,correlation
                     ,maxDelta
                     ,sqrt(variance)
                     );
}

inline auto computeLinearRegression(RegressionData const& points)
{   return computeLinearRegression(DataSpan<RegressionPoint>{points}); }



/**
 * Compute linear regression over a time series with zero-based indices.
 * @remark using the indices as x-values, the calculations for a regression line
 *         can be simplified, using the known closed formula for a sum of integers,
 *         shifting the indices to 0…n-1 (leaving out the 0 and 0² term)
 *         - `1+…+n = n·(n+1)/2`
 *         - `1+…+n² = n·(n+1)·(2n+1)/6`
 * @return `(socket,gradient)` to describe the regression line y = socket + gradient · i
 */
template<typename D>
inline auto computeTimeSeriesLinearRegression(DataSpan<D> const& series)
{
    if (series.size() < 2) return make_tuple(0.0,0.0,0.0);

    auto [ysum,yysum, xysum] = computeStatSums(series);

    size_t n = series.size();
    double im = (n-1)/2.0;                     // mean of zero-based indices i ∈ {0 … n-1}
    double ym = ysum / n;                      // mean y
    double varx = (n-1)*(n+1)/12.0;            // variance of zero-based indices Σ(i-im)² / n
    double vary = yysum/n - ym*ym;             // variance of data values  Σ(y-ym)² / n
    double cova = xysum  - ysum *(n-1)/2;      // Time series Covariance = Σ(i-im)(y-ym) = Σiy + im·ym·n - ym·Σi - im·Σy; use n*ym = Σy

    // Linear Regression minimising σ²
    double gradient = cova / (n*varx);         // Gradient = Correlation · σy / σx ; σ = √Variance;  Correlation = Covariance /(√Σx √Σy)
    double socket   = ym - gradient * im;      // Regression line:  Y-ym = Gradient · (i-im)  ; set i≔0 yields socket

    // Correlation (Pearson's r)
    double correlation = yysum==0.0? 0.0 : gradient * sqrt(varx/vary);
    return make_tuple(socket,gradient,correlation);
}

inline auto computeTimeSeriesLinearRegression(VecD const& series)
{   return computeTimeSeriesLinearRegression(DataSpan<double>{series}); }



}//(End)namespace util
#endif /*TESTRUNNER_UTIL_STATISTIC_HPP_*/
