/*
  SplitSplice(Test)  -  verify interval splicing

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file split-splice-test.cpp
 ** unit test \ref SplitSplice_test
 ** to verify proper working of the »SplitSplice« algorithm.
 ** This is a generic setup to modify a segmentation (partitioning)
 ** of an ordered axis; the axis is represented as a collection of _segments,_
 ** which are assumed to be ordered and seamless, with the start point inclusive
 ** and the end point exclusive (thus the start of the next segment is identical
 ** with the end point of the current segment).
 ** 
 ** This test uses the natural number axis between -100 ... +100
 ** and establishes a binding for the generic algorithm with suitably rigged
 ** test data, to verify the algorithm properly inserts a new segment under all
 ** conceivable circumstances, since there are many possibilities of arrangement
 ** for two ordered segments of arbitrary length.
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/format-string.hpp"
#include "lib/split-splice.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <list>


namespace lib {
namespace test {
  
  using util::_Fmt;
  using util::isnil;
  using std::string;
  using std::move;

  namespace  {// Test Fixture....
    
    /**
     * Test Dummy: a "segment" representing an integer interval.
     * Memory management can be tracked since each instance gets a
     * distinct ID number. Moreover, a Seg can be marked as "empty",
     * which is visible on the `string` conversion
     */
    struct Seg
      : util::MoveOnly
      {
        int start;
        int after;
        bool empty;
        
       ~Seg()
          {
            check -= id;
            if (id) --cnt;
          }
        
        Seg (int s, int a, bool nil=false)
          : start{s}
          , after{a}
          , empty{nil}
          , id{++idGen}
          {
            ++cnt;
            check += id;
          }
        
        /** create a clone, but modify bounds */
        Seg (Seg const& ref, int s, int a)
          : start{s}
          , after{a}
          , empty{ref.empty}
          , id{ref.id}
          {
            ++cnt;
            check += id;
          }
        
        /** move-init: causes source-ref to be invalidated */
        Seg (Seg&& rr)
          : start{rr.start}
          , after{rr.after}
          , empty{rr.empty}
          , id{0}
          {
            std::swap (id, rr.id);
          }//transfer identity
        
        operator string()  const
          {
            return _Fmt{"[%i%s%i["}
                       % start
                       % (empty? "~":"_")
                       % after
                       ;
          }
        
        
        //-- Diagnostics --
        uint id;
        static uint idGen;
        static size_t cnt;
        static size_t check;
      };
      
    // Storage for static ID-Generator
    size_t Seg::check{0};
    size_t Seg::cnt{0};
    uint Seg::idGen{0};
    
    const int SMIN = -100;
    const int SMAX = +100;
    
    /**
     * Test-Segmentation comprised of a sequence of Seg entries.
     * It can be checked for _validity_ according to the following conditions
     * - the segmentation spans the complete range -100 ... +100
     * - segments follow each other _seamlessly_
     * - the bounds within each segment are ordered ascending
     * - segments are not empty (i.e. start differs from end)
     * The assessment of this validity conditions is appended on the string conversion.
     */
    struct SegL
      : std::list<Seg>
      {
        SegL (std::initializer_list<int> breaks)
          {
            int p = SMIN;
            bool bound = true;
            for (int b : breaks)
              {
                emplace_back (p,b, bound);
                bound = false;
                p = b;
              }
            emplace_back(p,SMAX, true);
          }
        
        SegL()
          : SegL({})
        { }
        
        // using standard copy
        
        operator string()  const
          {
            return renderContent() + assess();
          }
        
        bool
        isValid()  const
          {
            return isnil (this->assess());
          }
        
        string
        renderContent()  const
          {
            return "├"+util::join(*this,"")+"┤";
          }
        
        string
        assess()  const
          {
            string diagnosis;
            if (empty())
              diagnosis = "!empty!";
            else
              {
                if (front().start != SMIN)
                  diagnosis += "missing-lower-bound!";
                if (back().after != SMAX)
                  diagnosis += "missing-upper-bound!";
                int p = SMIN;
                for (auto const& s : *this)
                  {
                    if (s.start != p)
                      diagnosis += _Fmt{"!gap_%i<>%i_!"} % p % s.start;
                    if (s.start == s.after)
                      diagnosis += _Fmt{"!degen_%i_!"} % s.start;
                    if (s.start > s.after)
                      diagnosis += _Fmt{"!order_%i>%i_!"} % s.start % s.after;
                    p = s.after;
                  }
              }
            return diagnosis;
          }
      };
    
    
    
    
    /* ======= Split/Splice-Algo Setup ======= */
    
    using OptInt = std::optional<int>;
    using Iter = typename SegL::iterator;
    
    /**
     * Perform the »SplitSplice« Algorithm to splice a new Segment
     * into the given [segmentation of the integer-axis](\ref SegL).
     * A local λ-binding is setup to define the basic operations
     * required by the algorithm implementation to work with this
     * specific kind of data.
     * @return Tuple `(s,n,e)` to indicate where changes happened
     *   - s the first changed element
     *   - n the new main segment (may be identical to s)
     *   - e the first unaltered element after the changed range (may be `end()`)
     * @see lib::splitsplice::Algo
     * @see [setup for the Fixture-Segmentation](\ref steam::fixture::Segmentation::splitSplice)
     */
    auto
    invokeSplitSplice (SegL& segs, OptInt startNew, OptInt afterNew)
    {
      /*---configure-contextually-bound-Functors--------------------*/
      auto getStart =  [](Iter elm)                                 -> int  { return elm->start; };
      auto getAfter =  [](Iter elm)                                 -> int  { return elm->after; };
      auto createSeg= [&](Iter pos, int start, int after)           -> Iter { return segs.emplace (pos, start, after); };
      auto emptySeg = [&](Iter pos, int start, int after)           -> Iter { return segs.emplace (pos, start, after, true); };
      auto cloneSeg = [&](Iter pos, int start, int after, Iter src) -> Iter { return segs.emplace (pos, *src, start, after); };
      auto discard  = [&](Iter pos, Iter after)                     -> Iter { return segs.erase (pos,after); };
      
      
      lib::splitsplice::Algo splicer{ getStart
                                    , getAfter
                                    , createSeg
                                    , emptySeg
                                    , cloneSeg
                                    , discard
                                    , SMAX
                                    , segs.begin(),segs.end()
                                    , startNew, afterNew
                                    };
      splicer.determineRelations();
      return splicer.performSplitSplice();
    }
  }//(End)Test Fixture
  
  
  
  
  /****************************************************************************//**
   * @test verify proper working of a generic procedure to splice an interval
   *       into a complete segmentation of an ordered axis into seamless intervals.
   *       - demonstrate how to setup the invocation with custom data types
   *       - systematic coverage of all possible arrangements of intervals
   *       - handling of irregular cases
   * @see  split-splice.hpp
   * @see  steam::fixture::Segmentation::splitSplice
   */
  class SplitSplice_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_usage();
          verify_testFixture();
          verify_standardCases();
          verify_cornerCases();
        }
      
      
      /**
       * @test demonstrate how to use the »Split-Splice« algorithm with custom data
       */
      void
      demonstrate_usage()
        {
          SegL segmentation;
          CHECK (segmentation == "├[-100~100[┤"_expect);
          
          OptInt startNew{5},
                 afterNew{23};
          
          invokeSplitSplice (segmentation, startNew, afterNew);

          // The given segmentation was modified by side-effect
          // - a new segment [5...23[ has been inserted in the middle
          // - suitably adapted empty predecessor and successor segments
          CHECK (segmentation == "├[-100~5[[5_23[[23~100[┤"_expect);
          
          // The modified segmentation still seamlessly covers the whole axis
          CHECK (segmentation.isValid());
        }
      
      
      
      /** @test verify the fixture and self-diagnostics for this test */
      void
      verify_testFixture()
        {
          CHECK (0 == Seg::check);
          Seg::idGen = 0;
          {
            Seg x{1,3};                          // a segment 1 (inclusive) to 3 (exclusive)
            Seg u{2,4,true};                     // an "empty" segment  2 (incl) to 4 (excl)
            CHECK (x == "[1_3["_expect);
            CHECK (u == "[2~4["_expect);         // "empty" interval is marked with '~' in string stylisation
            CHECK (3 == Seg::check);
            CHECK (2 == Seg::cnt);
            
            Seg z{move(u)};
            CHECK (z == "[2~4["_expect);
            CHECK (3 == Seg::check);
            CHECK (2 == Seg::cnt);               // the "dead" instance u is not counted
            CHECK (0 == u.id);                   // (its ID has been reset to zero in move-ctor)
            CHECK (2 == z.id);
            
            SegL l1;                             // default ctor always adds an empty base segment -100 ... +100
            SegL l2{3};
            SegL l3{5,-5,10};
            CHECK (l1 == "├[-100~100[┤"_expect);
            CHECK (l2 == "├[-100~3[[3~100[┤"_expect);
            CHECK (l3 == "├[-100~5[[5_-5[[-5_10[[10~100[┤!order_5>-5_!"_expect);
            
            CHECK (l1.isValid());
            CHECK (l2.isValid());
            CHECK (not l3.isValid());            // l3 violates validity condition, because segment [5 ... -5[ is reversed
            CHECK (l3.assess() == "!order_5>-5_!"_expect);
            
            CHECK ( 9 == Seg::cnt  );            // 9 objects are alive
            CHECK ( 9 == Seg::idGen);            // ID generator sticks at 9
            CHECK (45 == Seg::check);            // checksum 1+..+9
            
            l3.erase(l3.begin());
            CHECK (l3.assess() == "missing-lower-bound!!gap_-100<>5_!!order_5>-5_!"_expect);
            CHECK ( 8 == Seg::cnt  );            // also one object less alive
            
            l3.begin()->after = 5;               // manipulate first segment to make it degenerate (empty
            CHECK (l3.renderContent() == "├[5_5[[-5_10[[10~100[┤"_expect);
            CHECK (l3.assess()        == "missing-lower-bound!!gap_-100<>5_!!degen_5_!!gap_5<>-5_!"_expect);
            l3.clear();
            CHECK (l3.assess() == "!empty!"_expect);
            
            CHECK ( 5 == Seg::cnt  );
            CHECK ( 9 == Seg::idGen);
            CHECK (15 == Seg::check);
          }
          // all objects go out of scope
          CHECK (0 == Seg::cnt  );
          CHECK (0 == Seg::check);
          CHECK (9 == Seg::idGen);
        }
      
      
      
      /**
       * @test cover all possible cases of splicing an interval
       */
      void
      verify_standardCases()
        {
          auto testCase = [](SegL segmentation
                            ,int startNew
                            ,int afterNew
                            ,ExpectString expectedResult)
                            {
                              OptInt startSpec{startNew},
                                     afterSpec{afterNew};
                              
                              invokeSplitSplice (segmentation, startSpec, afterSpec);
                              CHECK (segmentation == expectedResult);
                              CHECK (segmentation.isValid());
                            };
                                                             ////////
          testCase (SegL{}, -23,24,              "├[-100~-23[[-23_24[[24~100[┤"_expect);            // simple segment into empty axis
          
          // insert smaller segment
          testCase (SegL{5,10},  2,3,                 "├[-100~2[[2_3[[3~5[[5_10[[10~100[┤"_expect); // smaller segment left spaced off
          testCase (SegL{5,10},  4,5,                 "├[-100~4[[4_5[[5_10[[10~100[┤"_expect);      // left adjacent
          testCase (SegL{5,10},  4,8,                 "├[-100~4[[4_8[[8_10[[10~100[┤"_expect);      // left overlapping
          testCase (SegL{5,10},  5,8,                 "├[-100~5[[5_8[[8_10[[10~100[┤"_expect);      // left inside justified
          testCase (SegL{5,10},  6,8,            "├[-100~5[[5_6[[6_8[[8_10[[10~100[┤"_expect);      // smaller segment complete inside
          testCase (SegL{5,10},  7,10,          "├[-100~5[[5_7[[7_10[[10~100[┤"_expect);            // right inside justified
          testCase (SegL{5,10},  9,13,          "├[-100~5[[5_9[[9_13[[13~100[┤"_expect);            // right overlapping
          testCase (SegL{5,10}, 10,13,        "├[-100~5[[5_10[[10_13[[13~100[┤"_expect);            // right adjacent
          testCase (SegL{5,10}, 13,23, "├[-100~5[[5_10[[10~13[[13_23[[23~100[┤"_expect);            // right spaced off
          
          // insert identical segment
          testCase (SegL{5,10},  5,10,               "├[-100~5[[5_10[[10~100[┤"_expect);            // identical size replacement
          
          // insert larger segment
          testCase (SegL{5,10},  3,10,               "├[-100~3[[3_10[[10~100[┤"_expect);            // larger segment right aligned
          testCase (SegL{5,10},  3,23,               "├[-100~3[[3_23[[23~100[┤"_expect);            // larger segment overarching
          testCase (SegL{5,10},  5,23,               "├[-100~5[[5_23[[23~100[┤"_expect);            // larger segment left aligned
        }                                                      //////
      
      
      
      /**
       * @test cover special and boundary cases
       */
      void
      verify_cornerCases()
        {
          auto testCase = [](SegL segmentation
                            ,OptInt startNew           // Note: these are optional...
                            ,OptInt afterNew
                            ,ExpectString expectedResult)
                            {
                              invokeSplitSplice (segmentation, startNew, afterNew);
                              CHECK (segmentation == expectedResult);
                              CHECK (segmentation.isValid());
                            };
          auto x = std::nullopt;
                                                       //////
          testCase (SegL{}, 3,2,              "├[-100~2[[2_3[[3~100[┤"_expect);           // flipped interval spec is reoriented
                                                       //////
          testCase (SegL{}, 3,x,            "├[-100~3[[3_100[┤"_expect);                  // expanded until domain end
          testCase (SegL{}, x,5,                   "├[-100_5[[5~100[┤"_expect);           // expanded to start of domain
                                                        /////
          testCase (SegL{4,6}, 5,x,      "├[-100~4[[4_5[[5_6[[6~100[┤"_expect);           // expanded until end of enclosing segment
          testCase (SegL{4,6}, x,5,           "├[-100~4[[4_5[[5_6[[6~100[┤"_expect);      // expanded to start of enclosing segment
                                                        /////
          testCase (SegL{4,6}, 3,x,           "├[-100~3[[3_4[[4_6[[6~100[┤"_expect);      // expanded to fill gap to next segment
          testCase (SegL{4,6}, x,3,                "├[-100_3[[3~4[[4_6[[6~100[┤"_expect); // expanded to cover predecessor completely
          testCase (SegL{4,6}, 4,x,           "├[-100~4[[4_6[[6~100[┤"_expect);           // expanded to cover (replace) successor
          testCase (SegL{4,6}, x,4,           "├[-100_4[[4_6[[6~100[┤"_expect);           // expanded to cover (replace) predecessor
                                                        /////
          testCase (SegL{4,6}, 7,x, "├[-100~4[[4_6[[6~7[[7_100[┤"_expect);                // shorten successor and expand new segment to end of successor (=domain end)
          testCase (SegL{4,6}, x,7,      "├[-100~4[[4_6[[6_7[[7~100[┤"_expect);           // fill gap between predecessor and given new segment end
          testCase (SegL{4,6}, 6,x,      "├[-100~4[[4_6[[6_100[┤"_expect);                // expand to cover (replace) the following segment until domain end
          testCase (SegL{4,6}, x,6,           "├[-100~4[[4_6[[6~100[┤"_expect);           // expanded to cover (replace) the preceding segment
                                                        /////
          testCase (SegL{},    x,x,                "├[-100_100[┤"_expect);                // without any specification, the whole domain is covered
          testCase (SegL{4},   x,x,           "├[-100~4[[4_100[┤"_expect);                // otherwise, without any spec the last segment is replaced
          testCase (SegL{4,6}, x,x,      "├[-100~4[[4_6[[6_100[┤"_expect);
                                                        /////
          testCase (SegL{4,5,6,8}, 3,6,       "├[-100~3[[3_6[[6_8[[8~100[┤"_expect);      // spanning and thus replacing multiple segments
          testCase (SegL{4,5,6,8}, 4,6,       "├[-100~4[[4_6[[6_8[[8~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 4,7,       "├[-100~4[[4_7[[7_8[[8~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 3,7,       "├[-100~3[[3_7[[7_8[[8~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 3,8,       "├[-100~3[[3_8[[8~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 4,8,       "├[-100~4[[4_8[[8~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 4,9,       "├[-100~4[[4_9[[9~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 5,9,  "├[-100~4[[4_5[[5_9[[9~100[┤"_expect);
          testCase (SegL{4,5,6,8}, 5,x,  "├[-100~4[[4_5[[5_6[[6_8[[8~100[┤"_expect);
          testCase (SegL{4,5,7,8}, x,6,  "├[-100~4[[4_5[[5_6[[6_7[[7_8[[8~100[┤"_expect);
        }                                               /////
    };
  
  LAUNCHER (SplitSplice_test, "unit common");
  
  
}} // namespace lib::test
