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
namespace test{
  
  using ::Test;
  using util::join;
  using util::isnil;
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
          
///////////////////////////////////////////////////TODO WIP
          using Searcher = decltype(search);
          SHOW_TYPE (Searcher);
          SHOW_TYPE (Searcher::DebugPipeline);
          SHOW_TYPE (Searcher::value_type);
          SHOW_TYPE (Searcher::pointer);
          
//        TypeDebugger<Searcher::reference> guggi;   // --> std::__cxx11::basic_string<char> const&
///////////////////////////////////////////////////TODO WIP
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
      
      
      
      /** @test verify the
       */
      void
      chainedIteration ()
        {
        }
      
      
      
      /** @test verify the
       */
      void
      backtracking ()
        {
        }
    };
  
  LAUNCHER (IterChainSearch_test, "unit common");
  
  
}} // namespace lib::test

