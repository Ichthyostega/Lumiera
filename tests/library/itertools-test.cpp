/*
  IterTools(Test)  -  building combined and filtering iterators based on the Iterator tools

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file §§§
 ** unit test TODO §§§
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "lib/itertools.hpp"

#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using util::for_each;
  using util::isnil;
  using std::vector;
  using std::rand;
  
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  
  
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
  
  
  
  
  
  
  
  /***************************************************************************//**
   *  @test build combined and filtering iterators with the help of lib::IterTool.
   *        Check correct behaviour of the resulting iterators and
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
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[1]);
          
          TestSource source(NUM_ELMS);
          
          pullOut (source.begin());
          verifyComparisons (source.begin());
          
          
          buildFilterIterator (source.begin());
          Iter ii (source.begin());
          ++++++ii;
          buildFilterIterator (ii);
          verify_filterExtension();
          verify_filterRepetitions();
          
          buildTransformingIterator (source.begin());
          
          verifyPullLast(source.begin());
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
      static bool takeOdd (int i) { return 0 != i % 2; }
      static bool takeEve (int i) { return 0 == i % 2; }
      static bool takeTrd (int i) { return 0 == i % 3; }
      
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
            CHECK (all != odd);
          
          while (++all) { }
          CHECK (isnil (odd));
          CHECK (all == odd);
        }
      
      
      /** @test verify the ability to extend a filter condition
       *        while in the middle of an ongoing iteration.
       * Typically this means sharpening the filter condition
       * and thus making the filter more restrictive, filtering
       * away more elements of the source stream. But through
       * the ability to add disjunctive and negated clauses,
       * it is also possible to weaken the filter condition
       * @note in case of a weakened filter condition, there is
       *       \em no reset of the source iterator, i.e. we don't
       *       re-evaluate from start, but just from current head.
       */
      void
      verify_filterExtension ()
        {
          typedef vector<uint64_t> Src;
          typedef Src::iterator SrcIter;
          typedef RangeIter<SrcIter> SeqIter;
          typedef ExtensibleFilterIter<SeqIter> FilteredSeq;
          
          Src src;
          for (uint i=0; i < 3*NUM_ELMS; ++i)
            src.push_back(i);
          
          SeqIter completeSequence (src.begin(), src.end());
          FilteredSeq filterIter (completeSequence, takeAll);
          
          CHECK (!isnil (filterIter));
          CHECK (0 == *filterIter);
          ++filterIter;
          CHECK (1 == *filterIter);
          
          filterIter.andFilter(takeEve);
          CHECK (!isnil (filterIter));
          CHECK (2 == *filterIter);
          ++filterIter;
          CHECK (4 == *filterIter);
          
          // sharpen the condition...
          filterIter.andFilter(takeTrd);
          CHECK (!isnil (filterIter));
          CHECK (6 == *filterIter);   // divisible by two and by three
          ++filterIter;
          CHECK (12 == *filterIter);
          
          verifyComparisons (filterIter);
          pullOut (filterIter);
          
          // adding a disjunctive clause actually weakens the filter...
          filterIter = {completeSequence, takeTrd};
          CHECK (!isnil (filterIter));
          CHECK (0 == *filterIter);
          ++filterIter;
          CHECK (3 == *filterIter);
          
          filterIter.orFilter(takeEve);
          CHECK (3 == *filterIter);
          ++filterIter;
          CHECK (4 == *filterIter);
          ++filterIter;
          CHECK (6 == *filterIter);
          verifyComparisons (filterIter);
          
          // flip filter logic
          filterIter.flipFilter();
          CHECK (7 == *filterIter);   // not even and not divisible by three
          ++filterIter;
          CHECK (11 == *filterIter);
          ++filterIter;
          CHECK (13 == *filterIter);
          
          verifyComparisons (filterIter);
          pullOut (filterIter);
        }
      
      
      /** @test verify the helper to filter duplicate elements
       *        emitted by an source iterator. This test creates
       *        a sequence of numbers with random repetitions.
       */
      void
      verify_filterRepetitions ()
        {
          vector<uint> numberz;
          for (uint i=0; i<NUM_ELMS; ++i)
            {
              uint n = 1 + rand() % 100;
              do numberz.push_back(i);
              while (--n);
            }
          CHECK (NUM_ELMS < numberz.size(), "no repetition in test data??");
          
          typedef vector<uint>::iterator SrcIter;
          typedef RangeIter<SrcIter> SeqIter;
          typedef FilterIter<SeqIter> FilteredSeq;
          
          SeqIter completeSequence (numberz.begin(), numberz.end());
          FilteredSeq filtered = filterRepetitions (completeSequence);
          
          uint num=0;
          for (; num<NUM_ELMS && !isnil(filtered);
               ++num,
               ++filtered
              )
            CHECK (num == *filtered);
          
          CHECK (num == NUM_ELMS && isnil(filtered));
        }
      
      
      
      static ulong addTwo (int i) { return i+2; }
      static int   negate (int i) { return -i; }
      static int   idFunc (int i) { return i; }
      
      void
      buildTransformingIterator (Iter const& ii)
        {
          pullOut (transformIterator(ii, idFunc));
          pullOut (transformIterator(ii, negate));
          pullOut (transformIterator(ii, addTwo));  // note: changing output type to unsigned
          
          TransformIter<Iter, int> idi (ii, idFunc);
          TransformIter<Iter, int> neg (ii, negate);
          verifyComparisons (idi);
          verifyComparisons (neg);
          
          CHECK (idi);
          CHECK (neg);
          for ( ;idi&&neg;
              ++idi,++neg)
            CHECK (idi != neg);
          
          CHECK (!idi && !neg);
          CHECK (idi == neg);
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
          CHECK ( isnil (iN));
          CHECK (!isnil (i1));
          CHECK (!isnil (i2));
          
          CHECK (i1 == i2); CHECK (i2 == i1);
          CHECK (i1 != iN); CHECK (iN != i1);
          CHECK (i2 != iN); CHECK (iN != i2);
          
          ++i1;
          CHECK (i1 != i2);
          CHECK (i1 != iN);
          
          ++i2;
          CHECK (i1 == i2);
          CHECK (i1 != iN);
          CHECK (i2 != iN);
          
          while (++i1) { }
          CHECK (isnil(i1));
          CHECK (i1 != i2);
          CHECK (i1 == iN);
          
          while (++i2) { }
          CHECK (isnil(i2));
          CHECK (i2 == i1);
          CHECK (i2 == iN);
        }
      
      
      void
      verifyPullLast(Iter const& ii)
        {
          Iter::value_type lastElm = pull_last (ii);
          CHECK (1 == lastElm);     // TestSource holds a decreasing sequence of numbers ending with 1
          
          Iter emptyIterator;
          CHECK (isnil (emptyIterator));
          
          VERIFY_ERROR (ITER_EXHAUST, pull_last(emptyIterator) );
        }
    };
  
  LAUNCHER (IterTools_test, "unit common");
  
  
}} // namespace lib::test

