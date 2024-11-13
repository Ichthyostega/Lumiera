/*
  IterSource(Test)  -  how to build an opaque iterator-based data source

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

/** @file iter-source-test.cpp
 ** unit test \ref IterSource_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include "lib/iter-source.hpp"

#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include <string>
#include <list>
#include <map>



namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using boost::lexical_cast;
  using lib::time::TimeVar;
  using lib::test::randStr;
  using lib::test::randTime;
  using std::make_pair;
  using std::string;
  using std::list;
  
  using LERR_(ITER_EXHAUST);
  
  using iter_source::eachEntry;
  using iter_source::transform;
  using iter_source::singleVal;
  using iter_source::eachMapKey;
  using iter_source::eachMapVal;
  using iter_source::eachValForKey;
  using iter_source::eachDistinctKey;
  
  
  
  namespace { // Subject of test
    
    
    uint NUM_ELMS = 10;
    
    typedef const char* CStr;
    
    /**
     * Explicit implementation of the IterSource interface (test dummy)
     * Creates a random string and chops off a character on each iteration
     */
    class TestSource
      : public IterSource<CStr>
      , util::NonCopyable
      {
        
        string buffer_;
        CStr current_;
        
        virtual Pos                                    ////////////////////////////////////////////TICKET #1125 : this iteration control API should use three control functions, similar to IterStateWrapper
        firstResult ()  override
          {
            current_ = buffer_.c_str();
            ENSURE (current_);
            return &current_;
          }
        
        virtual void
        nextResult (Pos& pos)  override
          {
            if (pos && *pos && **pos)
              ++(*pos);
            
            if (!(pos && *pos && **pos))
              pos = 0;
          }
        
        
        
      public:
        TestSource (uint num)
          : buffer_(randStr (num))
          , current_(0)
          {
            INFO (test, "created TestSource(\"%s\")", cStr(buffer_));
          }
      };
    
    
    
    /** test dummy: simply wrapping an STL container
     *  and exposing a range as Lumiera Forward Iterator
     */
    struct WrappedList
      {
        list<int> data_;
        
        WrappedList(uint num)
          {
            while (num)
              data_.push_back(num--);
          }
        
        typedef list<int>::iterator sourceIter;
        typedef RangeIter<sourceIter> iterator;
        
        iterator begin() { return iterator(data_.begin(),data_.end()); }
        iterator end()   { return iterator();                          }
        
      };
    
    
    /** diagnostics helper */
    template<class IT>
    inline void
    pullOut (IT& iter)
    {
      for ( ; iter; ++iter )
        cout << "::" << *iter;
      cout << endl;
    }
  } // (END) impl test dummy containers
  
  
  
  
  
  
  
  /**************************************************************//**
   *  @test create some (opaque) data sources,
   *        and then pull the data out by iteration.
   *        Demonstrates simple usage of the IterSource interface
   *        
   * @see IterSource
   * @see PlacementIndex::Table#_eachEntry_4check real world usage example
   * @todo the output order of the values produced by this test
   *       is implementation dependent in for the hashmap case
   */
  class IterSource_test : public Test
    {
      
      typedef IterSource<int>::iterator IntIter;
      typedef IterSource<CStr>::iterator StrIter;
      typedef IterSource<string>::iterator StringIter;
      typedef IterSource<TimeVar>::iterator TimeIter;
      
      typedef std::map<string,TimeVar>           TreeMap;
      typedef std::unordered_map<string,TimeVar> HashMap;
      
      typedef std::multimap<int,int>          TreeMultimap;
      typedef std::unordered_multimap<int,int>HashMultimap;
      
      
      virtual void
      run (Arg arg)
        {
          seedRand();
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[1]);
          
          verify_simpleIters();
          verify_transformIter();
          
          verify_MapWrappers<TreeMap>();
          verify_MapWrappers<HashMap>();
          
          verify_MultimapIters<TreeMultimap>();
          verify_MultimapIters<HashMultimap>();
        }
      
      
      void
      verify_simpleIters()
        {
          // build the test data sources
          WrappedList customList(NUM_ELMS);
          TestSource dedicatedSource(NUM_ELMS);
          list<int>& rawList(customList.data_);
          
          IntIter iii (eachEntry (customList));
          IntIter isi (eachEntry (rawList.begin(), rawList.end()));
          StrIter cii (IterSource<CStr>::build(dedicatedSource));
          
          CHECK (!isnil (iii));
          CHECK (!isnil (isi));
          CHECK (!isnil (cii));
          
          pullOut (iii);
          pullOut (isi);
          pullOut (cii);
          
          CHECK (!iii);
          CHECK (!isi);
          CHECK (!cii);
        }
      
      
      /** @test verify transforming an embedded iterator
       * This test not only wraps a source iterator and packages it behind the
       * abstracting interface IterSource, but in addition also applies a function
       * to each element yielded by the source iterator. As demo transformation
       * we use the values from our custom container (\ref WrappedList) to build
       * a time value in quarter seconds
       *
       */
      void
      verify_transformIter()
        {
          WrappedList customList(NUM_ELMS);
          WrappedList::iterator sourceValues = customList.begin();

          // transformation function
          auto makeTime = [](int input_sec) -> TimeVar
                            {
                              return time::Time (time::FSecs (input_sec, 4));
                            };
          
          TimeIter tIt (transform (sourceValues, makeTime));
          CHECK (!isnil (tIt));
          pullOut (tIt);
          CHECK (!tIt);
        }
      
      
      /** @test an IterSouce which returns just a single value once */
      void
      verify_singleValIter()
        {
          int i{-9};

          IntIter ii = singleVal(12);
          CHECK (not isnil(ii));
          CHECK (12 == *ii);

          ++ii;
          CHECK (isnil (ii));
          VERIFY_ERROR (ITER_EXHAUST, *ii );

          // IterSource is an abstracting interface,
          // thus we're able to reassign an embedded iterator
          // with a different value type (int& instead of int)
          ii = singleVal(i);

          CHECK (not isnil(ii));
          CHECK (-9 == *ii);

          // NOTE: since we passed a reference, a reference got wrapped
          i = 23;
          CHECK (23 == *ii);
          ++ii;
          CHECK (isnil (ii));
        }
      
      
      
      
      template<class MAP>
      void
      verify_MapWrappers()
        {
          MAP testMap;
          for (uint i=0; i<NUM_ELMS; ++i)
            testMap['X'+randStr(11)] = randTime();
          
          StringIter sIter = eachMapKey (testMap);
          TimeIter   tIter = eachMapVal (testMap);
          
          CHECK (sIter && tIter);
          pullOut (sIter);
          pullOut (tIter);
          
          CHECK (!sIter && !tIter);
          
          
          // The each-value-for-given-key-Iterator can be used for a map or multimap.
          // In case of a map, it should yield exactly one result for existing values
          // and zero results for non-existing mappings
          StringIter justSomeKey = eachMapKey (testMap);
          TimeIter correspondingVal = eachMapVal (testMap);
          ++justSomeKey;
          ++correspondingVal;
          CHECK (justSomeKey);
          
          TimeIter value4key = eachValForKey (testMap, "nonexistent key");
          CHECK (!value4key);
          
          value4key = eachValForKey (testMap, *justSomeKey);
          CHECK (value4key);
          CHECK (*value4key == *correspondingVal);
          ++value4key;
          CHECK (!value4key);
        }
      
      
      template<class MAP>
      void
      verify_MultimapIters()  ///< @see IterTools_test#verify_filterRepetitions
        {
          MAP testMap;
          for (uint i=0; i<NUM_ELMS; ++i)
            {
              uint n = 1 + rani(100);
              do testMap.insert (make_pair (i,n));
              while (--n);
            }
          CHECK (NUM_ELMS < testMap.size(), "no repetition in test data??");
          
          IntIter keys = eachDistinctKey (testMap);
          
          cout << "distinct_keys";
          CHECK (keys);
          pullOut (keys);
          CHECK (!keys);
          
          cout << "values_4_key";
          IntIter vals = eachValForKey (testMap, NUM_ELMS); // non-existent key
          CHECK (!vals);
          
          vals = eachValForKey (testMap, 0);
          CHECK (vals);
          pullOut (vals); // should produce anything between 1 and 100 entries
          CHECK (!vals);
        }
    };
  
  
  LAUNCHER (IterSource_test, "unit common");
  
  
}} // namespace lib::test

