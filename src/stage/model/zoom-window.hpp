/*
  ZOOM-WINDOW.hpp  -  generic translation from domain to screen coordinates

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file zoom-window.hpp
 ** Abstraction: the current zoom- and navigation state of a view, possibly in multiple
 ** dimensions. This is a generic component to represent and handle the zooming and
 ** positioning of views within an underlying model space. This model space is conceived
 ** to be two fold:
 ** - it is a place or excerpt within the model topology (e.g. the n-th track in the fork)
 ** - it has a temporal extension within a larger temporal frame (e.g. some seconds within
 **   the timeline)
 ** This component is called »Zoom Window«, since it represents a window-like local visible
 ** interval, embedded into a larger time span covering a complete timeline.
 ** @note as of 10/2022 this component is in an early stage of development and just used
 **       to coordinate the horizontal extension of the timeline view.
 ** 
 ** # Rationale
 ** 
 ** Working with and arranging media requires a lot of navigation and changes of zoom detail level.
 ** More specifically, the editor is required to repeatedly return to the same locations and show
 ** arrangements at the same alternating scale levels. Most existing editing applications approach
 ** this topic naively, by just responding to some coarse grained interaction controls — thereby
 ** creating the need for a lot of superfluous and tedious search and navigation activities,
 ** causing constant grind for the user. And resolving these obnoxious shortcomings turns out
 ** as a never ending task, precisely due to the naive and ad-hoc approach initially taken.
 ** Based on these observations, the design of the Lumiera UI calls for centralisation of
 ** all zoom- and navigation handling into a single component, instantiated once for every
 ** visible context, outfitted with the ability to capture and maintain a history of zoom
 ** and navigation activities. The current zoom state is thus defined by
 ** - the overall TimeSpan of the timeline, defining a start and end time
 ** - the visible interval („window“), likewise modelled as time::TimeSpan
 ** - the scale defined as pixels per second
 ** 
 ** # Interactions
 ** 
 ** The basic parameters can be changed and adjusted through various setters, dedicated
 ** to specific usage scenarios. After invoking any setter, one of the mutating functions
 ** is invoked to adjust the base parameters and then re-establish the *invariant*
 ** - visible window lies completely within the overall range
 ** - scale factor and visible window line up logically
 ** - scale factor produces precise reproducible values
 ** 
 ** ## Safety Guards
 ** 
 ** Most setters require lib::time::TimeSpan or lib::time::Duration as _value arguments;_
 ** based on those entities internal definition, they can be assumed to be sanitised,
 ** within TimeValue bounds and properly oriented. Other setters taking possibly negative
 ** numbers are prepared to handle these (e.g. as offset). Range checks are in place to
 ** prevent possibly dangerous numbers from infesting the calculations.
 ** @note rational integral arithmetics can be insidious, since normalisation requires
 **       frequent multiplications, and large denominators might cause numeric overflow.
 ** 
 ** ## Change listener
 ** 
 ** A single change listener lambda can be installed (as of 10/2022 this is considered sufficient,
 ** since only the TimelineLayout was identified as collaborator requiring push notification).
 ** This callback will be invoked after any effective change and serves as notification; the
 ** receiver is expected to read the current settings by invoking the getters.
 ** 
 ** @todo WIP-WIP as of 10/2022 the usage is just emerging, while the actual meaning
 **               of this interface still remains somewhat nebulous...
 ** @see \ref ZoomWindow_test
 ** 
 */


#ifndef STAGE_MODEL_ZOOM_WINDOW_H
#define STAGE_MODEL_ZOOM_WINDOW_H


#include "lib/error.hpp"
#include "lib/rational.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <limits>
#include <functional>
#include <array>


namespace stage {
namespace model {
  
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using lib::time::TimeVar;
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::time::Time;
  
  using util::Rat;
  using util::rational_cast;
  using util::can_represent_Product;
  using util::reQuant;
  
  using util::min;
  using util::max;
  using util::sgn;
  
  namespace { ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #1259 : reorganise raw time base datatypes : need conversion path into FSecs
    /**
     * @todo preliminary helper to enter into fractional integer calculations
     *       - FSecs (maybe better called `RSec`) should be a light-weight wrapper
     *         on top of util::Rat = `boost::rational<int64_t>`
     *       - a conversion function like in TimeVar should be in the base type
     *       - however, cross conversion from raw int64_t should be prohibited
     *         to avoid ill-guided automatic conversions from µ-tick to seconds
     */
    inline FSecs
    _FSecs (TimeValue const& timeVal)
    {
      return FSecs{_raw(timeVal), TimeValue::SCALE};
    }
    
    /**
     * @return `true` if the given duration can be represented cleanly as µ-ticks.
     * @remark decision can be broken down to the remainder term: `n/d = i + r/d`;
     *         when expanding with `*u/u`, result is clean if `u/d` is non-fractional.
     * @todo should likewise be member of a FSecs wrapper type...
     */
    inline bool
    isMicroGridAligned (FSecs duration)
    {
      return 0 == Time::SCALE % duration.denominator();
    }
    
