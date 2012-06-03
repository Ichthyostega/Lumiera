/*
  IterExplorer(Test)  -  verify evaluation patterns built using iterators

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"

#include "lib/iter-explorer.hpp"
#include "lib/linked-elements.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>



namespace lib {
namespace test{
  
  using ::Test;
//  using boost::lexical_cast;
//  using util::for_each;
//  using util::isnil;
  using util::isnil;
  using util::isSameObject;
//  using std::vector;
  using std::cout;
  using std::endl;
  using std::string;
  using lib::LinkedElements;
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  
  namespace { // test dummy source iterator
  
//    uint NUM_ELMS = 10;
    
    /**
     * This iteration state type describes
     * a sequence of numbers yet to be delivered.
     */
    class State
      {
        uint p,e;
        
      public:
        State(uint start, uint end)
          : p(start)
          , e(end)
          { }
        
        friend bool
        checkPoint (State const& st)
        {
          return st.p < st.e;
        }
        
        friend uint&
        yield (State const& st)
        {
          return util::unConst(checkPoint(st)? st.p : st.e);
        }
        
        friend void
        iterNext (State & st)
        {
          if (!checkPoint(st)) return;
          ++st.p;
        }
      };
    
    
    
    /** 
     * A straight number sequence as basic test iterator.
     * The tests will dress up this source sequence in various ways.
     */
    class NumberSequence
      : public IterStateWrapper<uint, State>
      {
          
        public:
          explicit
          NumberSequence(uint end = 0)
            : IterStateWrapper<uint,State> (State(0,end))
            { }
          NumberSequence(uint start, uint end)
            : IterStateWrapper<uint,State> (State(start,end))
            { }
      };
    
    inline NumberSequence
    seq (uint end)
    {
      return NumberSequence(end); 
    }
    
    inline NumberSequence
    seq (uint start, uint end)
    {
      return NumberSequence(start, end); 
    }
    
    NumberSequence NIL_Sequence;
    
    
    
    /** Diagnostic helper: "squeeze out" the given iterator
     * and join all the elements yielded into a string
     */
    template<class II>
    inline string
    materialise (II ii)
    {
      std::ostringstream buff;
      while (ii)
        {
          buff << *ii;
          if (++ii) buff << "-";
        }
      return buff.str();
    }
    
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
  
  
  
  
  
  
  
  /*********************************************************************
   * @test use a simple source iterator yielding numbers
   *       to build various functional evaluation structures,
   *       based on the IterExplorer template.
   * 
   * \par Explanation
   * Both this test and the IterExplorer template might be bewildering
   * and cryptic, unless you know the Monad design pattern. Monads are
   * heavily used in functional programming, actually they originate
   * from Category Theory. Basically, Monad is a pattern where we
   * combine several computation steps in a specific way; but instead
   * of intermingling the individual computation steps and their
   * combination, the goal is to separate and isolate the mechanics
   * of combination, so we can focus on the actual computation steps:
   * The mechanics of combination are embedded into the Monad type,
   * which acts as a kind of container, holding elements to be processed.
   * The actual processing steps are then fed to the monad as parameters.
   * 
   * Using the monad pattern is well suited when both the mechanics of
   * combination and the individual computation steps tend to be complex.
   * In such a situation, it is beneficial to develop and test both
   * in isolation. The IterExplorer template applies this pattern
   * to the task of processing a source sequence. Typically we use
   * this in situations where we can't effort building elaborate
   * data structures in (global) memory, but rather strive at
   * doing everything on-the-fly. A typical example is the
   * processing of a variably sized data set without
   * using heap memory for intermediary results.
   * 
   * @see IterExplorer
   * @see IterAdapter
   */
  class IterExplorer_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verifyStateAdapter();
          
          verifyMonadOperator ();
          UNIMPLEMENTED ("IterExplorer Monad");
          verifyChainedIterators();
          verifyRawChainedIterators();
          
          verifyDepthFirstExploration();
          verifyBreadthFirstExploration();
        }
      
      
      
      /** @test all of the following IterExplorer flavours are built on top
       * of a special iterator adapter, centred at the notion of an iterable
       * state element type. The actual iterator just embodies one element
       * of this state representation, and typically there is not an hidden
       * back-link to some kind of container in charge of the elements yielded
       */
      void
      verifyStateAdapter ()
        {
          NumberSequence ii = seq(9);
          CHECK (!isnil (ii));
          CHECK (0 == *ii);
          ++ii;
          CHECK (1 == *ii);
          pullOut(ii);
          CHECK ( isnil (ii));
          CHECK (!ii);
          
          VERIFY_ERROR (ITER_EXHAUST, *ii );
          VERIFY_ERROR (ITER_EXHAUST, ++ii );
          
          ii = seq(5);
          CHECK (materialise(ii) == "0-1-2-3-4");
          ii = seq(5,8);
          CHECK (materialise(ii) == "5-6-7");
          
          ii = NIL_Sequence;
          CHECK ( isnil (ii));
          CHECK (!ii);
        }
      
      
      
      /** @test a simple and practical helper built on top of IterExplorer.
       * The resulting iterator \em combines and \em flattens a sequence
       * of source iterators, resulting in a simple sequence accessible
       * again as iterator. Here we verify the convenience / default
       * implementation; it uses a STL container behind the scenes
       * to keep track of all the added source iterators
       */
      void
      verifyChainedIterators ()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
          typedef ChainedIters<NumberSequence> Chain;
          
          Chain ci = iter::chain (seq(5),seq(7),seq(9));
          
          CHECK (!isnil (ci));
          pullOut (ci);
          CHECK ( isnil (ci));
          VERIFY_ERROR (ITER_EXHAUST, *ci );
          VERIFY_ERROR (ITER_EXHAUST, ++ci );
          
          CHECK (isnil(Chain()));
          CHECK (iter::chain (NIL_Sequence));
          
          // Iterator chaining "flattens" one level of indirection
          NumberSequence s9 = seq(9); 
          ci = iter::chain (s9);
          
          for ( ; s9 && ci; ++s9, ++ci )
            CHECK (*s9 == *ci);
          
          CHECK (isnil(s9));
          CHECK (isnil(ci));
          
          // Note: Iterator chain is created based on (shallow) copy
          //       of the source sequences. In case these have an independent
          //       per-instance state (like e.g. NumberSequence used for this test),
          //       then the created chain is independent from the source iterators.
          s9 = seq(9);
          ci = iter::chain (s9);
          CHECK (0 == *s9);
          CHECK (0 == *ci);
          
          pullOut (ci);
          CHECK (isnil(ci));
          CHECK (0 == *s9);
          pullOut (s9);
          CHECK (isnil(s9));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
        }
      
      
      /** @test variation of the iterator chaining facility.
       * This is the "raw" version without any convenience shortcuts.
       * The source iterators are given as iterator yielding other iterators. 
       */
      void
      verifyRawChainedIterators ()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
          typedef LinkedElements<NumberSequence, linked_elements::NoOwnership> IterContainer;
          typedef IterContainer::const_iterator IterIter;
          
          typedef ChainedIters<IterIter> Chain;
          
          NumberSequence s5 (1,5);
          NumberSequence s7 (5,8);
          NumberSequence s9 (8,10);
          
          CHECK (1 == *s5);
          CHECK (5 == *s7);
          CHECK (8 == *s9);
          
          IterContainer srcIters;
          srcIters.push (s9);
          srcIters.push (s7);
          srcIters.push (s5);
          
          IterIter iti(srcIters.begin());
          CHECK (!isnil (iti));
          CHECK (isSameObject (s5, *iti));
          
          Chain chain(iti);
          CHECK (!isnil (iti));
          CHECK (1 == *chain);
          
          ++chain;
          CHECK (2 == *chain);
          CHECK (2 == *s5);
          CHECK (5 == *s7);
          CHECK (8 == *s9);
          
          ++++chain;
          CHECK (4 == *chain);
          CHECK (4 == *s5);
          CHECK (5 == *s7);
          CHECK (8 == *s9);
          
          ++chain;
          CHECK (5 == *chain);
          CHECK (isnil(s5));
          CHECK (5 == *s7);
          CHECK (8 == *s9);
          
          ++chain;
          CHECK (6 == *chain);
          CHECK (isnil(s5));
          CHECK (6 == *s7);
          CHECK (8 == *s9);
          
          ++++chain;
          CHECK (8 == *chain);
          CHECK (isnil(s5));
          CHECK (isnil(s7));
          CHECK (8 == *s9);
          
          ++chain;
          CHECK (9 == *chain);
          CHECK (isnil(s5));
          CHECK (isnil(s7));
          CHECK (9 == *s9);
          
          ++chain;
          CHECK (isnil(chain));
          CHECK (isnil(s5));
          CHECK (isnil(s7));
          CHECK (isnil(s9));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
        }
      
      
      
      /** @test a depth-first visiting and exploration scheme
       * of a tree like system, built on top of the IterExplorer monad.
       * 
       * \par Test data structure
       * We build a functional datastructure here, on the fly, while exploring it.
       * The \c exploreChildren(m) function generates this tree like datastructure:
       * For a given node with number value i, it returns the children (2*i-1) and 2*i
       * -- but only up to the given maximum value m.
       * 
       * If we start such a tree structure with a root node 1, and maximum = 8, this yields:
       * \code
       * (       1       )
       * (   1       2   )
       * ( 1   2   3   4 )
       * (1 2 3 4 5 6 7 8)
       * \endcode
       * This tree has no meaning in itself, beyond being an easy testbed for tree exploration schemes.
       * 
       * \par How the exploration works
       * We use a pre defined Template \link DepthFirstExplorer \endlink, which is built on top of IterExplorer.
       * It contains the depth-first exploration strategy in a hardwired fashion. Actually this effect is achieved
       * by defining a specific way how to \em combine the results of an \em exploration -- the latter being the
       * function which generates the data structure. To yield a depth-first exploration, all we have to do
       * is to delve down immediately into the children, right after visiting the node itself.
       * 
       * Now, when creating such a DepthFirstExplorer by wrapping a given source iterator, the result is again
       * an iterator, but a specific iterator which at the same time is a Monad: It supports the \c >>= operation
       * (also known as \em bind operator or \em flatMap operator). This operator takes as second argument a
       * function, which in our case is the function to generate or explore the data structure.
       * 
       * The result of applying this \c >>= operation is a \em transformed version of the source iterator,
       * i.e. it is again an iterator, which yields the results of the exploration function, combined together
       * in the order as defined by the built-in exploration strategy (here: depth first)
       */
      void
      verifyDepthFirstExploration ()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
          typedef DepthFirstExplorer<NumberSequence> DepthFirst;
          DepthFirst root (seq(1));
          
          string explorationResult = materialise(root >>= exploreChildren(8));
          CHECK (explorationResult == "1-1-1-1-2-2-3-4-2-3-5-6-4-7-8");
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
        }
      
      
      
      /** @test a breadth-first visiting and exploration scheme
       * of a tree like system, built on top of the IterExplorer monad;
       * here, an internal queue is used to explore the hierarchy in layers.
       * The (functional) datastructure is the same, just we're visiting it
       * differently here (in rows or layers).
       */
      void
      verifyBreadthFirstExploration ()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
          typedef BreadthFirstExplorer<NumberSequence> BreadthFirst;
          BreadthFirst root (seq(1));
          
          string explorationResult = materialise(root >>= exploreChildren(8));
          CHECK (explorationResult == "1-1-2-1-2-3-4-1-2-3-4-5-6-7-8");
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #892
        }
      
      
      
      /** @test cover the basic monad bind operator,
       * which is used to build all the specialised Iterator flavours.
       * The default implementation ("Combinator strategy") just joins and flattens
       * the result sequences created by the functor bound into the monad. For this test,
       * we use a functor \c explode(top), which returns the sequence 0...top.
       */
      void
      verifyMonadOperator ()
        {
          // IterExplorer as such is an iterator wrapping the source sequence
          string result = materialise (exploreIter(seq(5)));
          CHECK (result == "0-1-2-3-4");
          
          // now, if the source sequence yields exactly one element 5...
          result = materialise (exploreIter(seq(5,6)));
          CHECK (result == "5");
          
          // then binding the explode()-Function yields just the result of invoking explode(5) 
          result = materialise (exploreIter(seq(5,6)) >>= explode);
          CHECK (result == "0-1-2-3-4");
          
          // binding anything into an empty sequence still results in an empty sequence
          result = materialise (exploreIter(seq(0)) >>= explode);
          CHECK (result == "");
          
          // also, n case the bound function yields an empty sequence, the result remains empty
          result = materialise (exploreIter(seq(1)) >>= explode);
          CHECK (result == "");
          
          // combining an empty sequence and the one element sequence (seq(0,1)) results in just one element
          result = materialise (exploreIter(seq(2)) >>= explode);
          CHECK (result == "0");
          
          // multiple result sequences will be joined (flattened) into one sequence
          result = materialise (exploreIter(seq(5)) >>= explode);
          CHECK (result == "0-0-1-0-1-2-0-1-2-3");
          
          // since the result is a monad, we can again bind yet another function
          result = materialise((exploreIter(seq(5)) >>= explode) >>= explode);
          CHECK (result == "0-0-0-1-0-0-1-0-1-2");
          
                        // Explanation:
                        // 0 -> empty sequence, gets dropped
                        // 1 -> 1-element sequence {0}
                        // 2 -> {0,1}
                        // 3 -> {0,1,2}
                        
                        // Note: when cascading multiple >>= the parentheses are necessary, since in C++ unfortunately
                        // the ">>=" associates to the right, while the proper monad bind operator should associate to the left
        }
      
      static NumberSequence
      explode (uint top)
        {
          return seq(0,top);
        }
    };
  
  
  LAUNCHER (IterExplorer_test, "unit common");
  
  
}} // namespace lib::test

