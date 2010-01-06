/*
  IterSource(Test)  -  how to build an opaque iterator-based data source
 
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
#include "lib/lumitime-fmt.hpp"
#include "lib/util.hpp"

#include "lib/iter-source.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <tr1/unordered_map>
#include <iostream>
#include <string>
#include <list>
#include <map>



namespace lib {
namespace test{
  
  using ::Test;
  using lumiera::Time;
  using boost::lexical_cast;
  using boost::noncopyable;
  using lib::test::randStr;
  using lib::test::randTime;
  using util::isnil;
  using util::cStr;
  using std::string;
  using std::list;
  using std::cout;
  using std::endl;
  
  
  
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
  
  
  
  
  
  
  
  /******************************************************************
   *  @test create some (opaque) data sources,
   *        and then pull the data out by iteration.
   *        Demonstrates simple usage of the IterSource interface
   *        
   * @see IterSource
   * @see PlacementIndex::Table#_eachEntry_4check real world usage example
   */
  class IterSource_test : public Test
    {
      
      typedef IterSource<int>::iterator IntIter;
      typedef IterSource<CStr>::iterator StrIter;
      typedef IterSource<const string>::iterator StringIter;
      typedef IterSource<Time>::iterator TimeIter;
      
      typedef std::map<string,Time>                TreeMap;
      typedef std::tr1::unordered_map<string,Time> HashMap;
      
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          verify_simpleIters();
          
          verify_MapWrappers<TreeMap>();
          verify_MapWrappers<HashMap>();
        }
      
      
      void
      verify_simpleIters()
        {
          // build the test data sources
          WrappedList customList(NUM_ELMS);
          TestSource dedicatedSource(NUM_ELMS);
          
          IntIter iii (eachEntry (customList));
          StrIter cii (IterSource<CStr>::build(dedicatedSource));
          
          ASSERT (!isnil (iii));
          ASSERT (!isnil (cii));
          
          pullOut (iii);
          pullOut (cii);
          
          ASSERT (!iii);
          ASSERT (!cii);
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
          
          ASSERT (sIter && tIter);
          pullOut (sIter);
          pullOut (tIter);
          
          ASSERT (!sIter && !tIter);
        }
      
    };
  
  LAUNCHER (IterSource_test, "unit common");
  
  
}} // namespace lib::test

