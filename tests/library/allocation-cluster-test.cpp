/*
  AllocationCluster(Test)  -  verify bulk (de)allocating a family of objects

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file allocation-cluster-test.cpp
 ** unit test \ref AllocationCluster_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/allocation-cluster.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <array>
#include <vector>
#include <limits>
#include <functional>
//#include <boost/lexical_cast.hpp>

//using boost::lexical_cast;
using lib::explore;
using lib::test::showSizeof;
using util::isnil;
using ::Test;

using std::numeric_limits;
using std::function;
using std::vector;
using std::array;



namespace lib {
namespace test {
  
  namespace { // a family of test dummy classes
    
    const uint NUM_CLUSTERS = 5;
    const uint NUM_TYPES    = 20;
    const uint NUM_OBJECTS  = 500;
    
    long checksum = 0; // validate proper pairing of ctor/dtor calls
    
    template<uint i>
    class Dummy
      {
        static_assert (0 < i);
        array<uchar,i> content_;
        
      public:
        Dummy (uchar id=1)
          {
            content_.fill(id);
            checksum += explore(content_).resultSum();
          }
        
       ~Dummy()
          {
            checksum -= explore(content_).resultSum();
          }
        
        char getID()  { return content_[0]; }
      };
    
    
    template<uint i>
    void
    place_object (AllocationCluster& clu, uchar id)
    {
      clu.create<Dummy<i>> (id);
    }
    
    
    inline array<function<void(AllocationCluster&, uchar)>, NUM_TYPES>
    buildTrampoline()
    {
      return { place_object<1>
             , place_object<2>
             , place_object<3>
             , place_object<5>
             , place_object<10>
             , place_object<13>
             , place_object<14>
             , place_object<15>
             , place_object<16>
             , place_object<17>
             , place_object<18>
             , place_object<19>
             , place_object<20>
             , place_object<25>
             , place_object<30>
             , place_object<35>
             , place_object<40>
             , place_object<50>
             , place_object<100>
             , place_object<200>
             };
    }
    
    void
    fill (AllocationCluster& clu)
      {
        auto invoker = buildTrampoline();
        for (uint i=0; i<NUM_OBJECTS; ++i)
          invoker[rand() % NUM_TYPES] (clu, uchar(i));
      }
    
  }
  
  
  
  /*********************************************************************//**
   *  @test verify the proper workings of our custom allocation scheme
   *        managing families of interconnected objects for the segments
   *        of the low-level model.
   */
  class AllocationCluster_test : public Test
    {
      virtual void
      run (Arg)
        {
          simpleUsage();
          checkLifecycle();
        }
      
      
      void
      simpleUsage()
        {
          AllocationCluster clu;
          
          char c1(123), c2(45);
          Dummy<66>& ref1 = clu.create<Dummy<66>> ();
          Dummy<77>& ref2 = clu.create<Dummy<77>> (c1);
          Dummy<77>& ref3 = clu.create<Dummy<77>> (c2);
          
//          TRACE (test, "%s", showSizeof(rX).c_str());///////////////////////OOO
          
          //returned references actually point at the objects we created
          CHECK (1  ==ref1.getID());
          CHECK (123==ref2.getID());
          CHECK (45 ==ref3.getID());
          
          CHECK (1        == clu.numExtents());
          CHECK (66+77+77 == clu.numBytes());
          
          // now use objects and just let them go;
        }
      
      
      void
      checkLifecycle()
        {
          CHECK (0==checksum);
          {
            vector<AllocationCluster> clusters (NUM_CLUSTERS);
            for (auto& clu : clusters)
              fill(clu);
            CHECK (0!=checksum);
          }
          CHECK (0==checksum);
        }
    };
  
  LAUNCHER (AllocationCluster_test, "unit common");
  
  
}} // namespace lib::test
