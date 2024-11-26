/*
  IterExplorer(Test)  -  verify tree expanding and backtracking iterator

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-explorer-test.cpp
 ** The \ref IterExplorer_test covers and demonstrates a generic mechanism
 ** to expand and evaluate tree like structures. It was created in response to
 ** a recurring need for configurable tree expanding and backtracking evaluations.
 ** Due to the nature of Lumiera's design, we repeatedly encounter this kind of
 ** algorithms, when it comes to matching configuration and parametrisation against
 ** a likewise hierarchical and rules based model. To keep the code base maintainable,
 ** we deem it crucial to reduce the inherent complexity in such algorithms by clearly
 ** separate the _mechanics of evaluation_ from the actual logic of the target domain.
 ** 
 ** This test relies on a demonstration setup featuring a custom encapsulated state type:
 ** we rely on a counter with start and end value, embedded into an iterator as »state core«.
 ** This running counter, when iterated, generates a descending sequence of numbers start ... end.
 ** So -- conceptually -- this counting iterator can be conceived as _representing_ this sequence 
 ** of numbers, while not actually representing all these numbers as data in memory. And this is
 ** the whole point of the exercise: _not to represent_ this sequence in runtime state at once,
 ** rather to __pull and expand it on demand._
 ** 
 ** All these tests work by first defining these _functional structures_, which just
 ** yields an iterator entity. We get the whole structure it conceptually defines
 ** only if we »pull« and »materialise« this iterator until exhaustion — which essentially
 ** is what the test does to verify proper operation. In contrast, _Real World Code_ of course
 ** would not proceed in this way, like pulling everything from such an iterator. Since often
 ** the very reason we're using such a setup is the ability to represent infinite structures.
 ** Like e.g. the evaluation graph of video passed through a complex processing pipeline.
 **
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include "lib/iter-explorer.hpp"
#include "lib/meta/trait.hpp"

#include <utility>
#include <vector>
#include <limits>
#include <string>
#include <tuple>
#include <cmath>
#include <set>


namespace lib {
namespace test{
  
  using ::Test;
  using util::_Fmt;
  using util::isnil;
  using util::isSameObject;
  using lib::iter_stl::eachElm;
  using LERR_(ITER_EXHAUST);
  using std::vector;
  using std::string;
  
  
  namespace { // test substrate: simple number sequence iterator
    
    /**
     * This iteration _"state core" type_ describes
     * a descending sequence of numbers yet to be delivered.
     */
    struct CountDown
      {
        uint p,e;
        
        CountDown(uint start =0, uint end =0)
          : p(start)
          , e(end)
          { }
        
        bool
        checkPoint ()  const
          {
            return p > e;
          }
        
        uint&
        yield ()  const
          {
            return util::unConst (checkPoint()? p : e);
          }
        
        void
        iterNext ()
          {
            if (not checkPoint()) return;
            --p;
          }
        
        bool
        operator== (CountDown const& o)  const
          {
            return e == o.e
               and p == o.p;
          }
      };
    
    
    /**
     * A straight descending number sequence as basic test iterator.
     * It is built wrapping an opaque "state core" (of type CountDown).
     * @note the "state core" is not accessible from the outside
     */
    class NumberSequence
      : public IterStateWrapper<CountDown>
      {
        
      public:
        explicit
        NumberSequence(uint start = 0)
          : IterStateWrapper{CountDown{start}}
          { }
        NumberSequence(uint start, uint end)
          : IterStateWrapper{CountDown(start,end)}
          { }
      };
    
    
    
    /**
     * Another iteration _"state core"_ to produce a sequence of random numbers.
     * Used to build an infinite random search space...
     */
    class RandomSeq
      {
        size_t lim_;
        size_t cnt_;
        char letter_;
        
        static char
        rndLetter()
          {
            return 'A' + rani(26);
          }
        
      public:
        RandomSeq(int len  =0)
          : lim_{len>=0? len : std::numeric_limits<size_t>::max()}
          , cnt_{0}
          , letter_{rndLetter()}
          { }
        
        bool
        checkPoint ()  const
          {
            return cnt_ < lim_;
          }
        
        char&
        yield ()  const
          {
            return unConst(this)->letter_;
          }
        
        void
        iterNext ()
          {
            ASSERT (checkPoint());
            ++cnt_;
            letter_ = rndLetter();
          }
      };
    
    
    /** Diagnostic helper: join all the elements from the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {                    // note: copy here when given by-ref
      return util::join (std::forward<II> (ii), "-");
    }
    
    /** Diagnostic helper: "squeeze out" the given iterator until exhaustion */
    template<class II>
    inline void
    pullOut (II & ii)
    {
      while (ii)
        {
          cout << *ii;
          if (++ii) cout << "-";
        }
      cout << endl;
    }
    
  } // (END) test helpers
  
  
  
  
  
  
  
  /*******************************************************************//**
   * @test use a simple source iterator yielding numbers
   *       to build various functional evaluation pipelines,
   *       based on the \ref IterExplorer template.
   *       - the adapter to wrap the source, which can either
   *         [be a state core](\ref verify_wrappedState() ), or can
   *         [be a Lumiera Forward Iterator](\ref verify_wrappedIterator() )
   *       - the defining use case for IterExplorer is to build a
   *         [pipeline for depth-first exploration](\ref verify_expandOperation() )
   *         of a (functional) tree structure. This "tree" is  created by invoking
   *         a "expand functor", which can be defined in various ways.
   *       - the usual building blocks for functional evaluation pipelines, that is
   *         [filtering](\ref verify_FilterIterator() ) and
   *         [transforming](\ref verify_transformOperation() ) of
   *         the elements yielded by the wrapped source iterator.
   *       - building complex pipelines by combining the aforementioned building blocks
   *       - using an opaque source, hidden behind the IterSource interface, and
   *         an extension (sub interface) to allow for "tree exploration" without
   *         any knowledge regarding the concrete implementation of the data source.
   * 
   * # Explanation
   * 
   * These tests build a evaluation pipeline by _wrapping_ some kind of data source
   * and then layering some evaluation stages on top. There are two motivations why
   * one might want to build such a _filter pipeline:_
   * - on demand processing ("pull principle")
   * - separation of source computation and "evaluation mechanics"
   *   when building complex search and backtracking algorithms.
   * 
   * This usage style is inspired from the *Monad design pattern*. In our case here,
   * the Iterator pipeline would be the monad, and can be augmented and reshaped by
   * attaching further processing steps. How those processing steps are to be applied
   * remains an internal detail, defined by the processing pipeline. »Monads« are heavily
   * used in functional programming, actually they originate from Category Theory. Basically,
   * Monad is a pattern where we combine several computation steps in a specific way; but
   * instead of intermingling the individual computation steps and their combination,
   * the goal is to isolate and separate the _mechanics of combination_, so we can focus
   * on the actual _computation steps:_ The mechanics of combination are embedded into the
   * Monad type, which acts as a kind of container, holding some entities to be processed.
   * The actual processing steps are then attached to the monad as "function object" parameters.
   * It is up to the monad to decide if, and when those processing steps are applied to the
   * embedded values and how to combine the results into a new monad.
   * 
   * @see IterExplorer
   * @see IterAdapter
   */
  class IterExplorer_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          verify_wrappedState();
          verify_wrappedIterator();
          
          verify_expandOperation();
          verify_expand_rootCurrent();
          verify_transformOperation();
          verify_elementGroupingOperation();
          verify_aggregatingGroupItration();
          verify_combinedExpandTransform();
          verify_customProcessingLayer();
          verify_scheduledExpansion();
          verify_untilStopTrigger();
          verify_FilterIterator();
          verify_FilterChanges();
          verify_asIterSource();
          verify_IterSource();
          verify_reduceVal();
          verify_effuse();
          verify_dedup();
          
          verify_depthFirstExploration();
          demonstrate_LayeredEvaluation();
        }
      
      
      
      /** @test without using any extra functionality,
       *        IterExplorer just wraps an iterable state.
       */
      void
      verify_wrappedState()
        {
          auto ii = explore (CountDown{5,0});
          CHECK (!isnil (ii));
          CHECK (5 == *ii);
          ++ii;
          CHECK (4 == *ii);
          pullOut(ii);
          CHECK ( isnil (ii));
          CHECK (!ii);
          
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
          
          ii = explore (CountDown{5});
          CHECK (materialise(ii) == "5-4-3-2-1"_expect);
          ii = explore (CountDown{7,4});
          CHECK (materialise(ii) == "7-6-5"_expect);
          ii = explore (CountDown{});
          CHECK ( isnil (ii));
          CHECK (!ii);
        }
      
      
      /** @test IterExplorer is able to wrap any _Lumiera Forward Iterator_ */
      void
      verify_wrappedIterator()
        {
          vector<int> numz{1,-2,3,-5,8,-13};
          auto ii = eachElm(numz);
          CHECK (!isnil (ii));
          CHECK (1 == *ii);
          ++ii;
          CHECK (-2 == *ii);
          
          auto jj = explore(ii);
          CHECK (!isnil (jj));
          CHECK (-2 == *jj);
          ++jj;
          CHECK (3 == *jj);
          
          // we passed a LValue-Ref, thus a copy was made
          CHECK (-2 == *ii);

          CHECK (materialise(ii) == "-2-3--5-8--13");
          CHECK (materialise(jj) ==    "3--5-8--13");
          
          // can even adapt STL container automatically
          auto kk = explore(numz);
          CHECK (!isnil (kk));
          CHECK (1 == *kk);
          CHECK (materialise(kk) == "1--2-3--5-8--13");
        }
      
      
      
      /** @test use a preconfigured "expand" functor to recurse into children.
       * The `expand()` builder function predefines a way how to _expand_ the current
       * head element of the iteration. However, expansion does not happen automatically,
       * rather, it needs to be invoked by the client, similar to increment of the iterator.
       * When expanding, the current head element is consumed and fed into the expand functor;
       * the result of this functor invocation is injected instead into the result sequence,
       * and consequently this result needs to be again an iterable with compatible value type.
       * Conceptually, the evaluation _forks into the children of the expanded element_, before
       * continuing with the successor of the expansion point. Obviously, expansion can be applied
       * again on the result of the expansion, possibly leading to a tree of side evaluations.
       * 
       * The expansion functor may be defined in various ways and will be adapted appropriately
       * - it may follow the classical "monadic pattern", i.e. take individual _values_ and return
       *   a _"child monad"_, which is then "flat mapped" (integrated) into the resulting iteration
       * - the resulting child collection may be returned as yet another iterator, which is then
       *   moved by the implementation into the stack of child sequences currently in evaluation
       * - or alternatively the resulting child collection may be returned just as a "state core",
       *   which can be adapted into a _iterable state_ (see lib::IterStateWrapper).
       * - or it may even return the reference to a STL collection existing elsewhere,
       *   which will then be iterated to yield the child elements
       * - and, quite distinct to the aforementioned "monadic" usage, the expansion functor
       *   may alternatively be written in a way as to collaborate with the "state core" used
       *   when building the IterExplorer. In this case, the functor typically takes a _reference_
       *   to this underlying state core or iterator. The purpose for this definition variant is
       *   to allow exploring a tree-like evaluation, without the need to disclose anything about
       *   the backing implementation; the expansion functor just happens to know the implementation
       *   type of the "state core" and manipulate it through its API to create a "derived core"
       *   representing a _child evaluation state_.
       * - and finally, there is limited support for _generic lambdas._ In this case, the implementation
       *   will try to instantiate the passed lambda by using the concrete source iterator type as argument.
       * 
       * @note expansion functor may use side-effects and indeed return something entirely different
       *       than the original sequence, as long as it is iterable and yields compatible values.
       */
      void
      verify_expandOperation()
        {
          /* == "monadic flatMap" == */
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](uint j){ return CountDown{j-1}; })                                   // expand-functor: Val > StateCore
                      );
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](uint j){ return NumberSequence{j-1}; })                              // expand-functor: Val > Iter
                      );                // NOTE: different Iterator type than the source!
          
          // lambda with side-effect and return type different from source iter
          vector<vector<uint>> childBuffer;
          auto expandIntoChildBuffer = [&](uint j) -> vector<uint>&
                                          {
                                            childBuffer.emplace_back();
                                            vector<uint>& childNumbz = childBuffer.back();
                                            for (size_t i=0; i<j-1; ++i)
                                              childNumbz.push_back(j-1 - i);
                                            return childNumbz;
                                          };
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand(expandIntoChildBuffer)                                                  // expand-functor: Val > STL-container&
                      );
          
          // test routine called the expansion functor five times
          CHECK (5 == childBuffer.size());
          
          
          
          /* == "state manipulation" use cases == */
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](CountDown const& core){ return CountDown{ core.yield() - 1}; })      // expand-functor: StateCore const& -> StateCore
                      );
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](CountDown core){ return NumberSequence{ core.yield() - 1}; })        // expand-functor: StateCore -> Iter
                      );
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](auto & it){ return CountDown{ *it - 1}; })                           // generic Lambda: Iter& -> StateCore
                      );
          
          verify_treeExpandingIterator(
                    explore(CountDown{5})
                      .expand([](auto it){ return decltype(it){ *it - 1}; })                          // generic Lambda: Iter -> Iter
                      );
        }
      
      
      template<class EXP>
      void
      verify_treeExpandingIterator (EXP ii)
        {
          CHECK (!isnil (ii));
          CHECK (5 == *ii);
          ++ii;
          CHECK (4 == *ii);
          
          CHECK (0 == ii.depth());
          ii.expandChildren();
          CHECK (3 == *ii);
          CHECK (1 == ii.depth());
          ++ii;
          CHECK (2 == *ii);
          CHECK (1 == ii.depth());
          ii.expandChildren();
          CHECK (1 == *ii);
          CHECK (2 == ii.depth());
          ++ii;
          CHECK (1 == *ii);
          CHECK (1 == ii.depth());
          ++ii;
          CHECK (3 == *ii);
          CHECK (0 == ii.depth());
          CHECK (materialise(ii) == "3-2-1");
          ii.expandChildren();
          CHECK (1 == ii.depth());
          CHECK (materialise(ii) == "2-1-2-1");
          ++++ii;
          CHECK (0 == ii.depth());
          CHECK (materialise(ii) == "2-1");
          ii.expandChildren();
          CHECK (1 == ii.depth());
          CHECK (materialise(ii) == "1-1");
          ++ii;
          CHECK (0 == ii.depth());
          CHECK (1 == *ii);
          CHECK (materialise(ii) == "1");
          ii.expandChildren();
          CHECK (isnil (ii));
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
        }
      
      
      /** @test special feature of the Expander to lock into current child sequence.
       * This feature was added to support a specific use-case in the IterChainSearch component.
       * After expanding several levels deep into a tree, it allows to turn the _current child sequence_
       * into a new root sequence and discard the whole rest of the tree, including the original root sequence.
       * It is implemented by moving the current child sequence down into the root sequence. We demonstrate
       * this behaviour with the simple standard setup from #verify_expandOperation()
       */
      void
      verify_expand_rootCurrent()
        {
          auto tree = explore(CountDown{25})
                        .expand([](uint j){ return CountDown{j-1}; });
          
          CHECK (materialise(tree) == "25-24-23-22-21-20-19-18-17-16-15-14-13-12-11-10-9-8-7-6-5-4-3-2-1");
          
          CHECK (0 == tree.depth());
          CHECK (25 == *tree);
          ++tree;
          ++tree;
          ++tree;
          ++tree;
          CHECK (21 == *tree);
          tree.expandChildren();
          CHECK (1 == tree.depth());
          ++tree;
          ++tree;
          ++tree;
          ++tree;
          ++tree;
          CHECK (15 == *tree);
          tree.expandChildren();
          ++tree;
          ++tree;
          CHECK (2 == tree.depth());
          CHECK (materialise(tree) == "12-11-10-9-8-7-6-5-4-3-2-1-"                          // this is the level-2 child sequence
                                      "14-13-12-11-10-9-8-7-6-5-4-3-2-1-"                    // ...returning to the rest of the level-1 sequence
                                      "20-19-18-17-16-15-14-13-12-11-10-9-8-7-6-5-4-3-2-1"); // ...followed by the rest of the original root sequence
          CHECK (12 == *tree);
          
          tree.rootCurrent();
          CHECK (12 == *tree);
          CHECK (materialise(tree) == "12-11-10-9-8-7-6-5-4-3-2-1");  // note: level-2 continues unaltered, but level-1 and the original root are gone.
          CHECK (0 == tree.depth());
        }
      
      
      
      /** @test pipe each result through a transformation function.
       * The _transforming iterator_ is added as a decorator, wrapping the original iterator,
       * TreeEplorer or state core. As you'd expect, the given functor is required to accept
       * compatible argument types, and a generic lambda is instantiated to take a reference
       * to the embedded iterator's value type. Several transformation steps can be chained,
       * and the resulting entity is again a Lumiera Forward Iterator with suitable value type.
       * The transformation function is invoked only once per step and the result produced by
       * this invocation is placed into a holder buffer embedded within the iterator.
       * @note since the implementation uses the same generic adaptor framework,
       *       the transformation functor may be defined with the same variations
       *       as described for the expand-operation above. In theory, it might
       *       collaborate with the embedded "state core" type, thereby possibly
       *       bypassing other decorators added below.
       * @warning don't try this at home
       */
      void
      verify_transformOperation()
        {
          auto multiply  = [](int v){ return 2*v; };                             // functional map: value -> value
          
          _Fmt embrace{"≺%s≻"};
          auto formatify = [&](auto it){ return string{embrace % *it}; };        // generic lambda: assumed to take an Iterator&
          
          
          auto ii = explore(CountDown{7,4})
                      .transform(multiply)
                      ;
          
          CHECK (14 == *ii);
          CHECK (14 == *ii);
          ++ii;
          CHECK (12 == *ii);
          ++ii;
          CHECK (10 == *ii);
          ++ii;
          CHECK (isnil (ii));
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
          
          
          
          // demonstrate chaining of several transformation layers
          vector<int64_t> numz{1,-2,3,-5,8,-13};
          
          CHECK ("≺1≻-≺-2≻-≺3≻-≺-5≻-≺8≻-≺-13≻"                == materialise (explore(numz)
                                                                                .transform(formatify)) );
          
          CHECK ("≺2≻-≺-4≻-≺6≻-≺-10≻-≺16≻-≺-26≻"              == materialise (explore(numz)
                                                                                .transform(multiply)
                                                                                .transform(formatify)) );
          
          CHECK ("≺≺4≻≻-≺≺-8≻≻-≺≺12≻≻-≺≺-20≻≻-≺≺32≻≻-≺≺-52≻≻" == materialise (explore(numz)
                                                                                .transform(multiply)
                                                                                .transform(multiply)
                                                                                .transform(formatify)
                                                                                .transform(formatify)) );
          
          
          // demonstrate the functor is evaluated only once per step
          int fact = 3;
          
          auto jj = explore (CountDown{4})
                      .transform([&](int v)
                                    {
                                      v *=fact;
                                      fact *= -2;
                                      return v;
                                    });
          CHECK (3*4 == *jj);
          CHECK (fact == -2*3);
          
          CHECK (3*4 == *jj);
          CHECK (3*4 == *jj);
          
          ++jj;
          CHECK (fact == -2*3);    // NOTE : functor is evaluated on first demand
          CHECK (-2*3*3 == *jj);   //        ...which happens on yield (access the iterator value)
          CHECK (fact == 2*2*3);   //        and this also causes the side-effect
          CHECK (-2*3*3 == *jj);
          CHECK (-2*3*3 == *jj);
          CHECK (fact == 2*2*3);   //        no further evaluation and thus no further side-effect
          
          ++jj;
          CHECK (2*2*3*2 == *jj);
          CHECK (fact == -2*2*2*3);
          
          fact = -23;
          CHECK (2*2*3*2 == *jj);
          
          ++jj;
          CHECK (fact == -23);
          CHECK (-23*1 == *jj);
          CHECK (fact == 2*23);
          
          ++jj;
          CHECK (isnil (jj));
          CHECK (fact == 2*23);
          
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          CHECK (fact == 2*23);    // exhaustion detected on source and thus no further evaluation
          
          
          
          // demonstrate a transformer accessing the source state core...
          // should not be relevant in practice, but works due to the generic adapters
          auto kk = explore (CountDown{9,4})
                      .transform([](CountDown& core)
                                   {
                                     uint delta = core.p - core.e;
                                     if (delta % 2 == 0)
                                       --core.p;     // EVIL EVIL
                                     return delta;
                                   });
          
          CHECK (5 == *kk); // the delta between 9 (start) and 4 (end)
          ++kk;
          CHECK (4 == *kk); // Core manipulated by SIDE-EFFECT at this point...
          CHECK (4 == *kk); // ...but not yet obvious, since the result is cached
          ++kk;
          CHECK (2 == *kk); // Surprise -- someone ate my numberz...
          ++kk;
          CHECK (isnil (kk));
        }
      
      
      
      /** @test package elements from the source pipeline into fixed-sized groups.
       * These groups are implemented as std::array and initialised with the values
       * yielded consecutively from the underlying source pipeline. The main iterator
       * then yields a reference to this data (which can be unpacked conveniently
       * by a structured binding, or processed as a STL container.
       * Moreover, there is a secondary interface, allowing to iterate over the
       * values stored in this group; this is also exposed for the rest, which
       * did not suffice to fill a full group.
       */
      void
      verify_elementGroupingOperation()
        {
          auto showGroup = [](auto it){ return "["+util::join(*it)+"]"; };
          CHECK (materialise (
                    explore(CountDown{10})
                      .grouped<3>()
                      .transform(showGroup)
                    )
                 == "[10, 9, 8]-[7, 6, 5]-[4, 3, 2]"_expect);
          
          
          auto ii = explore(CountDown{23})
                      .grouped<5>();
          CHECK(ii);
          CHECK(ii.getGroupedElms());
          CHECK(not ii.getRestElms());
          CHECK (materialise(ii.getGroupedElms()) == "23-22-21-20-19"_expect);
          
          CHECK ( test::showType<decltype(*ii)>()== "array<uint, 5ul>&"_expect);
          
          uint s = *(ii.getGroupedElms());
          for ( ; ii; ++ii)
            {
              auto grp = *ii;
              CHECK (5 == grp.size());
              auto& [a,b,c,d,e] = grp;
              CHECK (a == s);
              CHECK (b == a-1);
              CHECK (c == a-2);
              CHECK (d == a-3);
              CHECK (e == a-4);
              CHECK (not ii.getRestElms());
              s -= 5;
            }
          CHECK (s < 5);
          CHECK (s == 3);
          
          CHECK (not ii);
          CHECK(ii.getGroupedElms());
          CHECK(ii.getRestElms());
          CHECK (materialise(ii.getGroupedElms()) == "3-2-1"_expect);
          CHECK (materialise(ii.getRestElms())    == "3-2-1"_expect);
          
          
          auto iii = explore(CountDown{4})
                      .grouped<5>();
          CHECK (not iii);
          CHECK (materialise(iii.getRestElms())    == "4-3-2-1"_expect);
        }
      
      
      /** @test another form of grouping, where groups are formed by a derived property
       * thereby passing each element in the group to an aggregator function, working on
       * an accumulator per group. Downstream, the resulting, accumulated value is exposed
       * for each group, while consuming all source values belonging to this group.
       * - in the simple form, all members of a group are "added" together
       * - the elaborate form allows to provide a custom aggregation function, which takes
       *   the »accumulator« as first argument by reference; the type of this argument
       *   implicitly defines what is instantiated for each group and yielded as result.
       */
      void
      verify_aggregatingGroupItration()
        {
          CHECK (materialise (
                    explore(CountDown{10})
                      .groupedBy(std::ilogbf)
                    )
                 == "27-22-5-1"_expect);       // 10+9+8|7+6+5+4|3+2|1

          CHECK (materialise (
                    explore(CountDown{10})
                      .transform(util::toString<uint>)
                      .groupedBy([](auto& it) { return std::ilogbf (it.p); })    // note trickery: takes not the value, rather the iterator and
                    )                                                           //  accesses internals of CountDown, bypassing the transform layer above
                 == "1098-7654-32-1"_expect);  // `+` does string concatenation
          
          
          auto showGroup = [](auto it){ return "["+util::join(*it)+"]"; };
          // elaborate form with custom aggregation...
          CHECK (materialise (
                    explore(CountDown{10})
                      .groupedBy(std::ilogbf
                                ,[](vector<uint>& accum, uint val)
                                    {
                                      accum.push_back (val);
                                    })
                      .transform(showGroup)
                    )
                 == "[10, 9, 8]-[7, 6, 5, 4]-[3, 2]-[1]"_expect);
        }
      
      
      /** @test combine the recursion into children with a tail mapping operation.
       * Wile basically this is just the layering structure of IterExplorer put into action,
       * you should note one specific twist: the iter_explorer::Expander::expandChildren() call
       * is meant to be issued from ``downstream'', from the consumer side. Yet the consumer at
       * that point might well see the items as processed by a transforming step layered on top.
       * So what the consumer sees and thinks will be expanded need not actually be what will
       * be processed by the _expand functor_. This may look like a theoretical or cosmetic
       * issue -- yet in fact it is this tiny detail which is crucial to make abstraction of
       * the underlying data source actually work in conjunction with elaborate searching and
       * matching algorithms. Even more so, when other operations like filtering are intermingled;
       * in that case it might even happen that the downstream consumer does not even see the
       * items resulting from child expansion, because they are evaluated and then filtered
       * away by transformers and filters placed in between.
       * @note as a consequence of the flexible automatic adapting of bound functors, it is
       *       possible for bound functors within different "layers" to collaborate, based on
       *       additional knowledge regarding the embedded data source internals. This test
       *       demonstrates a transform functor, which takes the _source iterator_ as argument
       *       and invokes `it.expandChildren()` to manipulate the underlying evaluation.
       *       However, since the overall evaluation is demand driven, there are inherent
       *       limitations to such a setup, which bends towards fragility when leaving
       *       the realm of pure functional evaluation.
       */
      void
      verify_combinedExpandTransform()
        {
          auto ii = explore(CountDown{5})
                      .expand([](uint j){ return CountDown{j-1}; })
                      .transform([](int v){ return 2*v; })
                      ;
          
          CHECK ("int" == meta::typeStr(*ii));  // result type is what the last transformer yields
          CHECK (10 == *ii);
          ++ii;
          CHECK (8 == *ii);
          ii.expandChildren();
          CHECK ("6-4-2-6-4-2" == materialise(ii) );
          
          
          // the following contrived example demonstrates
          // how intermediary processing steps may interact
          
          CHECK (materialise (
                    explore(CountDown{5})
                      .expand([](uint j){ return CountDown{j-1}; })
                      .transform([](int v){ return 2*v; })
                      .transform([](auto& it)
                                   {
                                     auto elm = *it;
                                     if (elm == 6)
                                       {
                                         it.expandChildren();         // NOTE at that point we're forced to decide if
                                         elm = *it * 10;              //      we want to return the parent or the 1st child
                                       }
                                     return elm;
                                   })
                      .transform([](float f){ return 0.055 + f/2; })
                 )
                 == "5.055-4.055-20.055-1.055-2.055-1.055" );
        }
      
      
      /**
       * demo of a custom processing layer
       * interacting directly with the iteration mechanism.
       * @note we can assume `SRC` is itself a Lumiera »State Core«
       */
      template<class SRC>
      struct MagicTestRubbish
        : public SRC
        {
          using SRC::SRC;
          
          void
          iterNext()
            {
              ++(*this);
              if (*this)
                ++(*this);
            }
        };
      
      /** @test extension point to inject a client-defined custom processing layer
       * This special builder function allows to install a template, which needs to wrap
       * a source iterator and expose a _state core like_ interface. We demonstrate this
       * extension mechanism here by defining a processing layer which skips each other element.
       */
      void
      verify_customProcessingLayer()
        {
          CHECK (materialise(
                    explore(CountDown{7})
                      .processingLayer<MagicTestRubbish>()
                )
                == "7-5-3-1");
          
          CHECK (materialise(
                    explore(CountDown{7})
                      .transform([](uint v){ return 2*v; })
                      .processingLayer<MagicTestRubbish>()
                      .filter([](int v){ return v % 3; })
                )
                == "14-10-2");
        }
      
      
      
      /** @test child expansion can be scheduled to happen on next iteration.
       * As such, _"child expansion"_ happens right away, thereby consuming a node
       * and replacing it with its child sequence. Sometimes, when building search and matching
       * algorithms, we rather just want to _plan_ a child expansion to happen on next increment.
       * Such is especially relevant when searching for a locally or global maximal solution, which
       * is rather simple to implement with an additional filtering layer -- and this approach requires
       * us to deliver all partial solutions for the filter layer to act on. Obviously this functionality
       * leads to additional state and thus is provided as optional layer in the IterExplorer builder.
       */
      void
      verify_scheduledExpansion()
        {
          auto ii = explore(CountDown{6})
                      .expand([](uint j){ return CountDown{j-2}; })
                      .expandOnIteration();
          
          CHECK (!isnil (ii));
          CHECK (6 == *ii);
          ++ii;
          CHECK (5 == *ii);
          CHECK (ii.depth() == 0);
          
          ii.expandChildren();
          CHECK (5 == *ii);
          CHECK (ii.depth() == 0);
          ++ii;
          CHECK (3 == *ii);
          CHECK (ii.depth() == 1);
          
          ii.expandChildren();
          ii.expandChildren();
          CHECK (ii.depth() == 1);
          CHECK (3 == *ii);
          ++ii;
          CHECK (1 == *ii);
          CHECK (ii.depth() == 2);
          ++ii;
          CHECK (2 == *ii);
          CHECK (ii.depth() == 1);
          
          ii.expandChildren();
          ++ii;
          CHECK (1 == *ii);
          CHECK (ii.depth() == 1);
          ++ii;
          CHECK (4 == *ii);
          CHECK (ii.depth() == 0);
          ++ii;
          CHECK (3 == *ii);
          ++ii;
          CHECK (2 == *ii);
          ++ii;
          CHECK (1 == *ii);
          ++ii;
          CHECK (isnil (ii));
        }
      
      
      
      /** @test control end of iteration by a stop condition predicate.
       * When decorating the pipeline with this adapter, iteration end depends not only on
       * the source iterator, but also on the end condition; once the condition flips, the
       * overall pipeline iterator is exhausted and can never be re-activated again (unless
       * some special trickery is done by conspiring with the data source)
       */
      void
      verify_untilStopTrigger()
        {
          CHECK (materialise (
                    explore (CountDown{10})
                      .iterUntil([](uint j){ return j < 5; })
                    )
                 == "10-9-8-7-6-5"_expect);
          
          CHECK (materialise (
                    explore (CountDown{10})
                      .iterWhile([](uint j){ return j > 5; })
                    )
                 == "10-9-8-7-6"_expect);
          
          CHECK (materialise (
                    explore (CountDown{10})
                      .iterWhile([](int j){ return j > -5; })
                    )
                 == "10-9-8-7-6-5-4-3-2-1"_expect);
          
          CHECK (materialise (
                    explore (CountDown{10})
                      .iterWhile([](uint j){ return j > 25; })
                    )
                 == ""_expect);
        }
      
      
      
      /** @test add a filtering predicate into the pipeline.
       * As in all the previously demonstrated cases, also the _filtering_ is added as decorator,
       * wrapping the source and all previously attached decoration layers. And in a similar way,
       * various kinds of functors can be bound, and will be adapted automatically to work as a
       * predicate to approve the elements to yield.
       */
      void
      verify_FilterIterator()
        {
          // canonical example, using a clean side-effect free predicate based on element values
          CHECK (materialise (
                    explore(CountDown{10})
                      .filter([](uint j){ return j % 2; })
                    )
                 == "9-7-5-3-1"_expect);
          
          
          // Filter may lead to consuming util exhaustion...
          auto ii = explore(CountDown{10})
                      .filter([](int j){ return j > 9; });
          
          CHECK (not isnil (ii));
          CHECK (10 == *ii);
          ++ ii;
          CHECK (isnil (ii));
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
          
          
          // none of the source elements can be approved here...
          auto jj = explore(CountDown{5})
                      .filter([](int j){ return j > 9; });
          
          CHECK (isnil (jj));
          
          
          
          // a tricky example, where the predicate takes the source core as argument;
          // since the source core is embedded as baseclass, it can thus "undermine"
          // and bypass the layers configured in between; here the transformer changes
          // uint to float, but the filter interacts directly with the core and thus
          // judges based on the original values
          CHECK (materialise (
                    explore(CountDown{10,4})
                      .transform([](float f){ return 0.55 + 2*f; })
                      .filter([](CountDown& core){ return core.p % 2; })
                    )
                 == "18.55-14.55-10.55"_expect);
          
          
          
          // contrived example to verify interplay of filtering and child expansion;
          // especially note that the filter is re-evaluated after expansion happened.
          CHECK (materialise (
                    explore(CountDown{10})
                      .expand([](uint i){ return CountDown{i%4==0? i-1 : 0}; })      // generate subtree at 8 and 4 ==> 10-9-8-7-6-5-4-3-2-1-3-2-1-7-6-5-4-3-2-1-3-2-1
                      .filter([](uint i){ return i%2 == 0; })
                      .expandAll()                                                   // Note: sends the expandChildren down through the filter
                    )
                 == "10-8-6-4-2-2-6-4-2-2"_expect);
          
          
          
          // another convoluted example to demonstrate
          // - a filter predicate with side-effect
          // - and moreover the predicate is a generic lambda
          // - accepting the iterator to trigger child expansion
          // - which also causes re-evaluation of the preceding transformer
          bool toggle = false;
          auto kk = explore(CountDown{10,5})
                      .expand([](uint j){ return CountDown{j-1}; })
                      .transform([](int v){ return 2*v; })
                      .filter([&](auto& it)
                                   {
                                     if (*it == 16)
                                       {
                                         it.expandChildren();
                                         toggle = true;
                                       }
                                     return toggle;
                                   });
          
          CHECK (materialise(kk)
                 == "14-12-10-8-6-4-2-14-12"_expect);
                 // Explanation:
                 // The source starts at 10, but since the toggle is false,
                 // none of the initial values makes it though to the result.
                 // The interspersed transformer doubles the source values, and
                 // thus at source == 8 the trigger value (16) is hit. Thus the
                 // filter now flips the context-bound toggle (side-effect) and
                 // then expands children, which consumes current source value 8
                 // to replace it with the sequence 7,6,5,4,3,2,1, followed by
                 // the rest of the original sequence, 7,6 (which stops above 5).
          
          CHECK (materialise(kk.filter([](long i){ return i % 7; }))
                 == "12-10-8-6-4-2-12"_expect);
                 // Explanation:
                 // Since the original IterExplorer was assigned to variable kk,
                 // the materialise()-Function got a lvalue-ref and thus made a copy
                 // of the whole compound. For that reason, the original state within
                 // kk still rests at 7 -- because the filter evaluates eagerly, the
                 // source was pulled right at construction until we reached the first
                 // value to yield, which is the first child (7,....) within the
                 // expanded sequence. But now, in the second call to materialise(),
                 // we don't just copy, rather we add another filter layer on top,
                 // which happens to filter away this first result (== 2*7), and
                 // also the first element of the original sequence after the
                 // expanded children
          
          // WARNING: kk is now defunct, since we moved it into the builder expression
          //          and then moved the resulting extended iterator into materialise!
        }
      
      
      
      /** @test a special filter layer which can be re-configured on the fly */
      void
      verify_FilterChanges()
        {
          auto seq = explore(CountDown{20})
                       .mutableFilter();
          
          auto takeEve = [](uint i){ return i%2 == 0; };
          auto takeTrd = [](uint i){ return i%3 == 0; };
          
          CHECK (20 == *seq);
          ++seq;
          CHECK (19 == *seq);
          CHECK (19 == *seq);
          
          seq.andFilter (takeEve);
          CHECK (18 == *seq);
          ++seq;
          CHECK (16 == *seq);
          
          seq.andFilter (takeTrd);
          CHECK (12 == *seq);   //  is divisible (by 2 AND by 3)
          
          seq.flipFilter();
          CHECK (11 == *seq);   // not divisible (by 2 AND by 3)
          ++seq;
          CHECK (10 == *seq);
          
          seq.setNewFilter (takeTrd);
          CHECK ( 9 == *seq);
          ++seq;
          CHECK ( 6 == *seq);
          
          seq.orNotFilter (takeEve);
          CHECK ( 6 == *seq);
          ++seq;
          CHECK ( 5 == *seq);   // disjunctive condition actually weakens the filter
          ++seq;
          CHECK ( 3 == *seq);
          
          // NOTE: arbitrary functors can be used/combined,
          //       since they are adapted individually.
          // To demonstrate this, we use a functor accessing and
          // manipulating the state core by side effect...
          string buff{"."};
          seq.andNotFilter ([&](CountDown& core)
                              {
                                buff += util::toString(core.p) + ".";
                                --core.p;                              // manipulate state core
                                return core.p % 2;                     // return a number, not bool
                              });
          
          CHECK ( 2 == *seq);                   // value in the core has been manipulated
          CHECK (".3." == buff);                // the filter has been invoked once, and saw core == 3
          
          ++seq;                                // core == 2 is filtered by the existing other filter (== not take even)
          CHECK (".3.1." == buff);              // the filter has been invoked again, and saw core == 1
          CHECK (0 == seq.p);                   // ...which he manipulated, so that core == 0
          CHECK (isnil (seq));                  // .....and thus iteration end is detected
          VERIFY_ERROR (ITER_EXHAUST, *seq );
          
          
          // verify enabling and disabling...
          seq = explore(CountDown{10})
                  .mutableFilter(takeTrd);
          
          CHECK (9 == *seq);
          seq.disableFilter();
          CHECK (9 == *seq);
          ++seq;
          CHECK (8 == *seq);
          seq.andNotFilter (takeEve);
          CHECK (7 == *seq);
          ++seq;
          CHECK (5 == *seq);
          seq.disableFilter();
          CHECK (5 == *seq);
          ++seq;
          CHECK (4 == *seq);
          ++seq;
          CHECK (3 == *seq);
          seq.flipFilter();  // everything rejected
          CHECK (isnil (seq));
        }
      
      
      
      
      /** @test verify _terminal operation_ to sum or reduce all values from the pipeline.
       */
      void
      verify_reduceVal()
        {
          auto accumulated = explore(CountDown{30})
                               .transform([](int i){ return i-1; })              // note: implicitly converts uint -> int
                               .resultSum();
          
          using Res = decltype(accumulated);
          CHECK (lib::test::showType<Res>()  == "int"_expect);
          
          auto expectedSum = [](auto N){ return N*(N+1) / 2; };
          CHECK (accumulated == expectedSum(29));
          
          // In the general case an accessor and a junctor can be given...
          CHECK (explore(CountDown{10})
                   .reduce([](int i){ return i - 0.5; }                          // accessor: produce a double
                          ,[](string accu, float val)
                                    {
                                      return accu+">"+util::toString(val);       // junctor:  convert to String and combine with separator char
                                    }
                          , string{">-"}                                         // seedVal:  starting point for the reduction; also defines result type
                          )
                 == ">->9.5>8.5>7.5>6.5>5.5>4.5>3.5>2.5>1.5>0.5"_expect);
          
          // If only the accessor is given, values are combined by std::plus...
          CHECK (explore(CountDown{9})
                   .reduce([](auto it) -> string
                             {
                               return _Fmt{"○%s●"} % *it;                        // accessor: format into a string
                             })
                 == "○9●○8●○7●○6●○5●○4●○3●○2●○1●"_expect);
          
          // a predefined IDENTITY accessor takes values from the pipeline as-is
          CHECK (explore(CountDown{9})
                   .reduce(iter_explorer::IDENTITY, std::minus<int>(), expectedSum(9))
                 == 0);
        }
      
      
      
      /** @test verify _terminal operation_ to append all results into a container.
       */
      void
      verify_effuse()
        {
          auto solidified = explore(CountDown{20})
                               .filter   ([](uint i){ return i % 2; })
                               .transform([](uint i){ return 0.5*i; })
                               .effuse();
          
          using Res = decltype(solidified);
          CHECK (lib::test::showType<Res>()  == "vector<double>"_expect);
          CHECK (util::join(solidified, "|") == "9.5|8.5|7.5|6.5|5.5|4.5|3.5|2.5|1.5|0.5"_expect);
        }
      
      
      /** @test verify to deduplicate the iterator's results into a std::set
       */
      void
      verify_dedup()
        {
          CHECK (materialise (
                    explore(CountDown{23})
                      .transform([](uint j){ return j % 5; })
                      .deduplicate()
                    )
                 == "0-1-2-3-4"_expect);  // note: values were also sorted ascending by std::set
        }
      
      
      
      
      /** @test package the resulting Iterator as automatically managed,
       *        polymorphic opaque entity implementing the IterSource interface.
       * The builder operations on IterExplorer each generate a distinct, implementation
       * defined type, which is meant to be captured by `auto`. However, the terminal builder
       * function `asIterSource()` moves the whole compound iterator object, as generated by
       * preceding builder steps, into a heap allocation and exposes a simplified front-end,
       * which is only typed to the result value type. Obviously, the price to pay comes in
       * terms of virtual function calls for iteration, delegating to the pipeline backend.
       * - thus a variable typed to that front-end, `IterSource<VAL>` is polymorphic and
       *   can be reassigned at runtime with an entirely different pipeline.
       * - but this structure also has the downside, that the implementation no longer
       *   resides directly within the iterator: several front-end copies share the
       *   same back-end. Note however that the behaviour of iterators copied this
       *   way is _implementation defined_ anyway. There is never a guarantee that
       *   a clone copy evolves with state independent from its ancestor; it just
       *   happens to work this way in many simple cases. You should never use
       *   more than one copy of a given iterator at any time, and you should
       *   discard it, when done with iteration.
       * - actually, the returned front-end offers an extended API over plain vanilla
       *   `IterSource<T>::iterator`, to expose the `expandChildren()` operation.
       */
      void
      verify_asIterSource()
        {
          IterSource<uint>::iterator sequence;     // note `sequence` is polymorphic
          CHECK (isnil (sequence));
          
          sequence = explore(CountDown{20,10})
                        .filter([](uint i){ return i % 2; })
                        .asIterSource();           // note this terminal builder function
                                                   // moves the whole pipeline onto the heap
          CHECK (not isnil (sequence));
          CHECK (19 == *sequence);
          
          
          // use one sequence as source to build another one
          sequence = explore(sequence)
                        .transform([](uint i){ return i*2; })
                        .asIterSource();
          
          CHECK (38 == *sequence);
          CHECK ("38-34-30-26-22" == materialise(sequence));
          
          // WARNING pitfall: `sequence` is a copyable iterator front-end
          //                  but holds onto the actual pipeline by shared-ptr
          //                  Thus, even while materialise() creates a copy,
          //                  the iteration state gets shared....
          CHECK (22 == *sequence);
          ++sequence;         // ...and even worse, iteration end is only detected after increment
          CHECK (isnil (sequence));
          
          
          // extended API to invoke child expansion opaquely
          IterExploreSource<char> exploreIter;
          CHECK (isnil (exploreIter));
          
          exploreIter = explore(CountDown{20,10})
                          .filter([](uint i){ return i % 2; })
                          .transform([](uint i){ return i*2; })
                          .filter([](int i){ return i>25; })
                          .expand([](uint i){ return CountDown{i-10, 20}; })
                          .transform([](uint u) -> char { return '@'+u-20; })
                          .asIterSource();
          
          
          CHECK ('R' == *exploreIter);   // 38-20 + '@'
          ++exploreIter;
          CHECK ('N' == *exploreIter);   // 34-20 + '@'
          
          exploreIter.expandChildren();  // expand consumes the current element (34)
                                         // and injects the sequence (24...20[ instead
          CHECK ('D' == *exploreIter);   // 34-10 == 24 and 'D' ==  24-20 + '@'
          
          CHECK ("D-C-B-A-J-F" == materialise(exploreIter));
        }                                // note how the remainder of the original sequence is picked up with 'J'...
      
      
      
      
      /** @test ability to wrap and handle IterSource based iteration.
       * Contrary to the preceding test case, here the point is to _base the whole pipeline_
       * on a data source accessible through the IterSource (VTable based) interface. The notable
       * point with this technique is the ability to use some _extended sub interface of IterSource_
       * and to rely on this interface to implement some functor bound into the IterExplorer pipeline.
       * Especially this allows to delegate the "child expansion" through such an interface and just
       * return a compatible IterSource as result. This way, the opaque implementation gains total
       * freedom regarding the concrete implementation of the "child series" iterator. In fact,
       * it may even use a different implementation on each level or even on each individual call;
       * only the result type and thus the base interface need to match.
       */
      void
      verify_IterSource()
        {
          class PrivateSource
            : public IterSource<uint>
            {
            public:
              virtual PrivateSource* expandChildren()  const  =0;
            };
          
          class VerySpecificIter
            : public WrappedLumieraIter<NumberSequence
            ,        PrivateSource     >
            {
            public:
              VerySpecificIter(uint start)
                : WrappedLumieraIter(NumberSequence{start})
                { }
              
              virtual PrivateSource*
              expandChildren()  const override
                {
                  return new VerySpecificIter{*wrappedIter() - 2};
                }
              
              uint
              currentVal()  const
                {
                  return *wrappedIter();
                }
            };
          
          
          // simple standard case: create a new heap allocated IterSource implementation.
          // IterExplorer will take ownership (by smart-ptr) and build a Lumiera Iterator front-End
          CHECK ("7-6-5-4-3-2-1" == materialise (
                                      explore (new VerySpecificIter{7})));
          
          
          // missing source detected
          PrivateSource* niente = nullptr;
          CHECK (isnil (explore (niente)));
          
          
          // attach to an IterSource living here in local scope...
          VerySpecificIter vsit{5};
          
          // ...and build a child expansion on top, which calls through the PrivateSource-API
          // Effectively this means we do not know the concrete type of the "expanded children" iterator,
          // only that it adheres to the same IterSource sub-interface as used on the base iterator.
          auto ii = explore(vsit)
                      .expand ([](PrivateSource& source){ return source.expandChildren(); });
          
          CHECK (not isnil (ii));
          CHECK (5 == *ii);
          CHECK (5 == vsit.currentVal());
          ++ii;
          CHECK (4 == *ii);
          CHECK (4 == vsit.currentVal());
          
          CHECK (0 == ii.depth());
          ii.expandChildren();             // note: calls through source's VTable to invoke VerySpecificIter::expandChildren()
          CHECK (1 == ii.depth());
          
          CHECK (2 == *ii);
          ++ii;
          CHECK (1 == *ii);
          
          CHECK (4 == vsit.currentVal());  // note: as long as expanded children are alive, the source pipeline is not pulled further
          CHECK (1 == ii.depth());
          ++ii;
          CHECK (0 == ii.depth());         //   ... but now the children were exhausted and thus also the source advanced
          CHECK (3 == *ii);
          CHECK (3 == vsit.currentVal());
          ++ii;
          CHECK (2 == *ii);
          CHECK (2 == vsit.currentVal());
          ++ii;
          CHECK (1 == *ii);
          CHECK (1 == vsit.currentVal());
          ++ii;
          CHECK (isnil (ii));
        }
      
      
      
      /** @test use a preconfigured exploration scheme to expand depth-first until exhaustion.
       * This is a simple extension where all elements are expanded automatically. In fact, the
       * `expandChildren()` operation implies already an iteration step, namely to dispose of the
       * parent element before injecting the expanded child elements. Based on that observation,
       * when we just replace the regular iteration step by a call to `expandChildren()`, we'll
       * encounter first the parent element and then delve depth-first into exploring the children.
       * @note such continued expansion leads to infinite iteration, unless the _expand functor_
       *       contains some kind of termination condition.
       *       - in the first example, we spawn a child sequence with starting point one below
       *         the current element's value. And since such a sequence is defined to terminate
       *         when reaching zero, we'll end up spawning an empty sequence at leaf nodes, which
       *         prompts the evaluation mechanism to pop back to the last preceding expansion.
       *       - the second example demonstrates how to use value tuples for the intermediary
       *         computation. In this case, we only generate a linear chain of children,
       *         thereby summing up all encountered values. Termination is checked
       *         explicitly in this case, returning an empty child iterator.
       */
      void
      verify_depthFirstExploration()
        {
          CHECK (materialise(
                    explore(CountDown{4})
                      .expand([](uint j){ return CountDown{j-1}; })
                      .expandAll()
                      .transform([](int i){ return i*10; })
                    )
                 == "40-30-20-10-10-20-10-10-30-20-10-10-20-10-10");
          
          
          using std::get;
          using Tu2 = std::tuple<uint, uint>;
          auto summingExpander = [](Tu2 const& tup)
                                  {
                                    uint val = get<0>(tup);
                                    uint sum = get<1>(tup);
                                    return val? singleValIterator (Tu2{val-1, sum+val})
                                              : SingleValIter<Tu2>();
                                  };
          
          CHECK (materialise(
                    explore(CountDown{4})
                      .transform([](uint i){ return Tu2{i,0}; })
                      .expand(summingExpander)
                      .expandAll()
                      .transform([](Tu2 res){ return get<1>(res); })
                    )
                 == "0-4-7-9-10-0-3-5-6-0-2-3-0-1");
        }
      
      
      
      /** @test Demonstration how to build complex algorithms by layered tree expanding iteration
       * @remarks this is the actual use case which inspired the design of IterExplorer:
       *  Search with backtracking over an opaque (abstracted), tree-shaped search space.
       *  - the first point to note is that the search algorithm knows nothing about its
       *    data source, beyond its ability to delve down (expand) into child nodes
       *  - in fact our data source for this test here is "infinite", since it is an
       *    very large random root sequence, where each individual number can be expanded
       *    into a limited random sub sequence, down to arbitrary depth. We just assume
       *    that the search has good chances to find its target sequence eventually and
       *    thus only ever visits a small fraction of the endless search space.
       *  - on top of this (opaque) tree navigation we build a secondary search pipeline
       *    based on a state tuple, which holds onto the underlying data source
       *  - the actual decision logic to guide the search lives within the filter predicate
       *    to pull for the first acceptable solution, i.e. a path down from root where
       *    each node matches the next element from the search string. It is from here
       *    that the `expandChildren()` function is actually triggered, whenever we've
       *    found a valid match on the current level. The (random) data source was chosen
       *    such as to make it very likely to find a match eventually, but also to produce
       *    some partial matches followed by backtracking
       *  - note how the "downstream" processing accesses the `depth()` information exposed
       *    on the opaque data source to react on navigation into nested scopes: here, we use
       *    this feature to create a protocol of the search to indicate the actual "winning path"
       */
      void
      demonstrate_LayeredEvaluation()
        {
          // Layer-1: the search space with "hidden" implementation
          using DataSrc = IterExploreSource<char>;
          DataSrc searchSpace = explore(RandomSeq{-1})
                                  .expand([](char){ return RandomSeq{15}; })
                                  .asIterSource();
          
          // Layer-2: State for search algorithm
          struct State
            {
              DataSrc& src;
              string& toFind;
              vector<uint> protocol;
              
              State(DataSrc& s, string& t)
                : src{s}
                , toFind{t}
                , protocol{0}
                { }
              
              bool
              checkPoint()  const
                {
                  return bool{src};
                }
              
              State&
              yield()  const
                {
                  return *unConst(this);
                }
              
              void
              iterNext()
                {
                  ++src;
                  protocol.resize (1+src.depth());
                  ++protocol.back();
                }
              
              void
              expandChildren()
                {
                  src.expandChildren();
                  protocol.resize (1+src.depth());
                }
              
              bool
              isMatch()  const
                {
                  ASSERT (src.depth() < toFind.size());
                  return *src == toFind[src.depth()];
                }
            };
          
          
          // Layer-3: Evaluation pipeline to drive search
          string toFind = util::join (explore (RandomSeq{5}), "");
          cout << "Search in random tree: toFind = "<<toFind<<endl;
          
          auto theSearch = explore (State{searchSpace, toFind})
                             .filter([](auto& it)
                                       {
                                         while (it->src.depth() < it->toFind.size() - 1
                                                and it->isMatch())
                                           it->expandChildren();
                                         
                                         return it->isMatch();
                                       });
          
          
          // perform the search over a random tree...
          CHECK (not isnil(theSearch));
          cout << "Protocol of the search: " << materialise(theSearch->protocol) <<endl;
        }
    };
  
  
  
  LAUNCHER (IterExplorer_test, "unit common");
  
  
}} // namespace lib::test

