/*
  IterAdapterSTL(Test)  -  building various custom iterators for a given container

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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/test-coll.hpp"
#include "lib/util.hpp"

#include "lib/iter-adapter-stl.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using util::isnil;
  using std::cout;
  using std::endl;
  
  namespace iter = lib::iter_stl;
  
  
  /** test an iterator: create it by calling a constructor function
   *  and then pull out all contents and print them to STDOUT  */
#define TEST_ITER(_CTOR_, _ARG_) \
      cout << STRINGIFY(_CTOR_) ;\
      pullOut (_CTOR_ _ARG_)    ;\
      cout << endl;
  
  /** print descriptive separator to STDOUT */
#define PRINT_FUNC(_F_NAME_, _F_TYPE_) \
      cout << "-----"<<STRINGIFY(_F_NAME_)<<"---" << showType<_F_TYPE_>() << endl;
  
  
  namespace {
    uint NUM_ELMS = 10; 
  }
  
  
  
  
  
  
  
  
  /********************************************************************//**
   * @test verify some of the adapters to expose typical container
   *       usage patterns as Lumiera Forward Iterators.
   *       - distinct values from a sequence
   *       - keys and values of a map / multimap
   *       - distinct keys from a multimap
   *       - multimap values associated with a given key
   *       - snapshot of an iterator, to be retrieved as iterator
   * 
   * @see RangeIter
   * @see iter-adapter.hpp
   * @see iter-adapter-stl.hpp
   * @todo this test just pulls all the values from the iterator
   *       and the testsuite verifies the retrieved values.
   *       But for the hash-map, the order of the results
   *       is not guaranteed, and implementation dependent.
   */
  class IterAdapterSTL_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[1]);
          
          checkDistinctValIter();
          
          iterateMapKeyVal (getTestMap_int<MapII> (NUM_ELMS));
          iterateMapKeyVal (getTestMap_int<HMapII> (NUM_ELMS));
          
          iterateMapKeyVal (getTestMultiMap_int<MMapII> (NUM_ELMS));
          iterateMapKeyVal (getTestMultiMap_int<HMMapII> (NUM_ELMS));
          
          iterateValues4Key (getTestMultiMap_int<MMapII> (NUM_ELMS));
          iterateValues4Key (getTestMultiMap_int<HMMapII> (NUM_ELMS));
          
          checkIteratorSnapshot();
        }
      
      
      template<class MAP>
      void
      iterateMapKeyVal (MAP const& map)
        { 
          PRINT_FUNC (iterateMapKeyVal, MAP);
          
          TEST_ITER (iter::eachKey, (map));
          TEST_ITER (iter::eachKey, (map.begin(), map.end()));
          TEST_ITER (iter::eachVal, (map));
          TEST_ITER (iter::eachVal, (map.begin(), map.end()));
          TEST_ITER (iter::eachDistinctKey, (map));
        }
      
      
      template<class MMAP>
      void
      iterateValues4Key (MMAP const& mumap)
        {
          PRINT_FUNC (iterateValues4Key, MMAP);
          
          TEST_ITER (iter::eachValForKey, (mumap, 0));
          
          // non-existing key should yield empty iterator
          CHECK (! iter::eachValForKey (mumap, NUM_ELMS));
        }
      
      
      void
      checkDistinctValIter()
        {
          PRINT_FUNC (filter-distinct-values, VecI);
          
          VecI vec;
          TEST_ITER(iter::eachDistinct, (vec));
          
          vec.push_back (1);
          vec.push_back (1);
          vec.push_back (1);
          TEST_ITER(iter::eachDistinct, (vec));
          
          vec.push_back (2);
          vec.push_back (3);
          vec.push_back (3);
          vec.push_back (1);
          TEST_ITER(iter::eachDistinct, (vec));
          
          vec.push_back (1);
          vec.push_back (1);
          vec.push_back (1);
          vec.push_back (1);
          vec.push_back (1);
          TEST_ITER(iter::eachDistinct, (vec));
        }
      
      
      void
      checkIteratorSnapshot()
        {
          typedef VecI::iterator Iter;
          typedef RangeIter<Iter> Range;
          typedef iter::IterSnapshot<int> Snapshot;
          
          VecI vec = getTestSeq_int<VecI> (NUM_ELMS);
          Snapshot capture1 (vec.begin(), vec.end());
          
          Range range_of_all (vec.begin(), vec.end());
          Snapshot capture2 (range_of_all);
          CHECK (range_of_all);                       // snapshot doesn't affect given source iterator pos
          CHECK (capture2);
          
          CHECK (vec.begin() == range_of_all.getPos());
          CHECK (vec.end()   == range_of_all.getEnd());
          CHECK (!isnil (vec));
          
          // concurrent or intermittent modification
          vec.clear();
          CHECK (isnil (vec));
          CHECK (vec.end() != range_of_all.getEnd()); // range_of_all is now corrupted
          
          CHECK (capture1);                           // ...but the snapshots remain unaffected
          CHECK (capture2);
          CHECK (capture1 == capture2);               // can compare snapshots, based on actual contents
          
          vec.push_back(22);
          vec.push_back(44);
          Snapshot capture3 (vec.begin(), vec.end()); // take another snapshot from current contents
          CHECK (capture3);
          CHECK (capture3 != capture1);
          CHECK (capture3 != capture2);
          
          
          uint sum_should_be = (NUM_ELMS-1)*NUM_ELMS/2;
          
          CHECK (sum_should_be == sumAll (capture1));
          CHECK (!capture1);                          // this one is exhausted now
          CHECK ( capture2);                          // ...but the others are really independent
          CHECK ( capture3);
          CHECK (capture1 != capture2);               // comparison includes the current position
          
          CHECK (sum_should_be == sumAll (capture2));
          CHECK (!capture1);
          CHECK (!capture2);
          CHECK ( capture3);
          CHECK (capture1 == capture2);               // now again equal (both exhausted and equal contents)
          
          CHECK (22+44 == sumAll (capture3));
          CHECK (!capture1);
          CHECK (!capture2);
          CHECK (!capture3);
          CHECK (capture1 == capture2);
          CHECK (capture3 != capture1);               // all exhausted, but the difference in contents remains
          CHECK (capture3 != capture2);
        }
      
      template<class IT>
      uint
      sumAll (IT& it)
        {
          uint sum(0);
          cout << "snapshot";
          while (it)
            {
              cout << "-" << *it;
              sum += *it;
              ++it;
            }
          cout << endl;
          return sum;
        }
    };
  
  LAUNCHER (IterAdapterSTL_test, "unit common");
  
  
}} // namespace lib::test
