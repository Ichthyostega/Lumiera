/*
  IterAdapterSTL(Test)  -  building various custom iterators for a given container
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
#include "lib/test/test-coll.hpp"
#include "lib/util.hpp"
//#include "lib/util-foreach.hpp"

#include "lib/iter-adapter-stl.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
//  using util::for_each;
//  using util::isnil;
//  using std::vector;
  using std::cout;
  using std::endl;
  
  namespace iter = lib::iter_stl;
  
  
  namespace { // provide STL containers with test data
  
    uint NUM_ELMS = 10;
    
  } // (END) test data
  
  
  /** test an iterator: create it by calling a constructor function
   *  and then pull out all contents and print them to STDOUT  */
#define TEST_ITER(_CTOR_, _ARG_) \
      cout << STRINGIFY(_CTOR_) ;\
      pullOut (_CTOR_ _ARG_)    ;\
      cout << endl;
  
  /** print descriptive separator to STDOUT */
#define PRINT_FUNC(_F_NAME_, _F_TYPE_) \
      cout << "-----"<<STRINGIFY(_F_NAME_)<<"---" << showType<_F_TYPE_>() << endl;
  
  
  
  
  
  
  
  
  /************************************************************************
   * @test provide test STL containers to verify some of the adapters
   *       to expose typical container usage patterns as lumiera iterators.
   *       - keys and values of a map
   *       - multimap values associated with a given key
   *
   * @see RangeIter
   * @see iter-adapter.hpp
   * @see iter-adapter-stl.hpp
   */
  class IterAdapterSTL_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          iterateMapKeyVal (getTestMap_int<MapII> (NUM_ELMS));
        }
      
      
      template<class MAP>
      void
      iterateMapKeyVal(MAP const& map)
        { 
          PRINT_FUNC (iterateMapKeyVal, MAP);
          
          TEST_ITER (iter::eachKey, (map));
          TEST_ITER (iter::eachVal, (map));
          TEST_ITER (iter::eachDistinctKey, (map));
        }
          
      
    };
  
  LAUNCHER (IterAdapterSTL_test, "unit common");
  
  
}} // namespace lib::test