    inline double
    approx (Rat r)
      {
        return util::rational_cast<double> (r);
      }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1261 : why the hell did I define time entities to be immutable. Looks like a "functional programming" fad in hindsight
    /** @todo we need these only because the blurry distinction between
     *        lib::time::TimeValue and lib::time::Time, which in turn is caused
     *        by a lack of clarity in the fundamental conception (see #1261)
     */
    inline TimeVar
    operator+ (Time const& tval, TimeVar const& tvar)
    {
      return TimeVar(tval) += tvar;
    }
    inline TimeVar
    operator- (Time const& tval, TimeVar const& tvar)
    {
      return TimeVar(tval) -= tvar;
    }
  }
  
  
  /** the deepest zoom is to use 2px per micro-tick */
  const uint ZOOM_MAX_RESOLUTION = 2 * TimeValue::SCALE;
  
  namespace {// initial values (rather arbitrary)
    const FSecs DEFAULT_CANVAS{23};
    const Rat   DEFAULT_METRIC{25};
    const uint  MAX_PX_WIDTH{100000};
    const FSecs MAX_TIMESPAN{_FSecs(Duration::MAX)};
    const FSecs MICRO_TICK{1_r/Time::SCALE};
    
    /** Maximum quantiser to be handled in fractional arithmetics without hazard.
     * @remark due to the common divisor normalisation, and the typical time computations,
     *         DENOMINATOR * Time::Scale has to stay below INT_MAX, with some safety margin
     */
    const int64_t LIM_HAZARD   {int64_t{1} << 40 };
    const int64_t HAZARD_DEGREE{util::ilog2(LIM_HAZARD)};
    const int64_t MAXDIM       {util::ilog2 (std::numeric_limits<int64_t>::max())};
    
