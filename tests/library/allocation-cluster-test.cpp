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
#include "lib/format-util.hpp"/////////////TODO
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <functional>
#include <limits>
#include <vector>
#include <array>
#include <set>

using ::Test;
using lib::explore;
using lib::test::showSizeof;
using util::getAddr;
using util::isnil;

using std::numeric_limits;
using std::function;
using std::vector;
using std::array;
using std::byte;



namespace lib {
namespace test {
  
  namespace { // a family of test dummy classes
    
    const uint NUM_CLUSTERS = 5;
    const uint NUM_TYPES    = 20;
    const uint NUM_OBJECTS  = 500;
    
    const size_t BLOCKSIZ = 256;   ///< @warning actually defined in allocation-cluster.cpp
    
    int64_t checksum = 0;         // validate proper pairing of ctor/dtor calls
    
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
        
        uint getID()  { return content_[0]; }
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
    
    inline uint
    sum (uint n)   ///< sum of integers 1...N
    {
      return n*(n+1) / 2;
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
          verifyInternals();
          use_as_Allocator();
        }
      
      
      void
      simpleUsage()
        {
          AllocationCluster clu;
          CHECK (0 == clu.numExtents());
          
          char c1(123), c2(45);
          Dummy<66>& ref1 = clu.create<Dummy<66>> ();
          Dummy<77>& ref2 = clu.create<Dummy<77>> (c1);
          Dummy<77>& ref3 = clu.create<Dummy<77>> (c2);
          
          //returned references actually point at the objects we created
          CHECK (1  ==ref1.getID());
          CHECK (123==ref2.getID());
          CHECK (45 ==ref3.getID());
          
          CHECK (0 < clu.numExtents());
          
          // now use objects and just let them go;
        }
      
      
      /** @test Allocation cluster grows when adding objects,
       *        but discards all objects at once when going out of scope,
       *        optionally also invoking (or not invoking) destructors.
       * @remark no destructors are invoked for any objects allocated
       *        through the `createDisposable<TY>(args...)` interface,
       *        or for allocations though the standard allocator adapter.
       */
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
          
          int64_t allSum;
          {// can also be used without invoking any destructors
            AllocationCluster clu;
            for (uint i=0; i<NUM_OBJECTS; ++i)
              clu.createDisposable<Dummy<223>>();
            
            CHECK (clu.numExtents() == NUM_OBJECTS);
            CHECK (checksum == NUM_OBJECTS * 223);
            allSum = checksum;
          }// Memory discarded here without invoking any destructor....
          CHECK (allSum == checksum);
          checksum = 0;
        }
      
      
      
