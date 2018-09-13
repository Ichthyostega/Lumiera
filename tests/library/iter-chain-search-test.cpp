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
#include "lib/format-util.hpp"  /////////////////////////////TODO necessary?
#include "lib/format-cout.hpp"  /////////////////////////////TODO necessary?
#include "lib/iter-chain-search.hpp"
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
  
  using lumiera::error::LERR_(ITER_EXHAUST);
  
  
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
    
  } // (END)fixture
  
  
  
///////////////////////////////////////////////////TODO WIP
#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;

    /** Diagnostic helper: join all the elements from a _copy_ of the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {
      return util::join (std::forward<II> (ii), "-");
    }
///////////////////////////////////////////////////TODO WIP
  
  
  
  
  /*****************************************************************//**
   * @test verify a setup for consecutive searches with backtracking.
   * 
   * @see iter-chain-search.hpp
   * @see iter-cursor.hpp
   * @see [usage example](event-log.hpp)
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
          auto search = chainSearch(SPAM)
                          .search([](string const& str){ return startsWith (str, "s"); });
///////////////////////////////////////////////////TODO WIP
          cout << materialise (search) <<endl;
///////////////////////////////////////////////////TODO WIP
          CHECK (search);
          
          search.addStep([](auto filter)
                            {
                              string currVal = *filter;
                              filter.setNewFilter ([=](string const& val){
                                  return val != currVal; });
                              return filter;
                            });
///////////////////////////////////////////////////TODO WIP
          cout << materialise (search) <<endl;
///////////////////////////////////////////////////TODO WIP
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
        }
    };
  
  LAUNCHER (IterChainSearch_test, "unit common");
  
  
}}} // namespace lib::iter::test
