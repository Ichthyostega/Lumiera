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
#include "lib/util.hpp"

#include "lib/iter-source.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>
#include <list>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using boost::noncopyable;
  using util::isnil;
  using std::list;
  using std::cout;
  using std::endl;
  
  
  
  namespace { // Subject of test
    
    
    uint NUM_ELMS = 10;
    
    /** 
     * Explicit implementation of the IterSource interface (test dummy)
     */
    class TestSource
      : public IterSource<char*>
      , noncopyable
      {
        
      virtual Pos
      firstResult ()
        {
          UNIMPLEMENTED ("start iteration");
        }
      
      virtual void
      nextResult (Pos& pos)
        {
          UNIMPLEMENTED ("iteration step");
        }
      
        
        
      public:
        TestSource (uint num)
          {
            UNIMPLEMENTED ("dedicated IterSource implementation");
          }
      };
    
      
    /** test dummy, simply wrapping an STL container
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
        
        typedef list<int>::const_iterator const_sourceIter;
        typedef RangeIter<const_sourceIter> const_iterator;
        
        iterator       begin()       { return       iterator(data_.begin(),data_.end()); }
        iterator       end()         { return       iterator();                          }
        const_iterator begin() const { return const_iterator(data_.begin(),data_.end()); }
        const_iterator end()   const { return const_iterator();                          }
        
      };
    
  } // (END) impl test dummy containers
  
  
  
  
  
  
  
  /******************************************************************
   *  @test create some (opaque) data sources,
   *        and then pull the data out by iteration.
   *        Demonstrates simple usage of the IterSource inteface
   *        
   */
  class IterSource_test : public Test
    {
      
      typedef IterSource<int>::iterator IntIter;
      typedef IterSource<char*>::iterator StrIter;
    
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          // build the test data sources
          WrappedList customList(NUM_ELMS);
          TestSource dedicatedSource(NUM_ELMS);
          
          IntIter iii (eachEntry (customList));
          StrIter cii (IterSource<char*>::build(dedicatedSource));
          
          ASSERT (!isnil (iii));
          ASSERT (!isnil (cii));
          
          pullOut (iii);
          pullOut (cii);
          
          ASSERT (!iii);
          ASSERT (!cii);
        }
      
      
      
      template<class IT>
      void
      pullOut (IT const& ii)
        {
          for (IT iter(ii) ; iter; ++iter )
            cout << "::" << *iter;
          cout << endl;
        }
      
    };
  
  LAUNCHER (IterSource_test, "unit common");
  
  
}} // namespace lib::test