      /** @test cover some tricky aspects of the low-level allocator
       * @remark due to the expected leverage of AllocationCluster,
       *         an optimised low-level approach was taken on various aspects of storage management;
       *         the additional metadata overhead is a power of two, exploiting contextual knowledge
       *         about layout; moreover, a special usage-mode allows to skip invocation of destructors.
       *         To document these machinations, change to internal data is explicitly verified here.
       * @todo WIP 5/24 ✔ define ⟶ ✔ implement
       */
      void
      verifyInternals()
        {
          CHECK (0==checksum);
          long markSum;
          {
            AllocationCluster clu;
            // no allocation happened yet
            CHECK (0 == clu.numExtents());
            CHECK (0 == clu.numBytes());
            CHECK (nullptr == clu.storage_.pos);
            CHECK (      0 == clu.storage_.rest);
            
            // build a simple object
            auto& i1 = clu.create<uint16_t> (1 + uint16_t(rand()));
            CHECK (i1 > 0);
            CHECK (1 == clu.numExtents());
            CHECK (2 == clu.numBytes());
            CHECK (clu.storage_.pos != nullptr);
            CHECK (clu.storage_.pos == (& i1) + 1 ); // points directly behind the allocated integer
            CHECK (clu.storage_.rest == BLOCKSIZ - (2*sizeof(void*) +  sizeof(uint16_t)));
            
            // Demonstration: how to reconstruct the start of the current extent
            byte* blk = static_cast<std::byte*>(clu.storage_.pos);
            blk += clu.storage_.rest - BLOCKSIZ;
            CHECK(size_t(blk) < size_t(clu.storage_.pos));

            // some abbreviations for navigating the raw storage blocks...
            auto currBlock = [&]{
                                  byte* blk = static_cast<std::byte*>(clu.storage_.pos);
                                  blk += clu.storage_.rest - BLOCKSIZ;
                                  return blk;
                                };
            auto posOffset = [&]{
                                  return size_t(clu.storage_.pos) - size_t(currBlock());
                                };
            auto slot = [&](size_t i)
                                {
                                  size_t* slot = reinterpret_cast<size_t*> (currBlock());
                                  return slot[i];
                                };
            
            CHECK (blk == currBlock());
            // current storage pos: 2 »slots« of admin overhead plus the first allocated element
            CHECK (posOffset() == 2 * sizeof(void*) + sizeof(uint16_t));
            CHECK (slot(0) == 0); // only one extent, thus next-* is NULL
            
            // allocate another one
            uint16_t i1pre = i1;
            auto& i2 = clu.create<uint16_t> (55555);
            CHECK (posOffset() == 2 * sizeof(void*) + 2 * sizeof(uint16_t));
            CHECK (clu.storage_.rest == BLOCKSIZ - posOffset());
            // existing storage unaffected
            CHECK (i1 == i1pre);
            CHECK (i2 == 55555);
            CHECK (slot(0) == 0);   // no administrative data yet...
            CHECK (slot(1) == 0);
            
            // alignment is handled properly
            char& c1 = clu.create<char> ('X');
            CHECK (posOffset() == 2 * sizeof(void*) + 2 * sizeof(uint16_t) + sizeof(char));
            auto& i3 = clu.create<int32_t> (42);
            CHECK (posOffset() == 2 * sizeof(void*) + 2 * sizeof(uint16_t) + sizeof(char) + 3*sizeof(byte) + sizeof(int32_t));
            CHECK (i1 == i1pre);                                                         // ^^^^Alignment
            CHECK (i2 == 55555);
            CHECK (c1 == 'X');
            CHECK (i3 == 42);
            CHECK (slot(0) == 0);
            
            // deliberately fill up the first extent completely
            for (uint i=clu.storage_.rest; i>0; --i)
              clu.create<uchar> (i);
            CHECK (clu.storage_.rest == 0);                            // no space left in current extent
            CHECK (posOffset() == BLOCKSIZ);
            CHECK (clu.numBytes() == BLOCKSIZ - 2*sizeof(void*));      // now using all the rest behind the admin »slots«
            CHECK (clu.numExtents() == 1);
            CHECK (slot(0) == 0);
            CHECK (blk == currBlock());                                // but still in the initial extent
            
            // trigger overflow and allocation of second extent
            char& c2 = clu.create<char> ('U');
            CHECK (blk != currBlock());                                // allocation moved to a new extent
            CHECK (getAddr(c2) == currBlock() + 2*sizeof(void*));      // c2 resides immediately after the two administrative »slots«
            CHECK (clu.storage_.rest == BLOCKSIZ - posOffset());
            CHECK (clu.numBytes() == BLOCKSIZ - 2*sizeof(void*) + 1);  // accounted allocation for the full first block + one byte
            CHECK (clu.numExtents() == 2);                             // we have two extents now
            CHECK (slot(0) == size_t(blk));                            // first »slot« of the current block points back to previous block
            CHECK (i1 == i1pre);
            CHECK (i2 == 55555);
            CHECK (c1 == 'X');
            CHECK (c2 == 'U');
            CHECK (i3 == 42);
            
            // allocate a "disposable" object (dtor will not be called)
            size_t pp = posOffset();
            auto& o1 = clu.createDisposable<Dummy<2>> (4);
            CHECK (o1.getID() == 4);
            markSum = checksum;
            CHECK (checksum == 4+4);
            CHECK (alignof(Dummy<2>) == alignof(char));
            CHECK (posOffset() - pp == sizeof(Dummy<2>));              // for disposable objects only the object storage itself plus alignment
            
            // allocate a similar object,
            // but this time also enrolling the destructor
            pp = posOffset();
            auto& o2 = clu.create<Dummy<2>> (8);
            CHECK (o2.getID() == 8);
            CHECK (checksum == markSum + 8+8);
            CHECK (posOffset() - pp > sizeof(Dummy<2>) + 2*sizeof(void*));
            CHECK (slot(1) > 0);
            CHECK (size_t(&o2) - slot(1) == 2*sizeof(void*));          // Object resides in a Destructor frame,
            using Dtor = AllocationCluster::Destructor;                // ... which has been hooked up into admin-slot-1 of the current extent
            auto dtor = (Dtor*)slot(1);
            CHECK (dtor->next == nullptr);
            
            // any other object with non-trivial destructor....
            string rands = lib::test::randStr(9);
            pp = posOffset();
            string& s1 = clu.create<string> (rands);                   // a string that fits into the small-string optimisation
            CHECK (s1 == rands);
            
            CHECK (posOffset() - pp >= sizeof(string) + 2*sizeof(void*));
            CHECK (size_t(&s1) - slot(1) == 2*sizeof(void*));          // again the Destructor frame is placed immediately before the object
            auto dtor2 = (Dtor*)slot(1);                               // and it has been prepended to the destructors-list in current extent
            CHECK (dtor2->next == dtor);                               // with the destructor of o2 hooked up behind
            CHECK (dtor->next == nullptr);
            
            // provoke overflow into a new extent
            // by placing an object that does not fit
            // into the residual space in current one
            auto& o3 = clu.create<Dummy<223>> (3);
            CHECK (clu.numExtents() == 3);                             // a third extent has been opened to accommodate this object
            CHECK (checksum == markSum + 8+8 + uchar(223*3));
            auto dtor3 = (Dtor*)slot(1);
            CHECK (dtor3->next == nullptr);                            // Destructors are chained for each extent separately
            CHECK (dtor3 != dtor2);
            CHECK (dtor2->next == dtor);                               // the destructor chain from previous extent is also still valid
            CHECK (dtor->next == nullptr);
            
            CHECK (i1 == i1pre);                                       // all data is intact (no corruption)
            CHECK (s1 == rands);
            CHECK (i2 == 55555);
            CHECK (c1 == 'X');
            CHECK (c2 == 'U');
            CHECK (i3 == 42);
            CHECK (o1.getID() == 4);
            CHECK (o2.getID() == 8);
            CHECK (o3.getID() == 3);
          }
          CHECK (checksum == markSum);                                 // only the destructor of the "disposable" object o1 was not invoked
        }
      
      
      
