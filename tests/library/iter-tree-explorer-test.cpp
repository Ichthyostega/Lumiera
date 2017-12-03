/*
  IterTreeExplorer(Test)  -  verify tree expanding and backtracking iterator

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-tree-explorer-test.cpp
 ** The \ref IterTreeExplorer_test covers and demonstrates a generic mechanism
 ** to expand and evaluate tree like structures. In its current shape (as of 2017),
 ** it can be seen as an preliminary step towards retrofitting IterExplorer into
 ** a framework of building blocks for tree expanding and backtracking evaluations.
 ** Due to the nature of Lumiera's design, we repeatedly encounter this kind of
 ** algorithms, when it comes to matching configuration and parametrisation against
 ** a likewise hierarchical and rules based model. To keep the code base maintainable,
 ** we deem it crucial to reduce the inherent complexity in such algorithms by clearly
 ** separate the _mechanics of evaluation_ from the actual logic of the target domain.
 ** 
 ** Similar to IterExplorer_test, the his test relies on a demonstration setup featuring
 ** a custom encapsulated state type: we rely on a counter with start and end value,
 ** embedded into an iterator. Basically, this running counter, when iterated, generates
 ** a descending sequence of numbers start ... end.
 ** So -- conceptually -- this counting iterator can be thought to represent this
 ** sequence of numbers. Note that this is a kind of abstract or conceptual
 ** representation, not a factual representation of the sequence in memory.
 ** The whole point is _not to represent_ this sequence in runtime state at once,
 ** rather to pull and expand it on demand.
 ** 
 ** All these tests work by first defining these _functional structures_, which just
 ** yields an iterator entity. We get the whole structure it conceptually defines
 ** only if we "pull" this iterator until exhaustion -- which is precisely what
 ** the test does to verify proper operation. Real world code of course would
 ** just not proceed in this way, like pulling everything from such an iterator.
 ** Often, the very reason we're using such a setup is the ability to represent
 ** infinite structures. Like e.g. the evaluation graph of video passed through
 ** a complex processing pipeline.
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include "lib/iter-tree-explorer.hpp"
#include "lib/meta/trait.hpp"

#include <utility>
#include <vector>
#include <string>


namespace lib {
namespace test{
  
  using ::Test;
  using util::_Fmt;
  using util::isnil;
  using util::isSameObject;
  using lib::iter_stl::eachElm;
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
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
        
        friend bool
        checkPoint (CountDown const& st)
        {
          return st.p > st.e;
        }
        
        friend uint&
        yield (CountDown const& st)
        {
          return util::unConst(checkPoint(st)? st.p : st.e);
        }
        
        friend void
        iterNext (CountDown & st)
        {
          if (not checkPoint(st)) return;
          --st.p;
        }
      };
    
    
    
    /**
     * A straight descending number sequence as basic test iterator.
     * It is built wrapping an opaque "state core" (of type CountDown).
     * @note the "state core" is not accessible from the outside
     */
    class NumberSequence
      : public IterStateWrapper<uint, CountDown>
      {
        
      public:
        explicit
        NumberSequence(uint start = 0)
          : IterStateWrapper<uint,CountDown> (CountDown{start})
          { }
        NumberSequence(uint start, uint end)
          : IterStateWrapper<uint,CountDown> (CountDown(start,end))
          { }
      };
    
    
    
    /** Diagnostic helper: join all the elements from a _copy_ of the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {
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
   *       to build various functional evaluation structures,
   *       based on the \ref IterExplorer template.
   *       - the [state adapter](\ref verifyStateAdapter() )
   *          iterator construction pattern
   *       - helper to [chain iterators](\ref verifyChainedIterators() )
   *       - building [tree exploring structures](\ref verifyDepthFirstExploration())
   *       - the [monadic nature](\ref verifyMonadOperator()) of IterExplorer
   *       - a [recursively self-integrating](\ref verifyRecrusiveSelfIntegration())
   *         evaluation pattern
   * 
   * ## Explanation
   * 
   * Both this test and the IterExplorer template might be bewildering
   * and cryptic, unless you know the *Monad design pattern*. »Monads«
   * are heavily used in functional programming, actually they originate
   * from Category Theory. Basically, Monad is a pattern where we combine
   * several computation steps in a specific way; but instead of intermingling
   * the individual computation steps and their combination, the goal is to
   * isolate and separate the _mechanics of combination_, so we can focus on
   * the actual _computation steps:_ The mechanics of combination are embedded
   * into the Monad type, which acts as a kind of container, holding some entities
   * to be processed. The actual processing steps are then attached to the monad as
   * "function object" parameters. It is up to the monad to decide if, and when
   * those processing steps are applied to the embedded values and how to combine
   * the results into a new monad.
   * 
   * Using the monad pattern is well suited when both the mechanics of
   * combination and the individual computation steps tend to be complex.
   * In such a situation, it is beneficial to develop and test both
   * in isolation. The IterExplorer template applies this pattern
   * to the task of processing a source sequence. Typically we use
   * this in situations where we can't afford building elaborate
   * data structures in (global) memory, but rather strive at
   * doing everything on-the-fly. A typical example is the
   * processing of a variably sized data set without
   * using heap memory for intermediary results.
   * 
   * @see TreeExplorer
   * @see IterAdapter
   */
  class IterTreeExplorer_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_wrappedState();
          verify_wrappedIterator();
          
          verify_expandOperation();
          verify_transformOperation();
          verify_combinedExpandTransform();
          verify_FilterIterator();
          verify_asIterSource();
          
          verify_depthFirstExploration();
          demonstrate_LayeredEvaluation();
        }
      
      
      
      /** @test without using any extra functionality,
       *        TreeExplorer just wraps an iterable state.
       */
      void
      verify_wrappedState()
        {
          auto ii = treeExplore (CountDown{5,0});
          CHECK (!isnil (ii));
          CHECK (5 == *ii);
          ++ii;
          CHECK (4 == *ii);
          pullOut(ii);
          CHECK ( isnil (ii));
          CHECK (!ii);
          
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
          
          ii = treeExplore (CountDown{5});
          CHECK (materialise(ii) == "5-4-3-2-1");
          ii = treeExplore (CountDown{7,4});
          CHECK (materialise(ii) == "7-6-5");
          ii = treeExplore (CountDown{});
          CHECK ( isnil (ii));
          CHECK (!ii);
        }
      
      
      /** @test TreeExplorer is able to wrap any _Lumiera Forward Iterator_ */
      void
      verify_wrappedIterator()
        {
          vector<int> numz{1,-2,3,-5,8,-13};
          auto ii = eachElm(numz);
          CHECK (!isnil (ii));
          CHECK (1 == *ii);
          ++ii;
          CHECK (-2 == *ii);
          
          auto jj = treeExplore(ii);
          CHECK (!isnil (jj));
          CHECK (-2 == *jj);
          ++jj;
          CHECK (3 == *jj);
          
          // we passed a LValue-Ref, thus a copy was made
          CHECK (-2 == *ii);

          CHECK (materialise(ii) == "-2-3--5-8--13");
          CHECK (materialise(jj) ==    "3--5-8--13");
          
          // can even adapt STL container automatically
          auto kk = treeExplore(numz);
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
       *   when building the TreeExplorer. In this case, the functor typically takes a _reference_
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
                    treeExplore(CountDown{5})
                      .expand([](uint j){ return CountDown{j-1}; })                                   // expand-functor: Val > StateCore
                      );
          
          verify_treeExpandingIterator(
                    treeExplore(CountDown{5})
                      .expand([](uint j){ return NumberSequence{j-1}; })                              // expand-functor: Val > Iter
                      );
          
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
                    treeExplore(CountDown{5})
                      .expand(expandIntoChildBuffer)                                                  // expand-functor: Val > STL-container&
                      );
          
          // test routine called the expansion functor five times
          CHECK (5 == childBuffer.size());
          
          
          
          /* == "state manipulation" use cases == */
          
          verify_treeExpandingIterator(
                    treeExplore(CountDown{5})
                      .expand([](CountDown const& core){ return CountDown{ yield(core) - 1}; })       // expand-functor: StateCore const& -> StateCore
                      );
          
          verify_treeExpandingIterator(
                    treeExplore(CountDown{5})
                      .expand([](CountDown core){ return NumberSequence{ yield(core) - 1}; })         // expand-functor: StateCore -> Iter
                      );
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1118 : GDB Segfault on loading the inferior
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1118 : Generated code works just fine and passes Test though
          verify_treeExpandingIterator(
                    treeExplore(CountDown{5})
                      .expand([](auto & it){ return CountDown{ *it - 1}; })                           // generic Lambda: Iter& -> StateCore
                      );
          
          verify_treeExpandingIterator(
                    treeExplore(CountDown{5})
                      .expand([](auto it){ return decltype(it){ *it - 1}; })                          // generic Lambda: Iter -> Iter
                      );
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1118 : GDB Segfault on loading the inferior
        }
      
      
      template<class EXP>
      void
      verify_treeExpandingIterator(EXP ii)
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
          
          
          auto ii = treeExplore (CountDown{7,4})
                      .transform(multiply)
                      ;
          
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
          
          CHECK ("≺1≻-≺-2≻-≺3≻-≺-5≻-≺8≻-≺-13≻"                == materialise (treeExplore(numz)
                                                                                .transform(formatify)) );
          
          CHECK ("≺2≻-≺-4≻-≺6≻-≺-10≻-≺16≻-≺-26≻"              == materialise (treeExplore(numz)
                                                                                .transform(multiply)
                                                                                .transform(formatify)) );
          
          CHECK ("≺≺4≻≻-≺≺-8≻≻-≺≺12≻≻-≺≺-20≻≻-≺≺32≻≻-≺≺-52≻≻" == materialise (treeExplore(numz)
                                                                                .transform(multiply)
                                                                                .transform(multiply)
                                                                                .transform(formatify)
                                                                                .transform(formatify)) );
          
          
          // demonstrate the functor is evaluated only once per step
          int fact = 3;
          
          auto jj = treeExplore (CountDown{4})
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
          auto kk = treeExplore(CountDown{9,4})
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
      
      
      /** @test combie the recursion into children with a tail mapping operation
       */
      void
      verify_combinedExpandTransform()
        {
          UNIMPLEMENTED("combine child expansion and result mapping");
        }
      
      
      /** @test add a filtering predicate into the pipeline
       */
      void
      verify_FilterIterator()
        {
          UNIMPLEMENTED("preconfigured repeated depth-first expansion");
        }
      
      
      /** @test package the resulting Iterator as automatically managed,
       *        polymorphic opaque implementing the IterSource interface.
       */
      void
      verify_asIterSource()
        {
          UNIMPLEMENTED("preconfigured repeated depth-first expansion");
        }
      
      
      /** @test use a preconfigured exploration scheme to expand depth-first until exhaustion
       */
      void
      verify_depthFirstExploration()
        {
          UNIMPLEMENTED("preconfigured repeated depth-first expansion");
        }
      
      
      /** @test Demonstration how to build complex algorithms by layered tree expanding iteration
       * @remarks this is the actual use case which inspired the design of TreeExplorer
       */
      void
      demonstrate_LayeredEvaluation()
        {
          UNIMPLEMENTED("build algorithm by layering iterator evaluation");
        }
    };
  
  
  
  LAUNCHER (IterTreeExplorer_test, "unit common");
  
  
}} // namespace lib::test

