/*
  IterChainSearch(Test)  -  verify chained search operations with backtracking

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

* *****************************************************/

/** @file iter-chain-search-test.cpp
 ** unit test \ref IterChainSearch_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-util.hpp"
#include "lib/iter-chain-search.hpp"
#include "lib/iter-cursor.hpp"
#include "lib/util.hpp"

#include <vector>
#include <string>



namespace lib {
namespace iter{
namespace test{
  
  using ::Test;
  using util::join;
  using util::isnil;
  using util::startsWith;
  using util::isSameObject;
  using std::vector;
  using std::string;
  
  using LERR_(ITER_EXHAUST);
  
  
  namespace { // test fixture
    
    using Spam = vector<string>;
    
    const Spam SPAM{"spam"
                   ,"sausage"
                   ,"spam"
                   ,"spam"
                   ,"bacon"
                   ,"spam"
                   ,"tomato"
                   ,"and"
                   ,"spam"
                   };
    
    
    /** Diagnostic helper: join all the elements from a _copy_ of the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {
      return util::join (std::forward<II> (ii), "-");
    }
  }// (END)fixture
  
  
  
  
  
  
  
  /*****************************************************************//**
   * @test verify a setup for consecutive searches with backtracking.
   *       - demonstrate simple consecutive matches and iterator behaviour
   *       - clear the filter underway and thus return to simple iteration
   *       - set up two conditions, the second one capturing state at the
   *         point where the first one matches
   *       - wrap a lib::IterCursor, which allows to toggle the search
   *         direction underway; this creates a situation where the
   *         first options picked do not lead to a successful solution,
   *         so the search has to track back, try further options and
   *         in each case re-apply all the following consecutive
   *         search conditions.
   * 
   * @see iter-chain-search.hpp
   * @see iter-cursor.hpp
   * @see [usage example](\ref event-log.hpp)
   */
  class IterChainSearch_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleSearch();
          chainedIteration();
          backtracking();
        }
      
      
      /** @test simple basic use case. */
      void
      simpleSearch ()
        {
          auto search = chainSearch(SPAM)
                          .search("bacon")
                          .search("tomato");
          
          CHECK (search);
          CHECK (not isnil(search));
          CHECK ("tomato" == *search);
          CHECK (isSameObject (*search, SPAM[6]));
          
          search.clearFilter();
          CHECK ("tomato" == *search);
          ++search;
          CHECK ("and" == *search);
          search.search("spam");
          CHECK ("spam" == *search);
          CHECK (isSameObject (*search, SPAM[8]));
          
          ++search;
          CHECK (not search);
          CHECK (isnil (search));
          VERIFY_ERROR (ITER_EXHAUST, *search);
        }
      
      
      
      /** @test verify consecutive application of several functors on the underlying filter.
       * In the general case, each step in the chain is a function working on a copy of the
       * current filter state. Since each such step configures its own copy of the complete
       * pipeline, it may reconfigure this filter pipeline in arbitrary ways. After exhausting
       * the last layer, the evaluation returns to the previous layer, but immediately re-applies
       * the configuration step on the then next element.
       */
      void
      chainedIteration ()
        {
          auto search = chainSearch(SPAM)                                  // Note: 1st filter step picks all s-words
                          .search([](string const& str){ return startsWith (str, "s"); });
          
          CHECK (materialise (search) =="spam-sausage-spam-spam-spam-spam");
          CHECK ("spam" == *search);
          
          search.addStep([](auto& filter)
                            {                                              // Note: pick the current value at the point
                              string currVal = *filter;                    //       where the 2nd filter step is (re)applied
                              filter.setNewFilter ([=](string const& val)  //       ...and bake this value into the lambda closure
                                                    {
                                                      return val != currVal;
                                                    });
                            });
          
          CHECK ("sausage" == *search);
          CHECK (materialise (search)
                 == "sausage-bacon-tomato-and-"               // everything in the rest, which is not "spam"
                    "spam-spam-bacon-spam-tomato-and-spam-"   // everything starting at "sausage" which is not "sausage"
                    "bacon-tomato-and-"                       // any non-spam behind the 2nd spam
                    "bacon-tomato-and-"                       // any non-spam behind the 3rd spam
                    "tomato-and"                              // any non-spam behind the 4th spam
                    "");                                      // and any non-spam behind the final spam
        }
      
      
      
      /** @test verify a complex search with backtracking.
       * This becomes relevant when a given search condition can be "too greedy" for the
       * complete chain to succeed. Most notably this is the case when the search is fundamentally
       * reconfigured in some steps, e.g. by switching the search orientation. To demonstrate this,
       * we use a "gear switching" iterator, which allows us to reverse the direction and to search
       * backwards from the current position. We configure the second condition in the chain such
       * that it can not succeed when starting from the first match on the first condition
       */
      void
      backtracking ()
        {
          using Cursor = IterCursor<decltype(SPAM.begin())>;
          
          auto search = chainSearch(Cursor{SPAM.begin(), SPAM.end()})
                          .search("spam")
                          .addStep([](auto& filter)
                                      {
                                        filter.switchBackwards(); // switch search direction without moving the cursor
                                        filter.flipFilter();      // flip from match on "spam" to match on non-spam
                                      })
                          .addStep([](auto& filter)
                                      {                           // filter is still configured to search non-spam backwards
                                        ++filter;                 // just "advance" this filter by one step (backward)
                                      });
          
          CHECK (materialise (search)        // Layer-0: 1-3 spam fail altogether, too greedy. Try 4rd spam....
                                             // Layer-1: turn back, toggle to non-spam, find bacon
                 == "sausage-"               // Layer-2: non-spam and one step further backwards yields sausage
                                             //
                                             // BACKTRACK to Layer-0: pick 5th (and last) spam...
                                             // Layer-1: turn back, toggle to non-spam, find "and"
                    "tomato-bacon-sausage-"  // Layer-2: non-spam and one step back yields tomato, next bacon, next sausage.
                                             // BACKTRACK to Layer-1: take previous one, which is tomato
                    "bacon-sausage-"         // Layer-2: non-spam and one step further back yields bacon, then next sausage.
                                             // BACKTRACK to Layer-1: take previous non-spam, which is bacon
                    "sausage"                // Layer-2: non-spam and one step further back yields sausage.
                    "");                     // BACKTRACK to Layer-1: exhausted, BACKTRACK to Layer-0: exhausted. Done.
        }
    };
  
  LAUNCHER (IterChainSearch_test, "unit common");
  
  
}}} // namespace lib::iter::test
