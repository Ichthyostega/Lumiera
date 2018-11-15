/*
  IterableClassification(Test)  -  detecting iterability of a generic type

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iterable-classification-test.cpp
 ** unit test \ref IterableClassification_test
 */


#include "lib/test/run.hpp"

#include "steam/mobject/session/scope-query.hpp"
#include "steam/mobject/session/effect.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util-foreach.hpp"
#include "lib/itertools.hpp"

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>


namespace lib  {
namespace meta {
namespace test {
  
  using proc::mobject::session::Effect;
  using proc::mobject::session::ScopeQuery;
  typedef lib::time::TimeVar Time;
  
  using std::cout;
  using std::endl;
  
  
  namespace { // a custom test container....
    
    
    struct TestSource
      {
        vector<int> data_;
        
        TestSource(uint num);
        
        typedef vector<int>::iterator sourceIter;
        typedef RangeIter<sourceIter> iterator;
        
        iterator begin() ;
        iterator end()   ;
                        // note: a bare type definition is sufficient here....
      };
    
    
    
  }//(End) test containers
  
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
  
  
  
  /*******************************************************************************//**
   * @test verify the (static) classification/detection of iterables.
   *       Currently (1/10) we're able to detect the following
   *       - a STL like container with \c begin() and \c end()
   *       - a Lumiera Forward Iterator
   * This test just retrieves the results of a compile time execution
   * of the type detection; thus we just define types and then access
   * the generated meta function value.
   */
  class IterableClassification_test : public Test
    {
      
      void
      run (Arg) 
        {
          // define a bunch of STL containers
          typedef std::vector<long>   LongVector;
          typedef std::multiset<Time> TimeSet;
          typedef std::map<int,char>  CharMap;
          typedef std::list<bool>     BoolList;
          typedef std::deque<ushort>  ShortDeque;
          typedef TestSource          CustomCont;
          
          // some types in compliance to the "Lumiera Forward Iterator" concept
          typedef TestSource::iterator                  ForwardRangeIter;
          typedef TransformIter<ForwardRangeIter, long> TransformedForwardIter;
          typedef FilterIter<TransformedForwardIter>    FilteredForwardIter;
          typedef ScopeQuery<Effect>::iterator          CustomForwardIter;
          
          
          // detect STL iteration
          SHOW_CHECK( can_STL_ForEach<LongVector> );
          SHOW_CHECK( can_STL_ForEach<TimeSet>    );
          SHOW_CHECK( can_STL_ForEach<CharMap>    );
          SHOW_CHECK( can_STL_ForEach<BoolList>   );
          SHOW_CHECK( can_STL_ForEach<ShortDeque> );
          SHOW_CHECK( can_STL_ForEach<CustomCont> );
          
          SHOW_CHECK( can_STL_ForEach<ForwardRangeIter> );
          SHOW_CHECK( can_STL_ForEach<TransformedForwardIter> );
          SHOW_CHECK( can_STL_ForEach<FilteredForwardIter> );
          SHOW_CHECK( can_STL_ForEach<CustomForwardIter> );
          
          // detect Lumiera Forward Iterator
          SHOW_CHECK( can_IterForEach<LongVector> );
          SHOW_CHECK( can_IterForEach<TimeSet>    );
          SHOW_CHECK( can_IterForEach<CharMap>    );
          SHOW_CHECK( can_IterForEach<BoolList>   );
          SHOW_CHECK( can_IterForEach<ShortDeque> );
          SHOW_CHECK( can_IterForEach<CustomCont> );
          
          SHOW_CHECK( can_IterForEach<ForwardRangeIter> );
          SHOW_CHECK( can_IterForEach<TransformedForwardIter> );
          SHOW_CHECK( can_IterForEach<FilteredForwardIter> );
          SHOW_CHECK( can_IterForEach<CustomForwardIter> );
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (IterableClassification_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