    inline int
    toxicDegree (Rat poison, const int64_t THRESHOLD =HAZARD_DEGREE)
    {
      int64_t magNum = util::ilog2(abs(poison.numerator()));
      int64_t magDen = util::ilog2(abs(poison.denominator()));
      int64_t degree = max (magNum, magDen);
      return max (0, degree - THRESHOLD);
    }
  }
  
  
  /**
   * A component to ensure uniform handling of zoom scale
   * and visible interval on the timeline. Changes through
   * the mutator functions are validated and harmonised to
   * meet the internal invariants; a change listener is
   * possibly notified to pick up the new settings.
   * 
   * A ZoomWindow...
   * - is a #visible TimeSpan
   * - which is completely inside an #overalSpan
   * - and is rendered at a scale factor #px_per_sec
   * - 0 < px_per_sec <= ZOOM_MAX_RESOLUTION
   * - zoom operations are applied around an #anchorPoint
   */
  class ZoomWindow
    : util::NonCopyable
    {
      TimeVar startAll_, afterAll_,
              startWin_, afterWin_;
      Rat px_per_sec_;
      
      std::function<void()> changeSignal_{};
      
    public:
      ZoomWindow (uint pxWidth, TimeSpan timeline =TimeSpan{Time::ZERO, DEFAULT_CANVAS})
        : startAll_{ensureNonEmpty(timeline).start()}
        , afterAll_{ensureNonEmpty(timeline).end()}
        , startWin_{startAll_}
        , afterWin_{afterAll_}
        , px_per_sec_{establishMetric (pxWidth, startWin_, afterWin_)}
        {
          conformWindowToMetricLimits (this->pxWidth());
          ensureInvariants();
        }
      
      ZoomWindow (TimeSpan timeline =TimeSpan{Time::ZERO, DEFAULT_CANVAS})
        : ZoomWindow{0, timeline}  //see ensureConsistent()
        { }
      
      TimeSpan
      overallSpan()  const
        {
          return TimeSpan{startAll_, afterAll_};
        }
      
      TimeSpan
      visible()  const
        {
          return TimeSpan{startWin_, afterWin_};
        }
      
      Rat
      px_per_sec()  const
        {
          return px_per_sec_;
        }
      
      uint
      pxWidth()  const
        {
          REQUIRE (startWin_  < afterWin_);
          return calcPixelsForDurationAtScale (px_per_sec(), afterWin_-startWin_);
        }
      
      
      
      /* === Mutators === */
      
      /**
       * define the extension of the window in pixels.
       * @note all other manipulations will always retain this value
       */
      void
      calibrateExtension (uint pxWidth)
        {
          adaptWindowToPixels (pxWidth);
          fireChangeNotification();
        }
      
      /**
       * explicitly set the zoom factor, defined as pixel per second
       * @note the given factor will be capped to remain below a maximal
       *       zoom of 2px per µ-tick; also the window may not be expanded
       *       beyond the current overall canvas size
       */
      void
      setMetric (Rat px_per_sec)
        {
          mutateScale (px_per_sec);
          fireChangeNotification();
        }
      
      /**
       * scale up or down on a 2-logarithmic scale.
       * Each step either doubles or halves the zoom level,
       * and the visible window is adjusted accordingly, using
       * the current #anchorPoint as centre for scaling.
       * @note the zoom factor is limited to be between
       *       2px per µ-tick and showing the full canvas
       */
      void
      nudgeMetric (int steps)
        {
          setMetric(
              steps > 0 ? Rat{px_per_sec_.numerator() << steps
                             ,px_per_sec_.denominator()}
                        : Rat{px_per_sec_.numerator()
                             ,px_per_sec_.denominator() << -steps});
        }
      
      /**
       * Set both the overall canvas, as well as the visible part
       * within that canvas. Given values will possibly be adjusted
       * to retain overall consistency, according to the following rules:
       * - all ranges are non empty and properly oriented
       * - the extension in pixels will always be retained
       * - zoom factor is only allowed to range between showing
       *   the full canvas and a maximum factor (2 pixel per µ-tick)
       * - the visible window will always be within the canvas area
       */
      void
      setRanges (TimeSpan overall, TimeSpan visible)
        {
          mutateRanges (overall, visible);
          fireChangeNotification();
        }
      
      /**
       * redefine the overall canvas range.
       * @note the currently visible window may be shifted or
       *       capped to fit within the new range, which may also
       *       change the zoom factor, while the overall pixel width
       *       is always retained unaltered
       */
      void
      setOverallRange (TimeSpan range)
        {
          mutateCanvas (range);
          fireChangeNotification();
        }
      
      void
      setOverallStart (TimeValue start)
        {
          mutateCanvas (TimeSpan{start, Duration(afterAll_-startAll_)});
          fireChangeNotification();
        }
      
      void
      setOverallDuration (Duration duration)
        {
          mutateCanvas (TimeSpan{startAll_, duration});
          fireChangeNotification();
        }
      
      /**
       * explicitly set the visible window,
       * possibly expanding the canvas to fit.
       * Typically used to zoom into a user selected range.
       */
      void
      setVisibleRange (TimeSpan newWindow)
        {
          mutateWindow (newWindow);
          fireChangeNotification();
        }

      /**
       * the »reverse zoom operation«: zoom out such as to
       * bring the current window at the designated time span.
       * Typically the user selects a sub-range, and the current
       * view is then collapsed accordingly to fit into that range.
       * As a side effect, the canvas may be expanded significantly.
       */
      void
      expandVisibleRange (TimeSpan target)
        {
          // Formulation: Assuming the current window was generated from TimeSpan
          //              by applying an affine-linear transformation f = a·x + b
          FSecs tarDur = _FSecs(target.end()-target.start());
          Rat a = FSecs{afterWin_-startWin_};
          Rat b = FSecs{startWin_}*_FSecs(target.end()) - FSecs{afterWin_}*_FSecs((target.start()));
          a /= tarDur;
          b /= tarDur;
          Time startNew {a * FSecs{startWin_} + b};
          Time afterNew {a * FSecs{afterWin_} + b};
          
          mutateWindow(TimeSpan{startNew, afterNew});
          fireChangeNotification();
        }
      
      /**
       * explicitly set the duration of the visible window range,
       * working around the relative anchor point; possibly expand canvas.
       * @remark the anchor point is based on the relative position of the
       *         window within canvas — however, other than for [scaling](\ref setMetric()),
       *         the canvas will possibly be expanded and the given duration will thus
       *         always be realised. */
      void
      setVisibleDuration (Duration duration)
        {
          mutateDuration (_FSecs(duration));
          fireChangeNotification();
        }
      
      /** scroll by arbitrary offset, possibly expanding canvas. */
      void
      offsetVisiblePos (Offset offset)
        {
          mutateWindow (TimeSpan{startWin_+offset, Duration{afterWin_-startWin_}});
          fireChangeNotification();
        }
      
      /** scroll by increments of half window size, possibly expanding. */
      void
      nudgeVisiblePos (int steps)
        {
          FSecs dur{afterWin_-startWin_};      //    navigate half window steps
          setVisibleRange (TimeSpan{Time{startWin_ + (dur*steps)/2}
                                   , dur});
        }
      
      /**
       * scroll the window to bring the denoted position in sight,
       * retaining the current zoom factor, possibly expanding canvas.
       */
      void
      setVisiblePos (Time posToShow)
        {
          FSecs canvasOffset{posToShow - startAll_};
          anchorWindowAtPosition (canvasOffset);
          fireChangeNotification();
        }

      /** scroll to reveal position designated relative to overall canvas */
      void
      setVisiblePos (Rat percentage)
        {
          FSecs canvasDuration{afterAll_-startAll_};
          anchorWindowAtPosition (scaleSafe (canvasDuration, percentage));
          fireChangeNotification();
        }
      
      void
      setVisiblePos (double percentage)
        { // use some arbitrary yet significantly large work scale
          int64_t scale = max (_raw(afterAll_-startAll_), MAX_PX_WIDTH);
          Rat factor{int64_t(scale*percentage), scale};
          setVisiblePos (factor);
        }
      
      void
      navHistory()
        {
          UNIMPLEMENTED ("navigate Zoom History");
        }
      
      
      /** Attach a λ or functor to be triggered on each actual change. */
      template<class FUN>
      void
      attachChangeNotification (FUN&& trigger)
        {
          changeSignal_ = std::forward<FUN> (trigger);
        }
      
      void
      detachChangeNotification()
        {
          changeSignal_ = std::function<void()>();
        }
      
      
    private:
      void
      fireChangeNotification()
        {
          if (changeSignal_) changeSignal_();
        }
      
      
      /* === utility functions to handle dangerous fractional values === */
      
      /**
       * Check and possibly sanitise a rational number to avoid internal numeric overflow.
       * Fractional arithmetics can be insidious, due to the frequent re-normalisation;
       * seemingly "harmless" numbers with a large denominator can cause numeric wrap-around.
       * As safeguard, by introducing a tiny error, problematic numbers can be re-quantised
       * to smaller denominators; moreover, large numbers must be limit checked.
       * @remark Both the denominator and the numerator must be kept below a toxic limit,
       *         which is defined by the ability to multiply with Time::Scale without wrap-around.
       *         This heuristic is based on the actual computations done with the zoom factor and
       *         is thus specific to the ZoomWindow implementation. To sanitise, the denominator
       *         is reduced logarithmically (bit-shift) sufficiently and then used as new quantiser,
       *         thus ensuring that both denominator (=quantiser) and numerator are below limit.
       * @warning the rational number must not be to large overall; this heuristic will fail
       *         on fractions with very large numerator and small denominator — however, for
       *         the ZoomWindow, this case is not relevant, since the zoom factor is limited,
       *         and other usages of rational numbers can be range checked explicitly.
       * @note the check is based on the 2-logarithm of numerator and denominator, which is
       *         pretty much the fastest possibility (even a simple comparison would have
       *         to do the same). Values below threshold are simply passed-through.
       * @todo this utility function could be factored out into a `FSecs` or `RSec` class  //////////////////TICKET #1262
       */
      static Rat
      detox (Rat poison)
        {
          int toxicity = toxicDegree (poison);
          return toxicity ? reQuant (poison, max (poison.denominator() >> toxicity, 64))
                          : poison;
        }
      
      /**
       * Scale a possibly large time duration by a rational factor, while attempting to avoid
       * integer wrap-around. In the typical use-case, the multiplication can be just computed
       * precisely and safe, but at least we check the limits. In the danger and boundary zone,
       * a slight error is introduced to allow cancelling out a common factor, so that the result
       * can be just constructed without any further dangerous computation. Obviously this is only
       * a heuristic, yet adequate within the framework of ZoomWindow, where the end result is
       * pixel aligned anyway.
       * @todo this utility function could be factored out into a `FSecs` or `RSec` class  //////////////////TICKET #1262
       */
      static FSecs
      scaleSafe (FSecs duration, Rat factor)
        {
          if (util::can_represent_Product(duration, factor))
            // just calculate ordinary numbers...
            return duration * factor;
          else
            {
              auto guess{approx(duration) * approx (factor)};
              if (approx(MAX_TIMESPAN) < abs(guess))
                return MAX_TIMESPAN * sgn(guess); // exceeds limits of time representation => cap the result
              if (0 == guess)
                return 0;
                        /**
                         * Descriptor for a Strategy to reduce the numbers to keep them in domain.
                         * After cross-wise cancelling out one part in each factor, the result can be
                         * constructed without any further multiplication. To achieve that, a slight
                         * error is introduced into one of the four participating numbers
                         */
                        struct ReductionStrategy
                          {
                            int64_t f1;  ///< factor one is safe and will not be changed
                            int64_t u;   ///< the counterpart of f1 is used as quantiser and cancelled out
                            int64_t q;   ///< the diagonal counterpart of u is scaled to u and cancelled
                            int64_t f2;  ///< the counterpart of #q is re-quantised to u; it acts as limit
                            bool invert; ///< Strategy will be applied to the inverse 1/x
                            
                            int64_t
                            determineLimit()
                              {
                                REQUIRE (u != 0);
                                return isFeasible()? u : 0;
                              }
                            
                            Rat
                            calculateResult()
                              {
                                REQUIRE (isFeasible());
                                f2 = reQuant (f2, q, u);
                                return invert? Rat{f2, f1}
                                             : Rat{f1, f2};
                              }
                            
                            bool
                            isFeasible()
                              {                           // Note: factors are nonzero,
                                REQUIRE (u and q and f2);//        otherwise exit after pre-check above
                                int dim_u = util::ilog2 (abs (u));
                                int dim_q = util::ilog2 (abs (q));
                                if (dim_q > dim_u) return true; // requantisation will reduce size and thus no danger
                                int dim_f = util::ilog2 (abs (f2));
                                int deltaQ = dim_u - dim_q;     // how much q must be increased to match u
                                int headroom = MAXDIM - dim_f;  // how much the counter factor f2 can be increased
                                return headroom > deltaQ;
                              }
                          };
              using Cases = std::array<ReductionStrategy, 4>;
              // There are four possible strategy configurations.
              // One case stands out, insofar this factor is guaranteed to be present:
              // because one of the numbers is a quantised Time, it has Time::SCALE as denominator,
              // maybe after cancelling out some further common integral factors
              auto [reduction,rem] = util::iDiv (Time::SCALE, duration.denominator());
              if (rem != 0) reduction = 1;  // when duration is not µ-Tick quantised
              int64_t durationQuant = duration.denominator()*reduction;
              int64_t durationTicks = duration.numerator()*reduction;
              
                         //-f1--------------------+-u-------------------+-q---------------------+-f2--------------------+-invert--
              Cases cases{{{durationTicks         , durationQuant       , factor.numerator()    , factor.denominator()  , false}
                          ,{factor.numerator()    , factor.denominator(), duration.numerator()  , duration.denominator(), false}
                          ,{duration.denominator(), duration.numerator(), factor.denominator()  , factor.numerator()    , true}
                          ,{factor.denominator()  , factor.numerator()  , duration.denominator(), duration.numerator()  , true}
                         }};
              // However, some of the other cases may yield a larger denominator to be cancelled out,
              // and thus lead to a smaller error margin. Attempt thus to find the best strategy...
              ReductionStrategy* solution{nullptr};
              int64_t maxLimit = 0;
              for (auto& candidate: cases)
                {
                  int64_t limit = candidate.determineLimit();
                  if (limit > maxLimit)
                    {
                      maxLimit = limit;
                      solution = &candidate;
                    }
                }
              
              ASSERT (solution and maxLimit > 0);
              return detox (solution->calculateResult());
            }
        }
      
      /**
       * Calculate sum (or difference) of possibly large time durations, avoiding integer wrap-around.
       * Again, this is a heuristics, based on re-quantisation to a smaller common denominator.
       * @return exact result if representable, otherwise approximation
       * @note result is capped to MAX_TIMESPAN when exceeding domain
       * @todo this utility function could be factored out into a `FSecs` or `RSec` class  //////////////////TICKET #1262
       */
      static FSecs
      addSafe (FSecs t1, FSecs t2)
        {
          if (util::can_represent_Sum (t1,t2))
            // directly calculate ordinary numbers...
            return t1 + t2;
          else
            {
              auto guess{approx(t1) + approx(t2)};
              if (approx(MAX_TIMESPAN) < abs(guess))
                return MAX_TIMESPAN * sgn(guess); // exceeds limits => cap the result
              
              // re-Quantise numbers to achieve a common denominator,
              // thus avoiding to multiply numerators for normalisation
              int64_t n1 = t1.numerator();
              int64_t d1 = t1.denominator();
              int s1 = sgn(n1)*sgn(d1);
              n1 = abs(n1); d1 = abs(d1);
              int64_t n2 = t2.numerator();
              int64_t d2 = t2.denominator();
              int s2 = sgn(n2)*sgn(d2);
              n2 = abs(n2); d2 = abs(d2);
              // quantise to smaller denominator to avoid increasing any numerator
              int64_t u = d1<d2? d1:d2;
              if (u < Time::SCALE)
                // regarding precision, quantising to µ-grid is the better solution
                u = Time::SCALE;
              else //re-quantise to common denominator more fine-grained than µ-grid
                if (s1*s2 > 0  // check numerators to detect danger of wrap-around
                    and (MAXDIM<=util::ilog2(n1) or MAXDIM<=util::ilog2(n2)))
                  u >>= 1;   // danger zone! wrap-around imminent
              
              n1 = d1==u? n1 : reQuant (n1,d1, u);
              n2 = d2==u? n2 : reQuant (n2,d2, u);
              FSecs res{s1*n1 + s2*n2, u};
              
              auto f128 = [](Rat n){ return rational_cast<long double>(n); }; // can't use the guess from above,
              ENSURE (abs (f128(res) - (f128(t1)+f128(t2))) < 1.0/u);        //  double precision is not sufficient
              return detox (res);
            }
        }
      
      
      
      
      /* === establish and maintain invariants === */
      /*
       * - oriented and non-empty windows
       * - never alter given pxWidth
       * - zoom metric factor < max zoom
       * - visibleWindow ⊂ Canvas
       */
      
      static TimeSpan
      ensureNonEmpty (TimeSpan const& span)
        {
          return TimeSpan{span.start()
                         ,util::isnil(span.duration())? Duration{DEFAULT_CANVAS}
                                                      : span.duration()
                         }.conform();
        }
      
      /** Assertion helper: resulting pxWidth matches expectations */
      static void
      ENSURE_matchesExpectedPixWidth (Rat zoomFactor, FSecs duration, uint pxWidth)
        {
          auto sizeAtRequestedScale = approx(zoomFactor) * approx(duration);
          ENSURE (abs(pxWidth - sizeAtRequestedScale) <= 1
                 ,"ZoomWindow: established size or metric misses expectation "
                  "by more than 1px. %upx != %1.6f expected pixel."
                 , pxWidth, sizeAtRequestedScale);
        }
      
      /** calculate `rational_cast<uint> (zoomFactor * duration)`
       * @remark indirect calculation to avoid overflow on large durations
       */
      static int64_t
      calcPixelsForDurationAtScale (Rat zoomFactor, FSecs duration)
        {// break down the integer division into several steps...
          auto zn = zoomFactor.numerator();
          auto zd = zoomFactor.denominator();
          auto dn = duration.numerator();
          auto dd = duration.denominator();
          auto [secs,r] = util::iDiv (dn, dd);        // split duration in full seconds and rest
          auto [px1,r1] = util::iDiv (secs*zn, zd);   // calc pixels required for full seconds
          auto [px2,r2] = util::iDiv (r*zn, dd*zd);   // calc pixels required for rest duration
          auto pxr      = (r1*dd +r2) /(dd*zd);       // and calculate integer div for combined remainders
          ENSURE (0 <= px1 and 0 <= px2 and 0<= pxr);
          return px1 + px2 + pxr;
        }
      
      /** window size beyond that limit would lead to
       *  numerically dangerous zoom factors (pixel/duration) */
      static FSecs
      maxSaneWinExtension (uint pxWidth)
        {
          return min (FSecs{LIM_HAZARD * pxWidth, 1000}, MAX_TIMESPAN);
        }     // Note: denominator 1000 is additional safety margin
             //        wouldn't be necessary, but makes detox(largeTime) more precise
      
      /**
       * Reform the effective metric in all dangerous corner cases.
       * Ensure the metric value is not »poisonous« and can be multiplied
       * even with Time::SCALE without numeric wrap-around.
       * @note this function introduces a slight error to simplify the numbers;
       *       then the result is optimised to conform to pxWith and duration
       */
      Rat
      optimiseMetric (uint pxWidth, FSecs dur, Rat rawMetric)
        {
          using util::ilog2;
          REQUIRE (0 < pxWidth and 0 < dur and 0 < rawMetric);
          REQUIRE (isMicroGridAligned (dur));
          // circumvent numeric problems due to excessive large factors
          int64_t magDen = ilog2(rawMetric.denominator());
          int reduction = toxicDegree (rawMetric);
          int quant     = max (magDen-reduction, 16);
          //  re-quantise metric into power of two <= 2^40 (headroom 22 bit)
          //  Known to work always, since 9e-10 < metric < 2e+6
          Rat adjMetric = util::reQuant (rawMetric, int64_t(1) << quant);
          
            // Correct that metric to reproduce expected pxWidth...
           //  Retain reduced denominator, but optimise the numerator
          //   pixel = trunc{ metric*duration }
          double epsilon = std::numeric_limits<double>::epsilon()
               , dn = dur.numerator()
               , dd = dur.denominator()
               , md = adjMetric.denominator()
               , mn = (pxWidth+epsilon)*md*dd/dn;
          // construct optimised zoom metric result
          int64_t num = mn, den = adjMetric.denominator();
          if (epsilon < mn - num)
            {// optimisation found inter-grid result -- increase precision
              int headroom = max (1, HAZARD_DEGREE - max (ilog2(num), ilog2(den)));
              int64_t scale = int64_t(1) << headroom;
              num = scale*mn;                 // quantise again with increased resolution
              den = scale*den;               //  at least factor 2 to get some improvement
              if (pxWidth > dn/dd*num/den)  //   If still some remaining error....
                ++num; // round up to be sure to hit the next higher pixel count
            }
          adjMetric = Rat{num, den};
          ENSURE (pxWidth == calcPixelsForDurationAtScale (adjMetric, dur));
          double impliedDur = double(pxWidth)*den/num;
          double relError   = abs(dn/dd /impliedDur -1);
          double quantErr   = 1.0/(num-1);
          ENSURE (quantErr  > relError, "metric misses duration by "
                  "%3.2f%% > %3.2f%% (=relative quantisation error)"
                 ,100*relError, 100.0*quantErr);
          return adjMetric;
        }
      
      
      static Rat
      establishMetric (uint pxWidth, Time startWin, Time afterWin)
        {
          REQUIRE (startWin < afterWin);
          FSecs dur = _FSecs(afterWin-startWin);
          if (pxWidth == 0 or pxWidth > MAX_PX_WIDTH) // default to sane pixel width
              pxWidth = max<uint> (1, rational_cast<uint> (DEFAULT_METRIC * dur));
          Rat metric = Rat(pxWidth) / dur;
          // rational arithmetic ensures we can always reproduce the pxWidth
          ENSURE (pxWidth == calcPixelsForDurationAtScale (metric, dur));
          ENSURE (0 < metric);
          return metric;
        }
      
      /** this is the centrepiece of the whole zoom metric logic...
       * @note control flow for every scale adjustment passes here */
      void
      conformWindowToMetric (Rat changedMetric)
        {
          REQUIRE (changedMetric > 0);
          REQUIRE (afterWin_> startWin_);
          FSecs dur{afterWin_-startWin_};
          uint pxWidth = calcPixelsForDurationAtScale (px_per_sec_, dur);
          dur = Rat(pxWidth) / detox (changedMetric);
          dur = min (dur, MAX_TIMESPAN);// limit maximum window size
          dur = max (dur, MICRO_TICK); //  prevent window going void
          TimeVar timeDur{Duration{dur}};
          // prefer bias towards increased window instead of increased metric
          if (not isMicroGridAligned (dur))
            timeDur = timeDur + TimeValue(1);
          // resize window relative to anchor point
          placeWindowRelativeToAnchor (dur);
          establishWindowDuration (Duration{timeDur});
          // re-check metric to maintain precise pxWidth
          px_per_sec_ = conformMetricToWindow (pxWidth);
          ENSURE (_FSecs(afterWin_-startWin_) <= MAX_TIMESPAN);
          ENSURE_matchesExpectedPixWidth (changedMetric, afterWin_-startWin_, pxWidth);
        }
      
      Rat
      conformMetricToWindow (uint pxWidth)
        {
          REQUIRE (pxWidth > 0);
          REQUIRE (afterWin_> startWin_);
          FSecs dur{afterWin_-startWin_};
          Rat adjMetric = Rat(pxWidth) / dur;
          if (not toxicDegree(adjMetric)
              and pxWidth == calcPixelsForDurationAtScale (adjMetric, dur))
            return adjMetric;
          else
            return optimiseMetric(pxWidth, dur, adjMetric);
        }
      
      /**
       * The zoom metric factor must not become "poisonous".
       * This leads to a minimum possible zoom factor for a given pixWidth,
       * thereby effectively limiting the maximum window extension. This
       * constraint is enforced by reducing the current window size.
       */
      void
      conformWindowToMetricLimits (uint pxWidth)
        {
          REQUIRE (pxWidth > 0);
          FSecs dur{afterWin_-startWin_};
          if (dur > maxSaneWinExtension (pxWidth))
            {
              dur = maxSaneWinExtension (pxWidth);
              placeWindowRelativeToAnchor (dur);
              establishWindowDuration (dur);
            }
        }
      
      void
      conformWindowToCanvas()
        {
          FSecs dur{afterWin_-startWin_};
          REQUIRE (dur <= MAX_TIMESPAN);
          startAll_ = max (startAll_, Time::MIN);
          afterAll_ = min (afterAll_, Time::MAX);
          if (dur <= _FSecs(afterAll_-startAll_))
            {//possibly shift into current canvas
              if (afterWin_ > afterAll_)
                {
                  Offset shift{afterWin_ - afterAll_};
                  startWin_ -= shift;
                  afterWin_ -= shift;
                }
              else
              if (startWin_ < startAll_)
                {
                  Offset shift{startAll_ - startWin_};
                  startWin_ += shift;
                  afterWin_ += shift;
                }
            }
          else
            {//need to cap window to fit into canvas
              startWin_ = startAll_;
              afterWin_ = afterAll_;
            }
          ENSURE (startAll_ <= startWin_);
          ENSURE (afterWin_ <= afterAll_);
          ENSURE (Time::MIN <= startWin_);
          ENSURE (afterWin_ <= Time::MAX);
        }
      
      void
      conformToBounds (Rat changedMetric)
        {
          if (changedMetric > ZOOM_MAX_RESOLUTION)
            {
              changedMetric = ZOOM_MAX_RESOLUTION;
              conformWindowToMetric (changedMetric);
            }
          startAll_ = min (startAll_, startWin_);
          afterAll_ = max (afterAll_, afterWin_);
          ENSURE (Time::MIN <= startWin_);
          ENSURE (afterWin_ <= Time::MAX);
          ENSURE (startAll_ <= startWin_);
          ENSURE (afterWin_ <= afterAll_);
          ENSURE (px_per_sec_ <= ZOOM_MAX_RESOLUTION);
          ENSURE (px_per_sec_ <= changedMetric); // bias
        }
      
      /**
       * Procedure to (re)establish the invariants.
       * Adjustments should be done first to windows,
       * then to the metric, using #conformWindowToMetric().
       * Then this function shall be called and will first
       * shift and possibly cap the window, then reestablish
       * the metric and possibly increase the canvas to keep
       * ensure the ZOOM_MAX_RESOLUTION is not exceeded.
       * These steps ensure overall pixel size remains stable.
       */
      void
      ensureInvariants(uint px =0)
        {
          if (px==0) px = pxWidth();
          conformWindowToCanvas();
          px_per_sec_ = conformMetricToWindow (px);
          conformToBounds (px_per_sec_);
        }
      
      
      
      /* === adjust and coordinate window parameters === */
      
      /** @internal set a different overall canvas range,
       *            possibly set window and metrics to fit */
      void
      mutateCanvas (TimeSpan canvas)
        {
          startAll_ = ensureNonEmpty(canvas).start();
          afterAll_ = ensureNonEmpty(canvas).end();
          ensureInvariants();
        }
      
      /** @internal change Window TimeSpan, possibly also outside
       *            of the current canvas, which is then expanded;
       *            validate and adjust all params accordingly */
      void
      mutateWindow (TimeSpan window)
        {
          uint px{pxWidth()};
          startWin_ = ensureNonEmpty(window).start();
          afterWin_ = ensureNonEmpty(window).end();
          conformWindowToMetricLimits (px);
          startAll_ = min (startAll_, startWin_);
          afterAll_ = max (afterAll_, afterWin_);
          ensureInvariants (px);
        }
      
      /** @internal change canvas and window position in one call,
       *            then validate and adjust to maintain invariants */
      void
      mutateRanges (TimeSpan canvas, TimeSpan window)
        {
          uint px{pxWidth()};
          startAll_ = ensureNonEmpty(canvas).start();
          afterAll_ = ensureNonEmpty(canvas).end();
          startWin_ = ensureNonEmpty(window).start();
          afterWin_ = ensureNonEmpty(window).end();
          conformWindowToMetricLimits (px);
          ensureInvariants (px);
        }
      
      /**
       * @internal adjust Window to match given scale,
       *           validate and adjust all params */
      void
      mutateScale (Rat changedMetric)
        {
          uint px{pxWidth()};
          changedMetric = max (changedMetric, px / maxSaneWinExtension(px));
          changedMetric = min (detox(changedMetric), ZOOM_MAX_RESOLUTION);
          if (changedMetric == px_per_sec_) return;
          conformWindowToMetric (changedMetric);
          ensureInvariants (px);
        }
      
      /** @internal change visible duration centred around anchor point,
       *            validate and adjust all params */
      void
      mutateDuration (FSecs duration, uint px =0)
        {
          if (px==0)
            px = pxWidth();
          if (duration <= 0)
            duration = DEFAULT_CANVAS;
          else if (duration > maxSaneWinExtension (px))
            duration = maxSaneWinExtension (px);
          Rat changedMetric = Rat(px) / duration;
          conformWindowToMetric (changedMetric);
          ensureInvariants (px);
        }
      
      /** @internal resize window to span the given pixel with,
       *            validate and adjust all other params */
      void
      adaptWindowToPixels (uint pxWidth)
        {
          pxWidth = util::limited (1u, pxWidth, MAX_PX_WIDTH);
          FSecs adaptedWindow{Rat{pxWidth} / px_per_sec_};
          adaptedWindow = max (adaptedWindow, MICRO_TICK); // prevent void window
          adaptedWindow = min (adaptedWindow, maxSaneWinExtension (pxWidth));
          establishWindowDuration (adaptedWindow);
          ensureInvariants (pxWidth);
        }
      
      /** @internal relocate window anchored at a position relative to canvas,
       *            also placing the anchor position relative within the window
       *            in accordance with the position relative to canvas.
       *            Window will enclose the given position, possibly extending
       *            canvas to fit, afterwards reestablishing all invariants. */
      void
      anchorWindowAtPosition (FSecs canvasOffset)
        {
          REQUIRE (afterWin_ > startWin_);
          REQUIRE (afterAll_ > startAll_);
          uint px{pxWidth()};
          FSecs duration{afterWin_-startWin_};
          Rat posFactor = canvasOffset / FSecs{afterAll_-startAll_};
          posFactor = parabolicAnchorRule (posFactor); // also limited 0...1
          FSecs partBeforeAnchor = scaleSafe (duration, posFactor);
          startWin_ = startAll_ + Offset{addSafe (canvasOffset, -partBeforeAnchor)};
          establishWindowDuration (duration);
          startAll_ = min (startAll_, startWin_);
          afterAll_ = max (afterAll_, afterWin_);
          ensureInvariants (px);
        }
      
      
      /** @internal similar operation as #anchorWindowAtPosition(),
       *            but based on the current window position and without
       *            relocation, rather intended for changing the scale */
      void
      placeWindowRelativeToAnchor (FSecs duration)
        {
          FSecs partBeforeAnchor = scaleSafe(duration, relativeAnchor());
          startWin_ = Time{anchorPoint()} - Time{partBeforeAnchor};
        }
      
      void
      establishWindowDuration (Duration duration)
        {
          if (startWin_<= Time::MAX - duration)
              afterWin_ = startWin_ + duration;
          else
            {
              startWin_ = Time::MAX - duration;
              afterWin_ = Time::MAX;
            }
        }
      
      
      
      /**
       * The anchor point or centre for zooming operations applied to the visible window
       * @return where the visible window should currently be anchored
       * @remark this point can sometimes be outside the current visible window,
       *         but any further zooming/scaling/scrolling operation should bring it back
       *         into sight. Moreover, the function #relativeAnchor() defines the position
       *         where this anchor point _should_ be placed relative to the visible window.
       * @todo 10/2022 we use a numerical rule currently, but that could be contextual state,
       *       like e.g. the current position of the play head or edit cursor or mouse.
       */
      FSecs
      anchorPoint()  const
        {
          return startWin_ + scaleSafe (afterWin_-startWin_, relativeAnchor());
        }
      
      /**
       * define at which proportion to the visible window's duration the anchor should be placed
       * @return a fraction 0 ... 1, where 0 means at start and 1 means after end.
       * @note as of 10/2022 we use a numerical rule to place the anchor point in accordance
       *       to the current visible window's position within the overall timeline; if it's
       *       close to the beginning, the anchor point is also rather to the beginning...
       */
      Rat
      relativeAnchor()  const
        {
          // the visible window itself has to fit in, which reduces the action range
          FSecs possibleRange = (afterAll_-startAll_) - (afterWin_-startWin_);
          if (possibleRange <= 0) // if there is no room for scrolling...
            return 1_r/2;        //  then anchor zooming in the middle
          
          // use a 3rd degree parabola to favour positions in the middle
          Rat posFactor = FSecs{startWin_-startAll_} / possibleRange;
          return parabolicAnchorRule (posFactor);
        }
      
      /**
       * A counter movement rule to place an anchor point, based on a percentage factor.
       * Used to define the anchor point within the window, depending on the window's position
       * relative to the overall canvas. Implemented using a cubic parabola, which moves quick
       * away from the boundaries, while hovering most of the time in the middle area.
       * @return factor effectively between 0 ... 1 (inclusive)
       * @warning in case of a "poisonous" input the calculation may go astray;
       *          yet results are limited at least...
       */
      static Rat
      parabolicAnchorRule (Rat posFactor)
        {
          posFactor = util::limited (0, posFactor, 1);
          if (toxicDegree(posFactor, 20))      //  prevent integer wrap
            posFactor = util::reQuant(posFactor, 1 << 20);
          posFactor = (2*posFactor - 1);             // -1 ... +1
          posFactor = posFactor*posFactor*posFactor; // -1 ... +1 but accelerating towards boundaries
          posFactor = (posFactor + 1) / 2;           //  0 ... 1
          posFactor = util::limited (0, posFactor, 1);
          return detox (posFactor);
        }
    };
  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ZOOM_WINDOW_H*/
