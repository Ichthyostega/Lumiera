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
 ** unit test §§TODO§§
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "lib/iter-source.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include <list>
#include <map>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using boost::noncopyable;
  using lib::time::TimeVar;
  using lib::test::randStr;
  using lib::test::randTime;
  using util::isnil;
  using util::cStr;
  using std::make_pair;
  using std::string;
  using std::list;
  using std::rand;
  
  
  
  namespace { // Subject of test
    
    
    uint NUM_ELMS = 10;
    
    typedef const char* CStr;
    
    /**
     * Explicit implementation of the IterSource interface (test dummy)
     * Creates a random string and chops off a character on each iteration
     */
    class TestSource
      : public IterSource<CStr>
      , noncopyable
      {
        
        string buffer_;
        CStr current_;
        
        virtual Pos
        firstResult ()
          {
            current_ = buffer_.c_str();
            ENSURE (current_);
            return &current_;
          }
        
        virtual void
        nextResult (Pos& pos)
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
      
      
      void
      verify_transformIter()
        {
          WrappedList customList(NUM_ELMS);
          WrappedList::iterator sourceValues = customList.begin(); 
          
          TimeIter tIt (transform (sourceValues, makeTime));
          CHECK (!isnil (tIt));
          pullOut (tIt);
          CHECK (!tIt);
        }
      
      /** transformation function, to be applied for each element:
       *  just build a time value, using the input as 1/4 seconds
       */
      static TimeVar
      makeTime (int input_sec)
        {
          return time::Time (time::FSecs (input_sec, 4));
        }
      
      
      
      template<class IT>
      void
      pullOut (IT& iter)
        {
          for ( ; iter; ++iter )
            cout << "::" << *iter;
          cout << endl;
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
              uint n = 1 + rand() % 100;
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