          template<typename X>
          using Allo = AllocationCluster::Allocator<X>;
      
      /** @test demonstrate use as Standard-Allocator
       *      - define a vector, string and set to use the AllocationCluster as backend
       *      - fill the vector with numbers and the set with random strings
       *  @note the extent size (hard coded as of 5/24) imposes a serious limitation
       *        regarding usable data structures; e.g. the std::deque immediately attempts
       *        to allocate a node buffer with >500 bytes, which is not supported by
       *        the current (rather simplistic) storage manager in AllocationCluster.
       */
      void
      use_as_Allocator()
        {
          using VecI = std::vector<uint16_t, Allo<uint16_t>>;
          using Strg = std::basic_string<char, std::char_traits<char>, Allo<char>>;
          using SetS = std::set<Strg, std::less<Strg>, Allo<Strg>>;
          
          AllocationCluster clu;
          CHECK (clu.numExtents() == 0);
          
          VecI veci{clu.getAllocator<uint16_t>()};
          
          // Since vector needs a contiguous allocation,
          // the maximum number of elements is limited by the Extent size (256 bytes - 2*sizeof(void*))
          // Moreover, the vector grows its capacity; AllocationCluster does not support re-allocation,
          // and thus the initial smaller memory chunks will just be abandoned.
          const uint MAX = 64;
          
          for (uint i=1; i<=MAX; ++i)
            veci.push_back(i);
          CHECK (clu.numExtents() == 2);
          CHECK (veci.capacity() == 64);
          
          // fill a set with random strings...
          SetS sets{clu.getAllocator<Strg>()};

          for (uint i=0; i<NUM_OBJECTS; ++i)
            sets.emplace (test::randStr(32), clu.getAllocator<char>());
          CHECK (sets.size() > 0.9 * NUM_OBJECTS);
          CHECK (clu.numExtents() > 200);
          
          // verify the data in the first allocation is intact
          CHECK (explore(veci).resultSum() == sum(64));
        }
    };
  
  LAUNCHER (AllocationCluster_test, "unit common");
  
  
}} // namespace lib::test
