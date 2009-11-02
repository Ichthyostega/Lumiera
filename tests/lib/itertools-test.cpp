/*
  IterTools(Test)  -  building combined and filtering iterators based on the Iterator tools
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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

#include "lib/itertools.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using util::for_each;
  using util::isnil;
  using std::vector;
  using std::cout;
  using std::endl;
  
  
  
  namespace { // Test data
  
    uint NUM_ELMS = 10;
    
    struct TestSource
      {
        vector<int> data_;
        
        TestSource(uint num)
          {
            while (num)
              data_.push_back(num--);
          }
        
        typedef vector<int>::iterator sourceIter;
        typedef RangeIter<sourceIter> iterator;
        
        iterator begin() { return iterator(data_.begin(),data_.end()); }
        iterator end()   { return iterator();                          }
        
      };
    
  } // (END) Test data
  
  
  
  
  
  
  
  /*******************************************************************************
   *  @test build combined and filtering iterators with the help of lib::IterTool.
   *        Check correct behaviour of the resulting iterator and 
   *        verify they fulfil the Lumiera Forward Iterator concept
   *        
   * @todo implement more iterator tools.... see Ticket #347
   */
  class IterTools_test : public Test
    {
      
      typedef TestSource::iterator Iter;
      
      
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          TestSource source(NUM_ELMS);
          
          pullOut (source.begin());
          verifyComparisons (source.begin());
          
          
          buildFilterIterator (source.begin());
          Iter ii (source.begin());
          ++++++ii;
          buildFilterIterator (ii);
        }
      
      
      
      template<class IT>
      void
      pullOut (IT const& ii)
        {
          for (IT iter(ii) ; iter; ++iter )
            cout << "::" << *iter;
          cout << endl;
        }
      
      
      
      static bool takeAll (int)   { return true; }
      static bool takeEve (int i) { return 0 == i % 2; }
      static bool takeOdd (int i) { return 0 != i % 2; }
      
      void
      buildFilterIterator (Iter const& ii)
        {
          pullOut (filterIterator (ii, takeAll));  // note: using the convenient builder function 
          pullOut (filterIterator (ii, takeEve));
          pullOut (filterIterator (ii, takeOdd));
          
          FilterIter<Iter> all (ii, takeAll);
          FilterIter<Iter> odd (ii, takeOdd);
          verifyComparisons (all);
          verifyComparisons (odd);
          
          while (++all && ++odd)
            ASSERT (all != odd);
          
          while (++all) { }
          ASSERT (isnil (odd));
          ASSERT (all == odd);
        }
      
      
      
      /** @test verify equality handling and NIL detection
       *        for the given iterator/wrapper handed in */
      template<class IT>
      void
      verifyComparisons (IT const& ii)
        {
          IT i1(ii);
          IT i2(ii);
          IT iN;
          ASSERT ( isnil (iN));
          ASSERT (!isnil (i1));
          ASSERT (!isnil (i2));
          
          ASSERT (i1 == i2); ASSERT (i2 == i1);
          ASSERT (i1 != iN); ASSERT (iN != i1); 
          ASSERT (i2 != iN); ASSERT (iN != i2);
          
          ++i1;
          ASSERT (i1 != i2);
          ASSERT (i1 != iN);
          
          ++i2;
          ASSERT (i1 == i2);
          ASSERT (i1 != iN); 
          ASSERT (i2 != iN);
          
          while (++i1) { }
          ASSERT (isnil(i1));
          ASSERT (i1 != i2);
          ASSERT (i1 == iN);
          
          while (++i2) { }
          ASSERT (isnil(i2));
          ASSERT (i2 == i1);
          ASSERT (i2 == iN);
        }
      
      
    };
  
  LAUNCHER (IterTools_test, "unit common");
  
  
}} // namespace lib::test

