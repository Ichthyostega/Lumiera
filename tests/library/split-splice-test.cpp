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
        
        Seg (int s, int a, bool nil=false)
          : start{s}
          , after{a}
          , empty{nil}
          , id{++idGen}
          {
            ++cnt;
            check += id;
          }
        
       ~Seg()
          {
            check -= id;
            if (id) --cnt;
          }
        
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
          TODO ("simple usage example");
        }
      
      
      /** @test verify the fixture and self-diagnostics for this test */
      void
      verify_testFixture()
        {
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
          UNIMPLEMENTED ("standard cases");
        }
      
      
      /**
       * @test cover special and boundary cases
       */
      void
      verify_cornerCases()
        {
          UNIMPLEMENTED ("corner cases");
        }
    };
  
  LAUNCHER (SplitSplice_test, "unit common");
  
  
}} // namespace lib::test
